#include "uart.h"
#include "ir_client.h"
#include "task.h"
#include "task_config.h"
#include "sleep.h"
#include "repeater.h"
#include "protocol_private.h"
#include "protocol.h"


extern unsigned char Uart_Test;

static char tx_buf[2500]; 	 //2048
static char rx_buf[2500];    //2048

#define IR_TIMEOUT    100  //1second  400 500

int forwardFrame(int cmd, char *rx_buf, unsigned int rx_len, mu_dev_info *p_mu, frame_info *orig_fr_info)
{
    int tx_len = 0;
    short uart_index = 0;
    frame_info fw_fr_info;
    short dest_id = orig_fr_info->destID_id;
 
    // Check dest type: only RU, SAU_M, SAU_S are valid. If dest type is RU, we cannot forward to RU directly, return error.
    if (orig_fr_info->destID_type == DEV_TYPE_RU)
    {            
        // Check branch ID
        switch (orig_fr_info->destID_branch) {
            case 1:
                uart_index =  UART_IR_BR1;                
                break;

            case 2:
                uart_index =  UART_IR_BR2;                
                break;

            case 3:
                uart_index =  UART_IR_BR3;                        
                break;
                
            case 4:
                uart_index =  UART_IR_BR4;                
                break;

            default:
                return -2; // Invalid branch ID
        }
    }
    else if (orig_fr_info->destID_type == DEV_TYPE_SAU)
    {
        // Forward to SAU_M or SAU_S through UART_TR
        uart_index = UART_TR;
        dest_id = 1; 
    }
    else
    {
        return -1; // Invalid destination type
    }

    // Prepare frame_info for the RU
    memcpy(&fw_fr_info, orig_fr_info, sizeof(frame_info));

    fw_fr_info.srcID_type = orig_fr_info->destID_type;
    fw_fr_info.srcID_branch = orig_fr_info->destID_branch;
    fw_fr_info.srcID_port = orig_fr_info->destID_port;
    fw_fr_info.srcID_id = orig_fr_info->destID_id;

    fw_fr_info.destID_type = orig_fr_info->srcID_type;
    fw_fr_info.destID_branch = orig_fr_info->srcID_branch;
    fw_fr_info.destID_port = orig_fr_info->srcID_port;
    fw_fr_info.destID_id = orig_fr_info->srcID_id;


    // Create message frame
    tx_len = createForwardFrame(cmd, rx_buf, rx_len, tx_buf, &fw_fr_info);

    // Send to forward port
    uartWrites(uart_index, tx_buf, tx_len);    

    return tx_len;
} 


int __forwardFrame(int cmd, char *rx_buf, int len, frame_info *orig_fr_info)
{
    int tx_len = 0;
    short uart_index = 0;
 
    // Check dest type: only RU, SAU_M, SAU_S are valid. If dest type is RU, we cannot forward to RU directly, return error.
    if (orig_fr_info->destID_type == DEV_TYPE_RU)
    {            
        // Check branch ID
        switch (orig_fr_info->destID_port) {
            case 0:
                uart_index =  UART_IR_BR1;                
                break;

            case 1:
                uart_index =  UART_IR_BR2;                
                break;

            case 2:
                uart_index =  UART_IR_BR3;                        
                break;
                
            case 3:
                uart_index =  UART_IR_BR4;                
                break;

            default:
                return -2; // Invalid branch ID
        }
    }
    else if (orig_fr_info->destID_type == DEV_TYPE_SAU)
    {
        // Forward to SAU_M or SAU_S through UART_TR
        uart_index = UART_TR;

    }
    else if (orig_fr_info->destID_type == DEV_TYPE_LMT){
    	uart_index = UART_CLI;
    }
    else
    {
        return -1; // Invalid destination type
    }
#if DEBUG | 0
    xil_printf("[FWD -> UART %d] len=%d: ", uart_index, len);
    for (int i = 0; i < len; i++) {
        xil_printf("%02X ", (unsigned char)rx_buf[i]);
    }
    xil_printf("\r\n");
#endif
    // Send to forward port
    uartWrites(uart_index, rx_buf, len);

    return 1;
}


extern mu_dev_info mu_info;

void vTaskTopologyBuilding() 
{     
    while (1)
    {
        xil_printf("Topology building started ...\r\n");
        StartTopologyBuilding(&mu_info);   
        vTaskDelay(pdMS_TO_TICKS(20000));
    }
}

void TopologyBuildingTask()
{
    BaseType_t ret = xTaskCreate(vTaskTopologyBuilding,
                "TR-Server vTaskTopologyBuilding",
                APP_TASK_START_STK_SIZE,
                (void *)0,
                OS_TASK_TR_SERVER_PRIO,
                NULL);
    if (ret != pdPASS) {
        printf("[FATAL] Could not create topology building task!\r\n");
    }
}

char ir_step = 0;


void BR_RU_Insall(unsigned char br_num,unsigned char ru_number)
{
	struct repeater_struct  *prpt;
	prpt = getRptDev();
	
	if(ru_number == 0)
	{
		prpt->Tree.sau_pri_ru[br_num][0] = 1;
	}
	else if(ru_number == 1)
	{
		prpt->Tree.sau_pri_ru[br_num][0] = 1;
		prpt->Tree.sau_pri_ru[br_num][1] = 1;
	}
	else if(ru_number == 2)
	{
		prpt->Tree.sau_pri_ru[br_num][0] = 1;
		prpt->Tree.sau_pri_ru[br_num][1] = 1;
		prpt->Tree.sau_pri_ru[br_num][2] = 1;
	}
	else if(ru_number == 3)
	{
		prpt->Tree.sau_pri_ru[br_num][0] = 1;
		prpt->Tree.sau_pri_ru[br_num][1] = 1;
		prpt->Tree.sau_pri_ru[br_num][2] = 1;
		prpt->Tree.sau_pri_ru[br_num][3] = 1;
	}

}

extern u8	Download_flag;

void vIrClientTask(void *pvParameters) {
	int len, ch;

	unsigned char 		  source_id[4] = {0,};
	unsigned char 		  dest_id[4] = {0,};
	//  int					  len,status=0,retval = 0;
	int i;
	static int			  Br_Num = 0;
	unsigned char         UART_IR_Num;

	static int			  Br_Tx = 0;
	static int			  Br_Rx = 0;

	unsigned char insert = 0;

	void					  *pmsg;
	struct repeater_struct  *prpt;
	prpt = getRptDev();

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(1000);


	xLastWakeTime = xTaskGetTickCount();
//	static u8 ir_tx_buf[] = {'a','b','c','d','e','f','g','h','i','j','k','a','b','c','d','e','f','g','h','1'};
//	static u8 ir_tx_buf[] = {0x16,0x16,0x16,0x16,0x13,0x00,0xff,0xff,0x14,0x00,0xff,0xff,0x60,0x00,0x00,0x49,0x6F};
	static u8 ir_tx_buf[] = {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'};

	int tx_len = sizeof(ir_tx_buf);


	int				  timeout = 0;
	unsigned char ID_port;

	source_id[0] = DEV_TYPE_MU;
	source_id[1] = 0x00;
	source_id[2] = prpt->ID_port;
	source_id[3] = prpt->RU_id;
	
	dest_id[0] = DEV_TYPE_RU;
	dest_id[1] = 0x00;
	dest_id[2] = prpt->ID_port;
	dest_id[3] = prpt->RU_id;


	prpt->ID_port = 0x00;
	prpt->RU_id = 0x00;

	Br_Num = 0;

  	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
		if(Uart_Test)
		{
			uartWrites(UART_TR, ir_tx_buf, tx_len);
			uartWrites(UART_IR_BR1, ir_tx_buf, tx_len);
			uartWrites(UART_IR_BR2, ir_tx_buf, tx_len);
			uartWrites(UART_IR_BR3, ir_tx_buf, tx_len);
			uartWrites(UART_IR_BR4, ir_tx_buf, tx_len);
		}
		else
		{
			if( prpt->ir_port_cnt >=1)
			{
				prpt->ir_port_cnt -= 1;
			}
			else
			{
				prpt->lmt_bypass = 0;
			}


			vTaskDelay(pdMS_TO_TICKS(1000));

			if(prpt->lmt_bypass)
				continue;

			if(prpt->boot_initilization == 1)	
			{
//				Br_Num = 0;
				prpt->is_booted = 0;

				UART_IR_Num = UART_IR_BR1 + Br_Num;
				prpt->ID_port = Br_Num;
				if(UART_IR_Num >= UART_IR_BR4)UART_IR_Num = UART_IR_BR4;
				source_id[2] = prpt->ID_port;
				dest_id[2] = prpt->ID_port;
//				printf(" 0 ID_port[%d]>[%d]\n",prpt->ID_port,UART_IR_Num);
				getPortIr();
				len = __TopologyReq(tx_buf, source_id, dest_id, TOPOLOGY_REQ);
				uartWrites(UART_IR_Num, tx_buf, len);
				pmsg = pendIrMsgBox(IR_TIMEOUT);
				
				if(pmsg == 0)
				{  
					Br_Tx++;

					printf(" 0 pendIrMsgBox[%d]>[%d]:[%d]\n",pmsg,Br_Num,Br_Tx);
					if(Br_Tx >= 3) 
					{
						prpt->Tree.sau_pri_ru[Br_Num][0] = 0;
						prpt->Tree.sau_pri_ru[Br_Num][1] = 0;
						prpt->Tree.sau_pri_ru[Br_Num][2] = 0;
						prpt->Tree.sau_pri_ru[Br_Num][3] = 0;

						Br_Num++;
						Br_Tx = 0;
					}
				}
				else
				{
					Br_Tx = 0;
					Br_Rx++;
					if(Br_Rx > 5) ///10
					{
						BR_RU_Insall(Br_Num,prpt->children_ru_number[Br_Num]);

						Br_Num++;
						Br_Rx = 0;
					}
				}
				putPortIr();
				printf("0 boot_init[%d][%d][%d]\r\n",Br_Tx,Br_Rx,Br_Num);

				if(Br_Num >= 4)
				{
					prpt->is_booted = 1;
					prpt->boot_initilization = 0;
					ir_step = 2;
					printf("0 children_ru_number[%d][%d][%d][%d]\r\n",prpt->children_ru_number[0],prpt->children_ru_number[1],prpt->children_ru_number[2],prpt->children_ru_number[3]);

					printf(" 0 br_insert[%d][%d][%d][%d]\n",prpt->br_insert[0],prpt->br_insert[1],prpt->br_insert[2],prpt->br_insert[3]);
					Br_Num = 0;

					prpt->ru_br_nu[0] = prpt->children_ru_number[0];
					prpt->ru_br_nu[1] = prpt->children_ru_number[1];
					prpt->ru_br_nu[2] = prpt->children_ru_number[2];
					prpt->ru_br_nu[3] = prpt->children_ru_number[3];
					prpt->ru_nu = prpt->ru_br_nu[0] + prpt->ru_br_nu[1] + prpt->ru_br_nu[2] + prpt->ru_br_nu[3];


				}
			}
			else
			{
				if(ir_step == 0)	
				{
					for(i = Br_Num;i<4;i++){
						Br_Num	= i;
						if(prpt->br_insert[i])break;
					}	

					UART_IR_Num = UART_IR_BR1 + Br_Num;
					prpt->ID_port = Br_Num;
					if(UART_IR_Num >= UART_IR_BR4)UART_IR_Num = UART_IR_BR4;
					source_id[2] = prpt->ID_port;
					dest_id[2] = prpt->ID_port;

//					printf(" 0 ID_port[%d]>[%d]\n",prpt->ID_port,UART_IR_Num);
//					printf(" 1_ UART_IR_Num>[%d]\n",UART_IR_Num);

					getPortIr();
					len = __TopologyReq(tx_buf, source_id, dest_id, TOPOLOGY_REQ);
					uartWrites(UART_IR_Num, tx_buf, len);
					pmsg = pendIrMsgBox(IR_TIMEOUT);
					
					if(pmsg == 0)
					{  
						printf(" 1 pendIrMsgBox[%d]>[%d]\n",pmsg,Br_Num);
					}
					else
					{
					//	printf(" 1 pendIrMsgBox[%d]\n",pmsg);
					}
					putPortIr();
					Br_Num++;
					if(Br_Num >= 3)
					{
						ir_step = 1;
						Br_Num = 0;
					}
				}
				else if(ir_step == 1)	
				{

					for(i = Br_Num;i<4;i++){
						Br_Num	= i;
						if(prpt->br_insert[i])break;
					}	

					UART_IR_Num = UART_IR_BR1 + Br_Num;
					prpt->ID_port = Br_Num;
					if(UART_IR_Num >= UART_IR_BR4)UART_IR_Num = UART_IR_BR4;
					source_id[2] = prpt->ID_port;
					dest_id[2] = prpt->ID_port;

//					printf(" 2_ UART_IR_Num>[%d]\n",UART_IR_Num);

					getPortIr();
					len = TreeReq(tx_buf, source_id, dest_id, TREE_REQ);
					uartWrites(UART_IR_Num, tx_buf, len);
					pmsg = pendIrMsgBox(IR_TIMEOUT);
					
					if(pmsg == 0)
					{  
					  printf(" 2_ pendIrMsgBox[%d]>[%d]\n",pmsg,Br_Num);
					}
					else
					{
					//	printf(" 1 pendIrMsgBox[%d]\n",pmsg);
					}
					putPortIr();

					Br_Num++;
					if(Br_Num >= 4)
					{
						ir_step = 2;
						Br_Num = 0;
					}
				}
				else if(ir_step == 2)
				{
//					printf(" 0 br_insert[%d][%d][%d][%d]->[%d]\n",prpt->br_insert[0],prpt->br_insert[1],prpt->br_insert[2],prpt->br_insert[3],Br_Num);
					insert = 0;

					for(i = Br_Num;i<4;i++){
						Br_Num	= i;
						if(prpt->br_insert[i])
						{
							insert = 1;
							break;
						}
					}	
					if(insert)
					{
						UART_IR_Num = UART_IR_BR1 + Br_Num;
						prpt->ID_port = Br_Num;
						if(UART_IR_Num >= UART_IR_BR4)UART_IR_Num = UART_IR_BR4;
						source_id[2] = prpt->ID_port;
						dest_id[2] = prpt->ID_port;
//						printf(" 3_ UART_IR_Num>[%d][%d]\n",UART_IR_Num,Br_Num);

						getPortIr();
						
						prpt->ir_br_txcount[Br_Num]++;
						len = StatusReq(tx_buf, source_id, dest_id, MAIN_STAT_REQ);
						uartWrites(UART_IR_Num, tx_buf, len);

						pmsg = pendIrMsgBox(IR_TIMEOUT);
						
						if(pmsg == 0)
						{  
							printf(" 3 pendIrMsgBox[%d]>[%d]\n",pmsg,Br_Num);
						}
						else
						{
						//	printf(" 1 pendIrMsgBox[%d]\n",pmsg);
						}
						putPortIr();
					}
					Br_Num++;
					if(Br_Num >= 4)
					{
					//	ir_step = 1;
						Br_Num = 0;
					//	ir_step = 4;
					}
				}
				else if(ir_step == 3)
				{
					for(i = Br_Num;i<4;i++){
						Br_Num	= i;
						if(prpt->br_insert[i])break;
					}	

					UART_IR_Num = UART_IR_BR1 + Br_Num;
					prpt->ID_port = Br_Num;
					if(UART_IR_Num >= UART_IR_BR4)UART_IR_Num = UART_IR_BR4;
					source_id[2] = prpt->ID_port;
					dest_id[2] = prpt->ID_port;
//					printf(" 3_ UART_IR_Num>[%d]\n",UART_IR_Num);

					getPortIr();
					
					prpt->ir_br_txcount[Br_Num]++;
					len = StatusReq(tx_buf, source_id, dest_id, MAIN_STAT_REQ2);
					uartWrites(UART_IR_Num, tx_buf, len);

					pmsg = pendIrMsgBox(IR_TIMEOUT);
					
					if(pmsg == 0)
					{  
						printf(" 3 pendIrMsgBox[%d]>[%d]\n",pmsg,Br_Num);
						
					//	resetUartFifo(UART_TR);
					//	resetUartFifo(UART_IR_BR1);
					//	resetUartFifo(UART_IR_BR2);
					//	resetUartFifo(UART_IR_BR3);
					//	resetUartFifo(UART_IR_BR4);
					}
					else
					{
					//	printf(" 1 pendIrMsgBox[%d]\n",pmsg);
					}
					putPortIr();

					Br_Num++;
					if(Br_Num >= 4)
					{
					//	ir_step = 1;
						Br_Num = 0;
						ir_step = 2;
					}
				}
				else if(ir_step == 4)
				{
				//	ir_step = 5;
				}
				else
				{
					ir_step = 0;
				}
			}

			if(timeout > 10)
			{
				timeout = 0;
//				Download_flag = 0;
//				ir_step = 2;
//				printf(" 3 timeout[%d]\n",timeout);
			}
		}


	  ///// Status request
	  	if(Download_flag)
  		{
//			vTaskDelay(pdMS_TO_TICKS(5000));
//			ir_step = 5;
  		}
		else
		{
//			vTaskDelay(pdMS_TO_TICKS(1000));
		}
//		vTaskDelay(pdMS_TO_TICKS(1000));
		timeout++;
	}
}


void init_Ir_client(void) {
  xTaskCreate(vIrClientTask,
              "IR-C Task",
              OS_TASK_CONSOLE_STK_SIZE,
              NULL,
              OS_TASK_IR_CLIENT_PRIO,
              NULL
  );
}
