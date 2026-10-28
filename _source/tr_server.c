
/* Tr_server.c */

#include "FreeRTOS.h"
#include <semphr.h>
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "xparameters.h"
#include "FreeRTOSConfig.h"
#include "config.h"
#include "task_config.h"
#include "xuartlite_l.h"
#include "repeater.h"
#include "protocol_private.h"
#include "config.h"
#include "uart.h"
#include "protocol.h"
#include "ir_client.h"
#include "protocol_ad4t.h"

extern mu_dev_info mu_info;

static char					rx_buf[MAX_RX_BUF_SIZE];
static char					tx_buf[MAX_TX_BUF_SIZE];


#define TR_QUEUE_LENGTH          1U
#define TR_PORT_TIMEOUT_MS       2000U

#define TR_STATUS_OK             0
#define TR_STATUS_TIMEOUT        1
#define TR_STATUS_ERROR          2

SemaphoreHandle_t TR_PORT = NULL;
static QueueHandle_t tr_mque = NULL;

#if 1
int initTrMessageQueue(void)
{
#if _Semaphore
    /*
     * If TR_PORT is intended for port access protection, a Mutex is suitable.
     * A mutex is in a usable state immediately after creation.
     */
    TR_PORT = xSemaphoreCreateMutex();

    if (TR_PORT == NULL)
    {
            printf("TR_PORT create error!!\n");
        return 0;
    }

    /*
     * A queue that stores a single `void *` pointer.
     *
     * importance
     * You should use sizeof(void *).
     */
    tr_mque = xQueueCreate(TR_QUEUE_LENGTH, sizeof(void *));

    if (tr_mque == NULL)
    {
        printf("TR message queue create error!!\n");

        vSemaphoreDelete(TR_PORT);
        TR_PORT = NULL;

        return 0;
    }
#endif
    return 1;
}

/*-----------------------------------------------------------
 * TR_PORT Mutex
 *----------------------------------------------------------*/

/*
 * Infinite Waiting
 * FreeRTOS:
 * xSemaphoreTake(TR_PORT, portMAX_DELAY);
 */
int getPortTr(void)
{
#if _Semaphore
    if (TR_PORT == NULL)
    {
	    printf("TR_PORT is NULL!!\n");
        return 0;
    }

    if (xSemaphoreTake(TR_PORT, portMAX_DELAY) != pdTRUE)
    {
        printf("TR_PORT take error!!\n");
        return 0;
    }
#endif
    return 1;
}




/*
 * Wait for 2 seconds.
 *
 * 반환값:
 * TR_STATUS_OK      : Acquisition successful
 * TR_STATUS_TIMEOUT : Time limit exceeded
 * TR_STATUS_ERROR   : Initialization error
 */
int getPortTrTimeOut(void)
{
#if _Semaphore
    if (TR_PORT == NULL)
    {
        printf("TR_PORT is NULL!!\n");
        return TR_STATUS_ERROR;
    }

    if (xSemaphoreTake(TR_PORT,pdMS_TO_TICKS(TR_PORT_TIMEOUT_MS)) != pdTRUE)
    {
        printf("TR_PORT timeout!!\n");
        return TR_STATUS_TIMEOUT;
    }
#endif
    return TR_STATUS_OK;
}

int putPortTr(void)
{
#if _Semaphore
    if (TR_PORT == NULL)
        return 0;

    if (xSemaphoreGive(TR_PORT) != pdTRUE)
    {
        printf("TR_PORT give error!!\n");
        return 0;
    }
#endif
    return 1;
}

/*-----------------------------------------------------------
 * Message Queue
 *----------------------------------------------------------*/

/*
 * Task에서 메시지 포인터 송신
 *
 * Queue가 가득 차 있으면 즉시 실패합니다.
 * 이는 OSQPost()의 non-blocking 동작과 유사합니다.
 */
int postTrMsgBox(void *pmsg)
{
#if _Semaphore
    if ((tr_mque == NULL) || (pmsg == NULL))
        return 0;

    /*
     * Queue에 저장할 값은 pmsg 포인터입니다.
     * 따라서 &pmsg를 전달해야 합니다.
     */
    if (xQueueSend(tr_mque, &pmsg, 0) != pdPASS)
    {
        printf("TR message queue full!!\n");
        return 0;
    }
#endif
    return 1;
}

/*
 * timeout 단위는 FreeRTOS Tick입니다.
 *
 * timeout == 0 : 무한 대기
 * timeout > 0  : 지정된 Tick만큼 대기
 */
void *pendTrMsgBox(int timeout)
{
    void *pmsg = NULL;
    TickType_t waitTicks;
#if _Semaphore

    if (tr_mque == NULL)
        return NULL;

    /*
     * μC/OS에서 timeout=0은 무한 대기이지만,
     * FreeRTOS에서 block time=0은 즉시 반환입니다.
     * 따라서 명시적으로 변환해야 합니다.
     */
    if (timeout == 0)
        waitTicks = portMAX_DELAY;
    else
        waitTicks = (TickType_t)timeout;

    if (xQueueReceive(tr_mque,
                      &pmsg,
                      waitTicks) != pdPASS)
    {
        return NULL;
    }
#endif
    return pmsg;
}


#endif


/* getPacket() : receive packet
 *	return value
 *		1. cmd    : ????? ????? ????? ????? ??????? ??? ?????? ??????? ???????.
 *		2. tmppkt : ????? ????? ??? ??????? ????.
 */
static int __getPacket(short uart_index, char *cmd, char *tmppkt, char *err, frame_info *fr_info)
{
	char			*pkt;
	unsigned char 	pkt_cmd;
	unsigned short 	pkt_len;
	unsigned short 	len;
	unsigned short 	crc, pkt_crc=0;

	/*	4Byte : SYNC (0x16,0x16,0x16,0x16) */
    if (getsync(uart_index) < 0)
	{
		sprintf(err, "getsync failed");
    	return (-1);
	}

    pkt = tmppkt;
	len = 0;

	/* 4 bytes	Source ID */
	fr_info->srcID_type = *(pkt + len++) = uartRead(uart_index);
	fr_info->srcID_branch = *(pkt + len++) = uartRead(uart_index);
	fr_info->srcID_port = *(pkt + len++) = uartRead(uart_index);
	fr_info->srcID_id = *(pkt + len++) = uartRead(uart_index);

	/* 4 bytes	Destination  ID */
	fr_info->destID_type = *(pkt + len++) = uartRead(uart_index);
	fr_info->destID_branch = *(pkt + len++) = uartRead(uart_index);
	fr_info->destID_port = *(pkt + len++) = uartRead(uart_index);
	fr_info->destID_id = *(pkt + len++) = uartRead(uart_index);

	/* cmd */
	pkt_cmd = *(pkt + len++) = uartRead(uart_index);

	/* length */
	*(pkt + len++) = uartRead(uart_index) & 0xFF;
	*(pkt + len++) = uartRead(uart_index) & 0xFF;

	pkt_len = pkt[len-2] & 0xFF;
	pkt_len <<= 8;
	pkt_len |= pkt[len-1] & 0xFF;

	if(pkt_len > 1500 ) {
		//mon_printf("len error: %d\n",pkt_len);
		sprintf(err, "pkg len too high : max %d, current %d (0x%x)", 1500, pkt_len, pkt_len);
		return (-2);
	}

#if PROTO_DEBUG | 0
	str_len += sprintf(&err[str_len], "cmd:%x\n",pkt_cmd);
	str_len += sprintf(&err[str_len], "len:%d:%x\n",pkt_len,pkt_len);
#endif

	uartReads(uart_index, pkt+len, pkt_len);
	len+=pkt_len;

	/* get CRC */
	pkt_crc = (char)uartRead(uart_index);
	pkt_crc <<= 8;
	pkt_crc |= (char)uartRead(uart_index) & 0xFF;

	// CRC calculation based on received data
	crc = Generate_CRC((char *)(pkt), len);


	if (crc != pkt_crc) {
#if PROTO_DEBUG | 0
		str_len += sprintf(&err[str_len], "crc error : %x<->%x | len=%d\n",crc,pkt_crc,len);
#endif
	//mon_printf("crc error : %x<->%x | len=%d\n",crc,pkt_crc,len);

		sprintf(err, "crc error, calculated 0x%04X, current read 0x%04X, msg len %d", crc, pkt_crc, len);
		return (-2);
}
	*cmd = pkt_cmd;

	// Debug
//	xil_printf("-->[%s] cmd 0x%x leng %d: ", dev_map_array[uart_index], *cmd, len);
//	logPackage(tmppkt, len);

	return (len);
}


void vTrServerTaskSAU()
{
	vTrServerWorker(UART_TR);
	//vTrServerWorkerTest(UART_TR);
}

void vTrServerWorker(short uart_index) {
	struct repeater_struct *prpt;
	prpt = getRptDev();

	unsigned char 			cmd;
	int						len;
	int 					tx_len;
	char 					errMsg[128] = "";
	frame_info 				fr_info;
	frame_info              tx_fr_info;
//	char 					rx_buf[MAX_RX_BUF_SIZE];
//	char                    tr_tx_buf[SHORT_TX_BUF_SIZE];	  
	unsigned char			source_id[4] = {0};
	unsigned char			dest_id[4] = {0};

	xil_printf("SAU TR server started ...\r\n");

    while (1) {
    	len = __getPacket(uart_index, (char *)&cmd, rx_buf, errMsg, &fr_info);

    	if (len < 0) {
			continue;
		}

		source_id[0] = fr_info.srcID_type;
		source_id[1] = fr_info.srcID_branch;
		source_id[2] = fr_info.srcID_port;
		source_id[3] = fr_info.srcID_id;
		
		// 4 bytes destination ID
		dest_id[0] = fr_info.destID_type;
		dest_id[1] = fr_info.destID_branch;
		dest_id[2] = fr_info.destID_port;
		dest_id[3] = fr_info.destID_id;

		if(source_id[0] == DEV_TYPE_SAU)
		{
			prpt->Tree.sau_pri = 1;
			prpt->Tree.sau_pri_mu = 1;
		}

		if(source_id[0] == DEV_TYPE_MU)
		{
			prpt->Tree.sau_pri_mu = 1;
		}		
		
		#if 0
		if (fr_info.destID_type == DEV_TYPE_RU)
		{
			// Message will be forwarded to BRx port if destination is RU:
			xil_printf("Forward message to RU:\r\n");
			logHeader(uart_index, cmd, &fr_info);
			forwardFrame(cmd, rx_buf, len, &mu_info, &fr_info);
			continue;
		}
		#endif
		
		prpt->tr_rxcount++;

		switch (cmd)
		{
	    	case TREE_REQ:
//	    		len = TreeAck(tx_buf, source_id, dest_id, TREE_ACK);
//				setTrIrSel(1,1);
//				usleep(1000);
//				UART_Writes(UART_TYPE_TR, tx_buf, len);
//				usleep(1000);
//				setTrIrSel(1,0);
				xil_printf("TR TREE_REQ\r\n");
				break;

	    	case TREE_ACK:
				xil_printf("TR TREE_ACK\r\n");

				break;

	    	case TOPOLOGY_ACK:
				xil_printf("TR TOPOLOGY_ACK\r\n");

				break;

			case TOPOLOGY_REQ:		
				// This message shall be sent only from SAU
				if (fr_info.srcID_type != DEV_TYPE_SAU) {
					
					// Invalid source type, send an error response
					sprintf(errMsg, "Invalid source type for TOPOLOGY_REQ: %d", fr_info.srcID_type);
					tx_len = errorStatus(tx_buf, errMsg);
					uartWrites(uart_index, tx_buf, tx_len);
					xil_printf("[FATAL] %s\r\n", errMsg);
					continue;
				}

				// DestID must be MU
				if (fr_info.destID_type != DEV_TYPE_MU) {
					char  tx_buf[SHORT_TX_BUF_SIZE];
					// Invalid destination type, send an error response
					sprintf(errMsg, "Invalid destination type for TOPOLOGY_REQ: %d", fr_info.destID_type);
					tx_len = errorStatus(tx_buf, errMsg);
					uartWrites(uart_index, tx_buf, tx_len);
					xil_printf("[FATAL] %s\r\n", errMsg);
					continue;
				}

				// Set info from message header				
				mu_info.mu.branch_id = fr_info.destID_branch;
				mu_info.mu.port_id = fr_info.destID_port;
				mu_info.mu.device_id = fr_info.destID_id;
				
				// Wait for all RU answered or timeout
				WaitForTopologyDone();

				// Send TOPOLOGY response to SAU
				logMuInfo(&mu_info);
				tx_len = TopologyAck(tx_buf, &mu_info, &fr_info);
				uartWrites(uart_index, tx_buf, tx_len);
				break;

			case MAIN_STAT_REQ:		
				setFrameInfo(&fr_info, &tx_fr_info);		
				tx_len = MainStatAck(tx_buf, &fr_info,MAIN_STAT_ACK);
				uartWrites(uart_index, tx_buf, tx_len);
				break;
				
			default:
				xil_printf("[FATAL] Invalid message received:\r\n");
				logHeader(uart_index, cmd, &fr_info);
				break;
		}
	}
}

void init_Tr_server(void) {

	// Fill test MU devices
	//fillTestMuInfo(&mu_info);

	//Create the TR-Server Task for SAU
    BaseType_t xRet = xTaskCreate(vTrServerTaskSAU,
                                 "TR-Server SAU",
                                 OS_TASK_CONSOLE_STK_SIZE,
								 (void *)0,
                                 OS_TASK_TR_SERVER_PRIO,
                                 NULL);
    if (xRet != pdPASS) {
        printf("[FATAL] Could not create TR-Server Task SAU!\r\n");
    }	
}


// void vTrServerWorkerTest(short uart_index) {
// 	char *buf[64];
// 	int len;

// 	len = UART_Reads(uart_index, buf, 20);
// 	xil_printf("[%d]:%s\r\n", len, buf);
// }

void WaitForTopologyDone()
{
	short index;
	bool done = true;

	// 1. Step: check the BR response status
	for (index = 0; index < RU_NUM_MAX; index++)
	{
		done &= (mu_info.top_status[index] >= TOPOLOGY_COMPLETED);
	}

	// 2. Step: if one BR response is not finish, wait for 1 sec since start	
	while (!done)
	{
		TickType_t curTick = xTaskGetTickCount();
		done = (pdTICKS_TO_MS((curTick - mu_info.top_start)) >= 1000);  // 1000 ms = 1sec
		vTaskDelay(pdMS_TO_TICKS(100));
	}	
}

