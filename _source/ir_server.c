
/* Ir_server.c */

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
#include "genlib.h"
#include "psa.h"
#include "fpga.h"
#include "protocol_private.h"
#include "config.h"
#include "uart.h"
#include "protocol.h"
#include "ir_client.h"

extern unsigned char Uart_Test;


extern mu_dev_info mu_info;


#define IR_QUEUE_LENGTH          1U
#define IR_PORT_TIMEOUT_MS       2000U

#define IR_STATUS_OK             0
#define IR_STATUS_TIMEOUT        1
#define IR_STATUS_ERROR          2

SemaphoreHandle_t IR_PORT = NULL;
static QueueHandle_t ir_mque = NULL;


extern int IR_TX_Cnt;
extern int IR_RX_Cnt;

#define TR_TX_MAX_PACKET_SIZE			1100		// FSK TX 3500
#define TR_RX_MAX_PACKET_SIZE			1100		// FSK RX 1100


char tr_rx_buf[TR_TX_MAX_PACKET_SIZE];
char tr_tx_buf[TR_TX_MAX_PACKET_SIZE];
int tr_rx_buf_Cnt = 0;
extern u8 Debug_Prn;

#if 1
int initIrMessageQueue(void)
{
#if _Semaphore
    /*
     * If TR_PORT is intended for port access protection, a Mutex is suitable.
     * A mutex is in a usable state immediately after creation.
     */
    IR_PORT = xSemaphoreCreateMutex();

    if (IR_PORT == NULL)
    {
            printf("IR_PORT create error!!\n");
        return 0;
    }

    /*
     * A queue that stores a single `void *` pointer.
     *
     * importance
     * You should use sizeof(void *).
     */
    ir_mque = xQueueCreate(IR_QUEUE_LENGTH, sizeof(void *));

    if (ir_mque == NULL)
    {
        printf("IR message queue create error!!\n");

        vSemaphoreDelete(IR_PORT);
        IR_PORT = NULL;

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
int getPortIr(void)
{
#if _Semaphore
    if (IR_PORT == NULL)
    {
	    printf("IR_PORT is NULL!!\n");
        return 0;
    }

    if (xSemaphoreTake(IR_PORT, portMAX_DELAY) != pdTRUE)
    {
        printf("IR_PORT take error!!\n");
        return 0;
    }
#endif
    return 1;
}


/*
 * Wait for 2 seconds.
 *
 * ��ȯ��:
 * IR_STATUS_OK      : Acquisition successful
 * IR_STATUS_TIMEOUT : Time limit exceeded
 * IR_STATUS_ERROR   : Initialization error
 */
int getPortIrTimeOut(void)
{
#if _Semaphore
    if (IR_PORT == NULL)
    {
        printf("IR_PORT is NULL!!\n");
        return IR_STATUS_ERROR;
    }

    if (xSemaphoreTake(IR_PORT,pdMS_TO_TICKS(IR_PORT_TIMEOUT_MS)) != pdTRUE)
    {
        printf("IR_PORT timeout!!\n");
        return IR_STATUS_TIMEOUT;
    }
#endif
    return IR_STATUS_OK;
}

int putPortIr(void)
{
#if _Semaphore
	if (IR_PORT == NULL)
        return 0;

    if (xSemaphoreGive(IR_PORT) != pdTRUE)
    {
        printf("IR_PORT give error!!\n");
        return 0;
    }
#endif
    return 1;
}

/*-----------------------------------------------------------
 * Message Queue
 *----------------------------------------------------------*/
/*
 * Sending a message pointer from a task
 *
 * It fails immediately if the queue is full.
 * This is similar to the non-blocking behavior of OSQPost().
 */
int postIrMsgBox(void *pmsg)
{
#if _Semaphore
    if ((ir_mque == NULL) || (pmsg == NULL))
        return 0;

    /*
     * Queue�� ������ ���� pmsg �������Դϴ�.
     * ���� &pmsg�� �����ؾ� �մϴ�.
     */
    if (xQueueSend(ir_mque, &pmsg, 0) != pdPASS)
    {
        printf("IR Mu message queue full!!\n");
        return 0;
    }
#endif
    return 1;
}

/*
 * The timeout unit is FreeRTOS ticks.
 *
 * timeout == 0 : Infinite Waiting
 * timeout > 0  : Wait for the specified number of ticks
 */
void *pendIrMsgBox(int timeout)
{
    void *pmsg = NULL;
    TickType_t waitTicks;
#if _Semaphore
    if (ir_mque == NULL)
        return NULL;

    /*
     * In ��C/OS, timeout=0 is an infinite wait, but,
     * In FreeRTOS, a block time of 0 results in an immediate return.
     * Therefore, you must explicitly convert it..
     * 400 4Sec
     */
    if (timeout == 0)
        waitTicks = portMAX_DELAY;
    else
        waitTicks = (TickType_t)timeout;

    if (xQueueReceive(ir_mque,
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
//static int __getPacket(short uart_index, char *cmd, char *tmppkt, char *err, frame_info *fr_info)
//{
//	char			*pkt;
//	unsigned char 	pkt_cmd;
//	unsigned short 	pkt_len;
//	unsigned short 	len;
//	unsigned short 	crc, pkt_crc=0;
//
//	/*	4Byte : SYNC (0x16,0x16,0x16,0x16) */
//    if (getsync(uart_index) < 0)
//	{
//		sprintf(err, "getsync failed");
//    	return (-1);
//	}
//
//    pkt = tmppkt;
//	len = 0;
//
//	/* 4 bytes	Source ID */
//	fr_info->srcID_type = *(pkt + len++) = uartRead(uart_index);
//	fr_info->srcID_branch = *(pkt + len++) = uartRead(uart_index);
//	fr_info->srcID_port = *(pkt + len++) = uartRead(uart_index);
//	fr_info->srcID_id = *(pkt + len++) = uartRead(uart_index);
//
//	/* 4 bytes	Destination  ID */
//	fr_info->destID_type = *(pkt + len++) = uartRead(uart_index);
//	fr_info->destID_branch = *(pkt + len++) = uartRead(uart_index);
//	fr_info->destID_port = *(pkt + len++) = uartRead(uart_index);
//	fr_info->destID_id = *(pkt + len++) = uartRead(uart_index);
//
//	/* cmd */
//	pkt_cmd = *(pkt + len++) = uartRead(uart_index);
//
//	/* length */
//	*(pkt + len++) = uartRead(uart_index) & 0xFF;
//	*(pkt + len++) = uartRead(uart_index) & 0xFF;
//
//	pkt_len = pkt[len-2] & 0xFF;
//	pkt_len <<= 8;
//	pkt_len |= pkt[len-1] & 0xFF;
//
//	if(pkt_len > 1500 ) {
//		//mon_printf("len error: %d\n",pkt_len);
//		sprintf(err, "pkg len too high : max %d, current %d (0x%x)", 1500, pkt_len, pkt_len);
//		return (-2);
//	}
//
//#if PROTO_DEBUG | 0
//	str_len += sprintf(&err[str_len], "cmd:%x\n",pkt_cmd);
//	str_len += sprintf(&err[str_len], "len:%d:%x\n",pkt_len,pkt_len);
//#endif
//
//	uartReads(uart_index, pkt+len, pkt_len);
//	len+=pkt_len;
//
//	/* get CRC */
//	pkt_crc = (char)uartRead(uart_index);
//	pkt_crc <<= 8;
//	pkt_crc |= (char)uartRead(uart_index) & 0xFF;
//
//	// CRC calculation based on received data
//	crc = Generate_CRC((char *)(pkt), len);
//
//
//	if (crc != pkt_crc) {
//#if PROTO_DEBUG | 0
//		str_len += sprintf(&err[str_len], "crc error : %x<->%x | len=%d\n",crc,pkt_crc,len);
//#endif
//	//mon_printf("crc error : %x<->%x | len=%d\n",crc,pkt_crc,len);
//
//		sprintf(err, "crc error, calculated 0x%04X, current read 0x%04X, msg len %d", crc, pkt_crc, len);
//		return (-2);
//}
//	*cmd = pkt_cmd;
//
//	// Debug
////	xil_printf("-->[%s] cmd 0x%x leng %d: ", dev_map_array[uart_index], *cmd, len);
////	logPackage(tmppkt, len);
//
//	return (len);
//}

static int __getPacket(short uart_index, char *cmd, char *tmppkt, char *err, frame_info *fr_info)
{
	char			*pkt;
	unsigned char 	pkt_cmd, byte_check;
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
	*(pkt + len++) = 0x16;
	*(pkt + len++) = 0x16;
	*(pkt + len++) = 0x16;
	*(pkt + len++) = 0x16;

	byte_check = (char)uartRead(uart_index);
	if( byte_check != 0x08){
		/* 4 bytes	Source ID */
//		fr_info->srcID_type = *(pkt + len++) = uartRead(uart_index);
		fr_info->srcID_type = *(pkt + len++) = (char)byte_check;
		fr_info->srcID_branch = *(pkt + len++) = uartRead(uart_index);
		fr_info->srcID_port = *(pkt + len++) = uartRead(uart_index);
		fr_info->srcID_id = *(pkt + len++) = uartRead(uart_index);

		/* 4 bytes	Destination  ID */
		fr_info->destID_type = *(pkt + len++) = uartRead(uart_index);
		fr_info->destID_branch = *(pkt + len++) = uartRead(uart_index);
		fr_info->destID_port = *(pkt + len++) = uartRead(uart_index);
		fr_info->destID_id = *(pkt + len++) = uartRead(uart_index);
	}
	else
	{
		*(pkt + len++) = (char)byte_check;
	}

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

	// CRC calculation based on received data
	crc = Generate_CRC((char *)(pkt+4), len-4);
	/* get CRC */
	pkt_crc = *(pkt + len++) = (char)uartRead(uart_index);
	pkt_crc <<= 8;
	pkt_crc |= *(pkt + len++) = (char)uartRead(uart_index) & 0xFF;



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
void vIrServerTaskRuBr1()
{
	vIrServerWorker(UART_IR_BR1);
	//vIrServerWorkerTest(UART_IR_BR1);
}

void vIrServerTaskRuBr2()
{
	vIrServerWorker(UART_IR_BR2);
	//vIrServerWorkerTest(UART_IR_BR2);
}

void vIrServerTaskRuBr3()
{
	vIrServerWorker(UART_IR_BR3);
	//vIrServerWorkerTest(UART_IR_BR3);
}

void vIrServerTaskRuBr4()
{
	vIrServerWorker(UART_IR_BR4);
	//vIrServerWorkerTest(UART_IR_BR4);
}

void vIrServerWorker(short uart_index) {

	unsigned char 			cmd;
	int						len, status=0, retval = 0;

	static char 			errMsg[128] = "";
	struct tbl_desc			*pdesc;
	unsigned char			tblType;
//	short					source_id;
//	short					dest_id;
	frame_info 				fr_info;
	static char 			rx_buf[MAX_RX_BUF_SIZE];
	static 					char tr_tx_buf[MAX_TX_BUF_SIZE];  // Buffer for transmitting data
	unsigned char			source_id[4] = {0};
	unsigned char			dest_id[4] = {0};
	unsigned int 			rx_len;

	struct repeater_struct  *prpt;
	prpt = getRptDev();

	xil_printf("IR server %d started ...\r\n", uart_index - 1);
#if 0
	for (;;) {
		len = uartRead(uart_index);
		if (len != -1) {
			uartWrite(UART_CLI,(char)len);
		}
	}
#endif	
#if 1
    while (1) {
    	len = __getPacket(uart_index, (char *)&cmd, rx_buf, errMsg, &fr_info);
    	if (len > 0) {
    	    	    xil_printf("--> Received packet len: %d\r\n", len);
    	    	    for (int i = 0; i < len; i++) {
    	    	        xil_printf("%02X ", (unsigned char)rx_buf[i]);
    	    	    }
    	    	    xil_printf("\r\n");
    	    	} else {
    	    	    xil_printf("Get packet failed, error code: %d, msg: %s\r\n", len, errMsg);
    	    	}

//		xil_printf("MU	[%d][%x]\r\n", status,cmd);
//
//    	if (len < 0) {
//			xil_printf("[FATAL] IR server read protocol frame failed rc=%d\r\n", status);
//			continue;
//		}


		source_id[0] = fr_info.srcID_type;
		source_id[1] = fr_info.srcID_branch;
		source_id[2] = fr_info.srcID_port;
		source_id[3] = fr_info.srcID_id;
		
		// 4 bytes destination ID
		dest_id[0] = fr_info.destID_type;
		dest_id[1] = fr_info.destID_branch;
		dest_id[2] = fr_info.destID_port;
		dest_id[3] = fr_info.destID_id;		
		// if(dest_id[0] == DEV_TYPE_LMT){
		// 		// len  = forwardFrame(cmd, rx_buf ,len, &fr_info);
		// 		// continue;
		// }
		tr_rx_buf_Cnt = len;
		memcpy(tr_rx_buf, rx_buf, len);
		postIrMsgBox((void *)tr_rx_buf);
		prpt->ir_br_rxcount[dest_id[2]]++;
		switch (cmd) {

			case TREE_REQ:
				if(Debug_Prn)
					xil_printf("IR TREE_REQ\r\n");
				break;

			case TREE_ACK:
				if(Debug_Prn)
					xil_printf("IR TREE_ACK\r\n");
				break;

			case MAIN_STAT_ACK:
				MainStatReq_RU(rx_buf, &source_id, &dest_id, 0);


				if(Debug_Prn)
					//xil_printf("MAIN_STAT_ACK[%d]\r\n",dest_id[2]);
				break;

			case TOPOLOGY_ACK:
//				prpt->BR_Install
				//xil_printf("IR TOPOLOGY_ACK[%x][%x][%x][%x]\r\n",dest_id[0],dest_id[1],dest_id[2],dest_id[3]);
				prpt->br_insert[dest_id[2]] = 1;

				IR_TopologyStatReq(rx_buf + 4, &source_id, &dest_id);

				prpt->children_ru_number[dest_id[2]] = prpt->ir_children_ru_number;


				if(prpt->curr_ir_children_ru_number == prpt->ir_children_ru_number)
				{
					prpt->TOPOLOGY_cnt +=1;
				}
				else
				{
					prpt->TOPOLOGY_cnt = 0;
					prpt->curr_ir_children_ru_number = prpt->ir_children_ru_number;
				}
				
				break;
			

		default :
    		break;

		}
		continue;

		#if 0

		if (cmd == ERROR_STATUS)
		{
			xil_printf("Error: %s\r\n", rx_buf);
		}
		else if (cmd == TOPOLOGY_RESP)
		{
			// Handle topology response message
			// This message shall be sent only from RU
			if (fr_info.srcID_type != DEV_TYPE_RU) {
				// Invalid source type, send an error response
				sprintf(errMsg, "Invalid source type for TOPOLOGY_RESP: %d", fr_info.srcID_type);
				len = errorStatus(tr_tx_buf, errMsg);
				uartWrites(uart_index, tr_tx_buf,len);
				xil_printf("%s\r\n", errMsg);
				break;
			}

			// logHeader(uart_index, cmd, &fr_info);

			// Update topology
			updateRuInfo(rx_buf, status, &fr_info, &mu_info, uart_index);

			// Debug
			logMuInfo(&mu_info);
		}
		else if (fr_info.destID_type == DEV_TYPE_MU)
		{
			// Handle message
		}
		else if (fr_info.destID_type == DEV_TYPE_SAU)
		{
			// Message will be forwarded to TR port to SAU:
			xil_printf("Forward message to SAU:\r\n");
			logHeader(uart_index, cmd, &fr_info);
			forwardFrame(cmd, rx_buf, len, &mu_info, &fr_info);
		}
		#endif
	}
#endif
}



// void vIrServerTaskTest() {
//    while (1) {

//    	int ch;
//    	ch = UART_IR_BR2_ReturnChar();
// 	xil_printf("--> [BR2]  '%c'\r\n", ch);
//    	if (ch != -1) {
//    		uartWrite(UART_CLI, (char)ch);
// 		}
// 	}
// }

void init_Ir_server(void) {
	initIrMessageQueue();

	BaseType_t xRet;
	//Create the Ir-Server Task for RU BR1
    xRet = xTaskCreate(vIrServerTaskRuBr1,
                                 "Ir-Server RU BR1",
                                 APP_TASK_START_STK_SIZE,
								 (void *)0,
                                 OS_TASK_IR_SERVER_PRIO,
                                 NULL);
    if (xRet != pdPASS) {
        printf("[FATAL] Could not create Ir-Server Task RU BR1!\r\n");
    }

	// Create the Ir-Server Task for RU BR2
    xRet = xTaskCreate(vIrServerTaskRuBr2,
                                 "Ir-Server RU BR2",
                                 APP_TASK_START_STK_SIZE,
								 (void *)0,
                                 OS_TASK_IR_SERVER_PRIO,
                                 NULL);
    if (xRet != pdPASS) {
        printf("[FATAL] Could not create Ir-Server Task RU BR2!\r\n");
    }
	//Create the Ir-Server Task for RU BR3
    xRet = xTaskCreate(vIrServerTaskRuBr3,
                                 "Ir-Server RU BR3",
                                 APP_TASK_START_STK_SIZE,
								 (void *)0,
                                 OS_TASK_IR_SERVER_PRIO,
                                 NULL);

    if (xRet != pdPASS) {
        printf("[FATAL] Could not create Ir-Server Task RU BR3!\r\n");
    }
	// Create the Ir-Server Task for RU BR4
    xRet = xTaskCreate(vIrServerTaskRuBr4,
                                 "Ir-Server RU BR4",
                                 APP_TASK_START_STK_SIZE,
								 (void *)0,
                                 OS_TASK_IR_SERVER_PRIO,
                                 NULL);
    if (xRet != pdPASS) {
        printf("[FATAL] Could not create Ir-Server Task RU BR3!\r\n");
    }
}


void vIrServerWorkerTest(short uart_index) {
	char *buf[64];
	int len;

	len = uartReads(uart_index, buf, 20);
	xil_printf("[%d]:%s\r\n", len, buf);
}

