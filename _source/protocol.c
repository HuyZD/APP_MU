/* protocol.c
 */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <includes.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
//#include "monlib.h"
#include "repeater.h"
#include "ics.h"
#include "psa.h"
//#include "tfs.h"
#include "tbl.h"
#include "protocol_private.h"
#include "uart.h"
#include "protocol.h"
#include "device.h"


#define CTLC    0x03    /* control-c */
#define SYNC    0x16

/*	??? ???????? ??��? ??? ???. */
#pragma data_alignment=4     // 4byte ?????? alignment

#pragma location=".EXRAM"    // ??? ??? ???

static char tx_buf[2500]; 	 //2048
static char rx_buf[2500];    //2048


mu_dev_info mu_info;

unsigned char Uart_Test = 0;
extern int tr_rx_buf_Cnt;

#define Max_cnt  				5

/* getPacket() : receive packet
 *	return value
 *		1. cmd    : ????? ????? ????? ????? ??????? ??? ?????? ??????? ???????.
 *		2. tmppkt : ????? ????? ??? ??????? ????.
 */
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


#define IR_TIMEOUT    500  //1second  400
#define TR_TX_MAX_PACKET_SIZE			1100		// FSK TX 3500
#define TR_RX_MAX_PACKET_SIZE			1100		// FSK RX 1100

extern char tr_rx_buf[TR_TX_MAX_PACKET_SIZE];
extern char tr_tx_buf[TR_TX_MAX_PACKET_SIZE];

u8	Download_flag = 0;
void protocol(void)
{
	unsigned char 			cmd;
	int						len,status=0,retval = 0;
	unsigned char 			graph_path;
	/* Download Data*/
	short					totFrame,curFrame;
	struct downDesc			downDesc;
	struct downParam 		param;
	/* Table Type */
	struct tbl_desc			*pdesc;
	unsigned char			tblFreq, tblType;
	extern 	int 			Reboot();
	static	char			index=0;
	long					source_id;
	long					dest_id;
	char 					errMsg[128] = "";
	frame_info 				fr_info;
	short 					uart_index = UART_CLI;
	unsigned char			__source_id[4] = {0};
	unsigned char			__dest_id[4] = {0};
	
    struct repeater_struct  *prpt; 

	// Initialize MU info
	mu_info.mu.type = DEV_TYPE_MU;
	mu_info.mu.device_id = -1; // Will be set upon receiving the first Topologymessage
	mu_info.mu.branch_id = -1;
	mu_info.mu.port_id = -1;

    while (1) {
		status = __getPacket(uart_index, (char *)&cmd, (char *)rx_buf, errMsg, &fr_info);


		//logHeader(cmd, &fr_info);

		if (status == -2) {
			//CRC_REEOR - CRC err check.			
			len = errorStatus(tx_buf, errMsg);
			uartWrites(uart_index, tx_buf,len);
			continue;
		}

		if (status == -1) {
			return;
		}
		
		__source_id[0] = fr_info.srcID_type;
		__source_id[1] = fr_info.srcID_branch;
		__source_id[2] = fr_info.srcID_port;
		__source_id[3] = fr_info.srcID_id;
		
		// 4 bytes destination ID
		__dest_id[0] = fr_info.destID_type;
		__dest_id[1] = fr_info.destID_branch;
		__dest_id[2] = fr_info.destID_port;
		__dest_id[3] = fr_info.destID_id;

		if( __dest_id[0] == DEV_TYPE_RU){
//			if(cmd == MAIN_STAT_REQ){
//				len  =  MainStatReq_RU_Ack(tx_buf, __dest_id, __source_id,MAIN_STAT_ACK);
//				uartWrites(uart_index, tx_buf,len);
//			}
//			else {
				// len  = forwardFrame(cmd, rx_buf ,status, &fr_info);
			//}
				// continue;
		}

//		printf("protocol %x\n", cmd); /////////
		if(rx_buf[4] != 0x08){
			if (__dest_id[3] == 0xFF) {
	    	switch (cmd) {
	    	case TREE_REQ:
//	    		len = TreeAck(tx_buf, source_id, dest_id, TREE_ACK);
	    		len = __TreeAck(tx_buf, __dest_id, __source_id, TREE_ACK);
				uartWrites(uart_index, tx_buf,len);
				break;

			case DEVICE_ID_LOOKUP_REQ :
				len = DeviceIDLookupAck(tx_buf, __dest_id, __source_id, DEVICE_ID_LOOKUP_ACK);
				uartWrites(uart_index, tx_buf,len);
				break;

			case MAIN_STAT_REQ :
				// MainStatReq(rx_buf, &source_id, &dest_id, &Band_sel);
//				len = MainStatAck(tx_buf, &fr_info,MAIN_STAT_ACK);
				
				len = __MainStatAck(tx_buf, __dest_id, __source_id,MAIN_STAT_ACK);
				uartWrites(uart_index, tx_buf,len);
				break;

			case MAIN_CTRL_REQ :
				MainCtrlReq(rx_buf+4, __source_id,__dest_id);
//				len = MainStatAck(tx_buf, &fr_info,MAIN_CTRL_ACK);
				len = __MainStatAck(tx_buf, __dest_id, __source_id,MAIN_CTRL_ACK);
				uartWrites(uart_index, tx_buf,len);
	    		break;


			case ADC_REQ :
				len  = AdcStatAck(tx_buf, &fr_info, ADC_ACK);
				uartWrites(uart_index, tx_buf,len);
				break;
			
			case TABLE_STAT_REQ :
				len = TableStatAck(rx_buf+4, tx_buf, &fr_info,TABLE_STAT_ACK );
				uartWrites(uart_index, tx_buf,len);
				break;
			case TABLE_CTRL_REQ :
				len  = TableCtrlReq (rx_buf+4, tx_buf, &fr_info,TABLE_CTRL_ACK);
				uartWrites(uart_index, tx_buf,len);
				break;


	    	case DOWN_START_REQ :
				retval = DownKickReq(rx_buf+4, &downDesc, &param);
				len = DownKickAck(tx_buf,__dest_id, __source_id, DOWN_START_ACK, &downDesc, &param, retval);
				uartWrites(uart_index, tx_buf,len);
				break;

			case DOWN_DATA_REQ :
				retval = DownDataReq(rx_buf+4, &downDesc, &param);
				len = DownDataAck(tx_buf, __dest_id, __source_id,DOWN_DATA_ACK, &downDesc, &param, retval);
	    		uartWrites(uart_index, tx_buf,len);
	    		break;

			case DOWN_END_REQ :
				retval = DownTermReq(rx_buf+4, &downDesc, &param);
				len = DownTermAck(tx_buf, __dest_id, __source_id,totFrame, curFrame, DOWN_END_ACK, &param, retval);
	    		uartWrites(uart_index, tx_buf,len);

	    		//OSTimeDly(1000);
//				Reboot();
	    		break;

	    	case DN_FPGA_START_REQ :
				retval = DownKickReq(rx_buf+4, &downDesc, &param);
				len = DownKickAck(tx_buf,__dest_id, __source_id, DN_FPGA_START_ACK, &downDesc, &param, retval);
				uartWrites(uart_index, tx_buf,len);
				break;

			case DN_FPGA_DATA_REQ :
				retval = DownDataReq(rx_buf+4, &downDesc, &param);
				len = DownDataAck(tx_buf,__dest_id, __source_id,DN_FPGA_DATA_ACK, &downDesc, &param, retval);
	    		uartWrites(uart_index, tx_buf,len);
	    		break;

			case DN_FPGA_END_REQ :
				retval = DownTermReq(rx_buf+4, &downDesc, &param);
				len = DownTermAck(tx_buf,__dest_id, __source_id,totFrame, curFrame, DN_FPGA_END_ACK, &param, retval);
	    		uartWrites(uart_index, tx_buf,len);

	    		//OSTimeDly(1000);
//				Reboot();
	    		break;

	    	case SYS_RST_REQ :
				//mon_printf("SYS_RST_REQ\n");
				SysResetAck(tx_buf, SYS_RST_ACK,0);
	    		break;

			default :
	    		break;
	    	}
		}
		else {
				if( __dest_id[0] == DEV_TYPE_RU){
						void					  *pmsg;

						if((cmd == DOWN_START_REQ)||(cmd == DN_FPGA_DATA_REQ)||(cmd == DN_FPGA_DATA_REQ))
						{
							Download_flag = _SET;
							prpt->lmt_bypass = 1;
							prpt->ir_port_cnt = Max_cnt*3;

							getPortIr();
							len  = forwardFrame(cmd, rx_buf ,status, &fr_info);
							pmsg = pendIrMsgBox(IR_TIMEOUT);

							if(pmsg == 0)
							{
							//	printf(" 0 pendIrMsgBox[%d]\n",pmsg);
							}
							else
							{
								uint8_t *pData = (uint8_t *)pmsg;
	//						printf(" 1 pendIrMsgBox[%x]:[%x][%x][%x][%x][%x]\n",tr_rx_buf_Cnt,pData[0],pData[1],pData[2],pData[3],pData[4]);
								uartWrites(uart_index, pData,tr_rx_buf_Cnt);
							}

							putPortIr();
							continue;
						}else if (cmd  == MAIN_STAT_REQ){
							len  =  MainStatReq_RU_Ack(tx_buf, __dest_id, __source_id,MAIN_STAT_ACK);
							uartWrites(uart_index, tx_buf,len);
						}

					}
					
					switch (cmd) {		
						case MAIN_STAT_REQ :
							{
							//void					  *pmsg;

//							uartWrites(uart_index, tx_buf,len);

//							getPortIr();
//							uartWrites(UART_IR_BR4, rx_buf, len);
							//pmsg = pendIrMsgBox(IR_TIMEOUT);
//							putPortIr();

							}

							break;
					
						default :	// ���� ��ȸ�� ������ �������� RU�� �����Ѵ�.
#if 0
							getPortIr();
					
							len = forwardPktToAndFromIr(rx_buf, tx_buf, status);
							if (len < 0)  {//timeout 
								putPortIr();
								continue;
							}							
							sendToGuiBypass(tx_buf,len);
					
							/* MU�����ϰ� �ִ� RU ����Ÿ ������ �ϴ� ������ �ʿ���	*/
							putPortIr();	
#endif											
							break;
					}
		}
		}
		else
		{
			len = CheckAck(tx_buf, DEVICE_ID_LOOKUP_ACK);
			uartWrites(uart_index, tx_buf,len);

		}
    	continue;

	}
}
