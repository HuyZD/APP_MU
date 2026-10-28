/* protocol.c
 */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "repeater.h"
#include "repeater_id.h"
#include "ics.h"
#include "psa.h"
#include "tbl.h"
#include "protocol_private.h"
#include "history.h"
#include "flash.h"
#include "uart.h"
#include "protocol.h"
#include "protocol_ad4t.h"
#include "device.h"

int errorStatus(char *buf, char *errMsg);


unsigned char		download_pool[1024*1024*7] = {0,};
unsigned char 		AmpoffFlag[MAX_BAND] = {0,};
static bool is_prpt_init = false;
static bool is_table_init = false;

extern dev_type_name dev_map_array[MAX_UART_TYPE];

static char tx_buf[2500]; 	 //2048
static char rx_buf[2500];    //2048
extern mu_dev_info mu_info;


////////////////////////////////////////////////////////////////////////
// 	Test
////////////////////////////////////////////////////////////////////////
#define PHASE0					0
#define MAX_DAS_DATA_LEN   1024
#define DEVICE_ID_LOOKUP_REQ_LENGTH 12
#define FW_RAM_MAX_SIZE  (312 * 1024)

int CheckAck(char *buf, unsigned cmd)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned short 	crc,swap16;

	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* Reserved */
	buf[len++] = 0x08;

	/* CMD */
	buf[len++] = cmd;

	/* lengh */
	len+=2;

	/* Repeater Id*/
	swap16 = swap2(AD3T_MU_Type);
	memcpy((void *)&buf[len], (void *)&swap16, 2); len+=2;

	for(int i=0; i<10; i++){
		buf[len++] = 0;
	}

	/* length */
	dlen = len - 8;
	dlen = swap2(dlen);
	memcpy((void *)&buf[6], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}


int getsync(short uart_index)
{
    char   	sync[4];
	int		i;

	for(i=0;i<4;i++)
		sync[i] = 0;

	while (1) {
		sync[3] = sync[2];
		sync[2] = sync[1];
		sync[1] = sync[0];
		sync[0] = (char)uartRead(uart_index);

		if (sync[0] == CTLC && sync[1] == CTLC && sync[2] == CTLC && sync[3] == CTLC)
			return -1;
		if (sync[0] == SYNC && sync[1] == SYNC && sync[2] == SYNC && sync[3] == SYNC) {
			return  1;
		}
	}
}

/* getPacket() : receive packet
 *	return value
 *		1. cmd    : ????? ????? ????? ????? ??????? ??? ?????? ??????? ???????.
 *		2. tmppkt : ????? ????? ??? ??????? ????.
 */
int checkDas(short uart_index, char *cmd, bool *isChecked)
{
    unsigned char reserved;
    unsigned char type;
    unsigned short length;
    char data[MAX_DAS_DATA_LEN];
    unsigned short pkt_crc, calc_crc;
    char full_pkt[MAX_DAS_DATA_LEN + 4]; // Ch?a Header + Data ��? tinh CRC
    int total_len = 0;

    /* 1. B?t 4 byte SYNC 0x16 */
    if (getsync(uart_index) < 0) {
        return -1;
    }

    /* 2. ��?c byte Reserved (ph?i b?ng 0x08 theo protocol) */
    reserved = (unsigned char)uartRead(uart_index);
    full_pkt[total_len++] = reserved;
    if (reserved != CHECK_DAS) {
        return -2;
    }

    /* 3. ��?c Type (Command) */
    type = (unsigned char)uartRead(uart_index);
    full_pkt[total_len++] = type;
    *cmd = type;

    /* 4. ��?c 2 byte Length (��?c tu?n t? tranh l?i ��?o byte) */
    unsigned char len_h = (unsigned char)uartRead(uart_index);
    unsigned char len_l = (unsigned char)uartRead(uart_index);
    full_pkt[total_len++] = len_h;
    full_pkt[total_len++] = len_l;
    length = (len_h << 8) | len_l;

    if (length > MAX_DAS_DATA_LEN) {
        return -3; // D? li?u qua dai
    }

    /* 5. ��?c Data */
    for (int i = 0; i < length; i++) {
        data[i] = uartRead(uart_index);
        full_pkt[total_len++] = data[i];
    }

    /* 6. ��?c 2 byte CRC ? cu?i */
    unsigned char crc_h = (unsigned char)uartRead(uart_index);
    unsigned char crc_l = (unsigned char)uartRead(uart_index);
    pkt_crc = (crc_h << 8) | crc_l;

    /* 7. Tinh va ki?m tra CRC */
    calc_crc = Generate_CRC(full_pkt, total_len);
    if (calc_crc != pkt_crc) {
        return -4;
    }
    *isChecked = true;
    return length;
}
int getPacket(short uart_index, char *cmd, char *tmppkt, char *err, frame_info *fr_info)
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

bool isFrameForMe(unsigned int cmd, frame_info *fr_info)
{
	if (cmd == TOPOLOGY_REQ || cmd == TOPOLOGY_RESP) {
		// Topology requests/responses are broadcast messages, so they are for everyone
		return true;
	}

	if (fr_info->destID_type == DEV_TYPE_MU) {
		return true;
	}

	return false;
}


void logPackage(char *msg, int len)
{
	for (int index = 0; index < len; index ++)
	{
		xil_printf(" 0x%x", msg[index]);
	}
	xil_printf(" \r\n");
}

int MsgHeaderCommon(char *buf, const frame_info *fr_info, int cmd)
{
	unsigned short 	len = 0;
	unsigned char 	sync = 0x16;

	// 4bytes SYNC
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

    // 3 bytes destination ID
    buf[len++] = fr_info->destID_type;
    buf[len++] = fr_info->destID_branch;
	buf[len++] = fr_info->destID_port;
    buf[len++] = fr_info->destID_id;
	// 4 bytes source ID
    buf[len++] = fr_info->srcID_type;
    buf[len++] = fr_info->srcID_branch;
	buf[len++] = fr_info->srcID_port;
    buf[len++] = fr_info->srcID_id;

//    xil_printf(" Source id : 0x%x  0x%x  0x%x  0x%x \n Dest id : 0x%x  0x%x  0x%x  0x%x /r/n",
//    		fr_info->destID_type,
//			fr_info->destID_branch,
//			fr_info->destID_port,
//			fr_info->destID_id,
//			fr_info->srcID_type,
//			fr_info->srcID_branch,
//			fr_info->srcID_port,
//			fr_info->srcID_id);

    // 1 byte command
    buf[len++] = cmd;

	/* Place holder for lengh */
	len+=2;

	return len; // 13 bytes header length
}

void setFrameInfo(frame_info *fr_info, frame_info *resp_fr_info)
{
	resp_fr_info->srcID_type = fr_info->destID_type;
	resp_fr_info->srcID_branch = fr_info->destID_branch;
	resp_fr_info->srcID_port = fr_info->destID_port;
	resp_fr_info->srcID_id = fr_info->destID_id;

	resp_fr_info->destID_type = fr_info->srcID_type;
	resp_fr_info->destID_branch = fr_info->srcID_branch;
	resp_fr_info->destID_port = fr_info->srcID_port;
	resp_fr_info->destID_id = fr_info->srcID_id;
}


void logHeader(short uart_index, char cmd, frame_info *fr_info)
{
	xil_printf("----[%d]: cmd 0x%x(%d) ---\r\n ", uart_index, cmd, cmd);
	xil_printf("src: 0x%x %d %d %d\r\n ", fr_info->srcID_type, fr_info->srcID_branch, fr_info->srcID_port, fr_info->srcID_id);
	xil_printf("dst: 0x%x %d %d %d\r\n ", fr_info->destID_type, fr_info->destID_branch, fr_info->destID_port, fr_info->destID_id);
}

int TopologyAck(char *buf, mu_dev_info *mu_info, frame_info *orig_fr_info)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned short 	crc,swap16;
	int 			index;
	int 			ru_number;
	frame_info      fr_info;

	// 4 bytes source ID
    fr_info.srcID_type = DEV_TYPE_MU;
    fr_info.srcID_branch = orig_fr_info->destID_branch;
	fr_info.srcID_port = orig_fr_info->destID_port;
    fr_info.srcID_id = orig_fr_info->destID_id;

    // 4 bytes destination ID
    fr_info.destID_type = orig_fr_info->srcID_type;   
    fr_info.destID_branch = orig_fr_info->srcID_branch;
	fr_info.destID_port = orig_fr_info->srcID_port;
    fr_info.destID_id = orig_fr_info->srcID_id;

	// Note that header info are not changed.
	len = MsgHeaderCommon(buf, &fr_info, TOPOLOGY_RESP);

	// Calculation of RU number based on branches
	ru_number = mu_info->br1_ru_number + mu_info->br2_ru_number + mu_info->br3_ru_number + mu_info->br4_ru_number;

	// Number of RU
	buf[len++] = ru_number & 0xFF;

	/* All RUs from BR1 */
	for (index = 0; index < mu_info->br1_ru_number; index++) {		
		buf[len++] = mu_info->ru_br1[index].branch_id & 0xFF; 
		buf[len++] = mu_info->ru_br1[index].port_id & 0xFF;
		buf[len++] = mu_info->ru_br1[index].device_id & 0xFF;
	}

	/* All RUs from BR2 */
	for (index = 0; index < mu_info->br2_ru_number; index++) {		
		buf[len++] = mu_info->ru_br2[index].branch_id & 0xFF;
		buf[len++] = mu_info->ru_br2[index].port_id & 0xFF;
		buf[len++] = mu_info->ru_br2[index].device_id & 0xFF;
	}

	/* All RUs from BR3 */
	for (index = 0; index < mu_info->br3_ru_number; index++) {		
		buf[len++] = mu_info->ru_br3[index].branch_id & 0xFF; 
		buf[len++] = mu_info->ru_br3[index].port_id & 0xFF;
		buf[len++] = mu_info->ru_br3[index].device_id & 0xFF;
	}

	/* All RUs from BR4 */
	for (index = 0; index < mu_info->br4_ru_number; index++) {		
		buf[len++] = mu_info->ru_br4[index].branch_id & 0xFF;
		buf[len++] = mu_info->ru_br4[index].port_id & 0xFF;
		buf[len++] = mu_info->ru_br4[index].device_id & 0xFF;
	}

	dlen = len - 15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}

void updateRuInfo(char* rx_buf, unsigned int rx_len, frame_info *fr_info, mu_dev_info *mu_info, short uart_index)
{
	unsigned short ru_number;
	unsigned short port_id;
	unsigned short ru_index = 0;
	dev_info *ru_info_ptr;
	int index = 11; // Skip 9 bytes of header (Source ID + Dest ID + Command + Length)
	
	// Which branch has answered?
	mu_info->top_status[uart_index - 2] = TOPOLOGY_COMPLETED;

	// Number of RU
	ru_number = rx_buf[index++];

	// Which branch is this?
	port_id = fr_info->srcID_port;
	switch (port_id) {
		case 0:
			mu_info->br1_ru_number = ru_number;
			ru_info_ptr = &mu_info->ru_br1;
			break;

		case 1:
			mu_info->br2_ru_number = ru_number;
			ru_info_ptr = &mu_info->ru_br2;
			break;

		case 2:
			mu_info->br3_ru_number = ru_number;
			ru_info_ptr = &mu_info->ru_br3;
			break;

		case 3:
			mu_info->br4_ru_number = ru_number;
			ru_info_ptr = &mu_info->ru_br4;
			break;

		default:
			// Invalid branch ID
			return;
	}

	// Fill the RU info for this branch
	for (ru_index = 0; ru_index < ru_number; ru_index++) {		
		ru_info_ptr[ru_index].device_id = rx_buf[index++];
		ru_info_ptr[ru_index].branch_id = rx_buf[index++];
		ru_info_ptr[ru_index].port_id = rx_buf[index++];
		ru_info_ptr[ru_index].type = DEV_TYPE_RU;
	}
}


int TreeReq(char *buf, unsigned char source_id[4], unsigned char dest_id[4], unsigned cmd)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned short 	crc,swap16;
	int 							i;

	struct repeater_struct *prpt;
	prpt = getRptDev();
#if 1
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* 	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;
#endif

	/* Command type */
	buf[len++] = cmd;

	/* lengh */
	len+=2;


//	printf("source_id[%02x][%02x][%02x][%02x]\r\n",source_id[0],source_id[1],source_id[2],source_id[3]);
//	printf("dest_id[%02x][%02x][%02x][%02x]\r\n",dest_id[0],dest_id[1],dest_id[2],dest_id[3]);

//	printf("[FATAL] Could not create topology building task!\r\n");


	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}


int TreeAck(char *buf, int source_id, int dest_id, unsigned cmd)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned short 	crc,swap16;

	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* 	Source ID */
//	buf[len] = source_id;
	memcpy((void *)&buf[len], (void *)&source_id, 3);
	len += 3;

	/* Dest ID */
//	buf[len] = dest_id;
	memcpy((void *)&buf[len], (void *)&dest_id, 3);
	len += 3;

	/* Command type */
	buf[len++] = cmd;

	/* lengh */
	len+=2;

	/* ROOT RU */

	/* ROOT ID */
	buf[len++] = 13;

	/* Branch ID */
	buf[len++] =0xFF;

	/* A larm Severity */
	buf[len++] = 0x01;

	dlen = len-8-5;
	dlen = swap2(dlen);
	memcpy((void *)&buf[11], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}

int __TreeAck(char *buf, unsigned char source_id[4], unsigned char dest_id[4], unsigned cmd)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned short 	crc,swap16;
	unsigned char 	i,j;

	struct repeater_struct *prpt;
	prpt = getRptDev();
	
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	prpt->tree_status[0] = 1;
	prpt->tree_status[3] = 1;
	prpt->tree_status[4] = 1;
	prpt->tree_status[5] = 1;

	/* 	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* Command type */
	buf[len++] = cmd;

	/* lengh */
	len+=2;
#if 0
	memcpy(&buf[len],prpt->tree_status , 93);
	len+=93;
#endif

#if 1
	/* DAU 0,1,2 */
	memcpy((void *)&buf[len], (void *)prpt->Tree.dau, 3);
	len += 3;

	/* SAU Tree */
	buf[len++] = prpt->Tree.sau_pri;

	/* MU Tree */
	buf[len++] = prpt->Tree.sau_pri_mu;

	/* RU Tree */
	for (i = 0; i < BR_NUM; i++) 
	{
		for (j = 0; j < RU_BR_NUM; j++) {
			buf[len++] = prpt->Tree.sau_pri_ru[i][j];
		}
	}

	/* SAU sec0 Tree */
	buf[len++] = prpt->Tree.sau_s1;

	/* MU sec0 Tree */
	buf[len++] = prpt->Tree.sau_s1;

	/* RU sec0 Tree */
	for (i = 0; i < BR_NUM; i++) 
	{
		for (j = 0; j < RU_BR_NUM; j++) {
			buf[len++] = prpt->Tree.sau_s1_ru[i][j];
		}
	}

	/* SAU sec1 Tree */
	buf[len++] = prpt->Tree.sau_s2;

	/* MU sec1 Tree */
	buf[len++] = prpt->Tree.sau_s2_mu;

	/* RU sec1 Tree */
	for (i = 0; i < BR_NUM; i++) 
	{
		for (j = 0; j < RU_BR_NUM; j++) {
			buf[len++] = prpt->Tree.sau_s2_ru[i][j];
		}
	}


	/* SAU sec2 Tree */
	buf[len++] = prpt->Tree.sau_s3;

	/* MU sec2 Tree */
	buf[len++] = prpt->Tree.sau_s3_mu;

	/* RU sec2 Tree */
	for (i = 0; i < BR_NUM; i++) 
	{
		for (j = 0; j < RU_BR_NUM; j++) {
			buf[len++] = prpt->Tree.sau_s3_ru[i][j];
		}
	}

	/* SAU sec3 Tree */
	buf[len++] = prpt->Tree.sau_s4;

	/* MU sec3 Tree */
	buf[len++] = prpt->Tree.sau_s4_mu;

	/* RU sec3 Tree */
	for (i = 0; i < BR_NUM; i++) 
	{
		for (j = 0; j < RU_BR_NUM; j++) {
			buf[len++] = prpt->Tree.sau_s4_ru[i][j];
		}
	}

#endif

	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}


int __TopologyReq(char *buf, unsigned char source_id[4], unsigned char dest_id[4], unsigned cmd)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned char 	Pre = 0xFF;

	unsigned short 	crc,swap16;

	struct repeater_struct *prpt;
	prpt = getRptDev();

	memcpy((void *)&buf[len], (void *)&Pre, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&Pre, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&Pre, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&Pre, 1); len+=1;

	
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* 	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* Command type */
	buf[len++] = cmd;

	/* lengh */
	len+=2;
#if 0
	buf[len++] = source_id[0];
	buf[len++] = source_id[1];
	buf[len++] = source_id[2];
	buf[len++] = source_id[3];

	buf[len++] = dest_id[0];
	buf[len++] = dest_id[1];
	buf[len++] = dest_id[2];
	buf[len++] = dest_id[3];
#endif	
#if 1
	buf[len++] = 0x01;

	buf[len++] = dest_id[0];
	buf[len++] = dest_id[1];
	buf[len++] = dest_id[2];
	buf[len++] = dest_id[3];
#endif

#if 0
	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC
#endif
	dlen = len-15 -4;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13 +4], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4+4],len-4-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}


int StatusReq(char *buf, unsigned char source_id[4], unsigned char dest_id[4], unsigned cmd)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned short 	crc,swap16;

	struct repeater_struct *prpt;
	prpt = getRptDev();
	
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* 	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* Command type */
	buf[len++] = cmd;

	/* lengh */
	len+=2;

	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}

int DeviceIDLookupAck(char *buf, unsigned char source_id[4], unsigned char dest_id[4], unsigned cmd)
{
	unsigned short 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned short 	crc,swap16;

	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* 	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* CMD */
	buf[len++] = cmd;

	/* lengh */
	len+=2;

	/* Repeater Id*/
	swap16 = swap2(AD3T_MU_Type);
	memcpy((void *)&buf[len], (void *)&swap16, 2); len+=2;

	/* length */
	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return (len);
}

/*
 * Protocol
 */

int MainStatReq(char *buf, frame_info *fr_info)
{
	unsigned int 	len = 0;
	unsigned short 					dlen,crc;

	len = MsgHeaderCommon(buf, fr_info, MAIN_STAT_REQ);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

	return len;
}

void TestRrpt(struct repeater_struct *prpt)
{
//	strcpy(prpt->repeater_model_name, "TJ-OP");
//	strcpy(prpt->repeater_type, "AD3T");
//	strcpy(prpt->repeater_maker, "TJ-Maker");
//	prpt->repeater_ID = 0x0203;
//	prpt->MU_ID = 0xFF;
	prpt->Topology_Set = 0;
	prpt->AGS_onoff = 0;
	prpt->ISO_onoff = 0;
//	strcpy(prpt->repeater_sn, "090909090");
//	prpt->firmware_version = 0x01;
//	prpt->fpga_version = 0x01;
//	prpt->temperature = 45;
	prpt->dc_volt = 57;
//	prpt->ru_nu = 5;
//	prpt->ru_alarm_nu = 3;
//	prpt->temperatureAlarm = 1;
//	prpt->temp_high = 45;
//	prpt->temp_offset = 2;
//	prpt->alarmDuration = 3;
//	prpt->tr_pwm = 100;
//	prpt->tr_sel = 1;
//	prpt->tr_txcount = 10;
//	prpt->tr_rxcount = 12;
//	prpt->led_test = 12;
	for(int i =0; i<BR_NUM; i++)
	{
		prpt->ru_con_status[i] = 0;
		prpt->br_onoff[i] = 1;
		prpt->ru_br_nu[i] = 2;
		prpt->br_pwr_alarm[i] = i+1;
		for(int j =0; j<RU_BR_NUM; j++)
		{
			prpt->ru_link_fail[i][j] = 1;
		}
//		prpt->band_in_use[i] = 1;
//		prpt->ir_pwm_br[i] = 5;
//		prpt->ir_pwm_sel[i] = 1;
//		prpt->ir_br_txcount[i] = 2;
//		prpt->ir_br_rxcount[i] = 3;
		// prpt->reserved[i] =0;
	}
	for(int i =0 ;i<16;i++){
		prpt->RU_Iso[i]  = 1;
	}
		prpt->psu_status = 0;
	for (int i =0; i<20; i++){
		prpt->led_status[i] = 1;
	}
	for(int i=0;i<20; i++){
		prpt->reserved[i] = 0;
	}


}
void TestAdc(struct repeater_adc_struct *prpt){
	prpt->DL_DET_High_Band = 0;
	prpt->DL_DET_Low_Band = 0;
	prpt->UL_DET_High_Band = 0;
	prpt->UL_DET_Low_Band = 0;
	prpt->DC_DET = 10;
}
void TestTable(struct repeater_table_struct *prpt){
	prpt->Table_Size  = 5;
	for (int i =0;i<prpt->Table_Size;i++){
		prpt->Actual_Table_Value[i*2] = i;
		prpt->Actual_Table_Value[i*2+1] = 5;
	}
}

int MainStatAck(char *buf, frame_info *fr_info, unsigned char cmd) {
	unsigned short 					len = 0;
	unsigned short 					dlen;
	unsigned char 					sync = 0x16;
	unsigned short 					crc,swap16;
	int 							i;
	unsigned int 					swap32;
	struct repeater_struct			*prpt;
	struct ics_struct 				*idev;
	struct tbl_desc 				*pdesc;

//	pdesc = getTblDesc(2);
	prpt = getRptDev();
//	idev = getIcsDev(band);

	// Fill test data
	TestRrpt(prpt);


	/* Message header */
	len = MsgHeaderCommon(buf, fr_info, cmd);

	/*	Body */
	/*1.  Operator, 10byte */
	for (i = 0; i < 10; i++)
		buf[len++] = prpt->repeater_model_name[i];

	/*2. Repeater type , 10byte */
	for (i = 0; i < 10; i++)
		buf[len++] = prpt->repeater_type[i];

	/*3. Repeater Maker , 10byte */
	for (i = 0; i < 10; i++)
		buf[len++] = prpt->repeater_maker[i];

	/*4. Repeater ID, 2byte */
	swap16 = swap2(prpt->repeater_ID);
	memcpy(&buf[len], &swap16, 2);
	len += 2;


	/*5. MU ID, 1byte */
	buf[len++] = prpt->MU_ID;

	/* 6. Boot Initialization, 1byte */
	buf[len++] = prpt->boot_initilization;

	/*7. Is Booted, 1byte */
	buf[len++] = prpt->is_booted;

	/*8. Topology Set, 1byte */
	buf[len++] = prpt->Topology_Set;

	/*9. AGS On/Off, 1byte */
	buf[len++] = prpt->AGS_onoff;

	/*10. ISO On/Off, 1byte */
	buf[len++] = prpt->ISO_onoff;

	/*11. Serial Number(S/N), 20byte*/
	for (i = 0; i < 20; i++)
		buf[len++] = prpt->repeater_sn[i];


	/*12. Firmware version, 2byte */
	swap16 = swap2(prpt->firmware_version);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*13. FPGA version, 2byte */
	swap16 = swap2(prpt->fpga_version);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*14.Temperature, 2byte*/
	swap16 = swap2(prpt->temperature);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*15. RU_Connection_State, 4byte*/
	for (int i = 0; i < 4; i++) {
		buf[len++] = prpt->ru_con_status[i];
	}

	/*16.DC_Volt, 2byte */
	swap16 = swap2(prpt->dc_volt);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*17.BRx_OnOff, 4byte */
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->br_onoff[i];
	}
	/*21. Number_of_RUs, 1byte*/
	buf[len++] = prpt->ru_nu;

	/*22. Number_of_BRx, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->ru_br_nu[i];
	}
	/*26. Number_of_RUs_Alarm, 1byte */
	buf[len++] = prpt->ru_alarm_nu;

	/*27. High Temperature Alarm, 1byte*/
	buf[len++] = prpt->temperatureAlarm;

	/*28. BRx_PWR_Alarm, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->br_pwr_alarm[i];
	}

	/*32. RUx_x_LINK_Fail, 16byte*/
	for (int i = 0; i < BR_NUM; i++) {
		for(int j =0;j<RU_BR_NUM; j++)
		buf[len++] = prpt->ru_link_fail[i][j];
	}
	/* 48 . Ru Isolation */

	for(int i=0;i<16;i++){
		swap16 = swap2(prpt->RU_Iso[i]);
			memcpy(&buf[len], &swap16, 2);
			len += 2;
	}

	/*49. BAND Use, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->band_in_use[i];
	}
	/*53. Temp High, 2byte */
	swap16 = swap2(prpt->temp_high);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*54. Temp Offset, 2byte*/
	swap16 = swap2(prpt->temp_offset);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*55. Alarm Duration, 2byte*/
	swap16 = swap2(prpt->alarmDuration);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*56. TR_PWM, 2byte*/
	swap16 = swap2(prpt->tr_pwm);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*57. TR_SEL, 1byte*/
	buf[len++] = prpt->tr_sel;

	/*58. IR_PWM_BRx, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		swap16 = swap2(prpt->ir_pwm_br[i]);
		memcpy(&buf[len], &swap16, 2);
		len += 2;
	}

	/*62.IR_SEL_BRx, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->ir_pwm_sel[i];
	}

	/*66. TR_TxCount, 2byte*/
	swap16 = swap2(prpt->tr_txcount);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*67.TR_RxCount, 2byte*/
	swap16 = swap2(prpt->tr_rxcount);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*68. IR_BRx_TxRxCount, 16byte*/
	for (int i = 0; i < BR_NUM; i++) {
		swap16 = swap2(prpt->ir_br_txcount[i]);
		memcpy(&buf[len], &swap16, 2);
		len += 2;
		swap16 = swap2(prpt->ir_br_rxcount[i]);
		memcpy(&buf[len], &swap16, 2);
		len += 2;
	}

	/* 76. PSU_STATUS (1 byte) */
	buf[len++] = prpt->psu_status;

	/*77. LED Status, 20byte*/
	for (int i = 0; i < 20; i++) {
		buf[len++] = prpt->led_status[i];
	}

	/*78. LED_Test, 1byte*/
	buf[len++] = prpt->led_test;

	/*79. Reserved, 20byte*/
	for (int i = 0; i < 20; i++) {
		buf[len++] = prpt->reserved[i];
	}

	/* length */
	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

//	UART_Writes(uart_index, band,2);
	return len;
}



int __MainStatAck(char *buf, int source_id, int dest_id, unsigned char cmd) {
	unsigned short 					len = 0;
	unsigned short 					dlen;
	unsigned char 					sync = 0x16;
	unsigned short 					crc,swap16;
	int 							i;
	unsigned int 					swap32;
	struct repeater_struct			*prpt;

	prpt = getRptDev();

	// Fill test data
	TestRrpt(prpt);


	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	prpt->tree_status[0] = 1;
	prpt->tree_status[3] = 1;
	prpt->tree_status[4] = 1;
	prpt->tree_status[5] = 1;

	/* 	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* Command type */
	buf[len++] = cmd;

	/* lengh */
	len+=2;
    
	/*	Body */

	/*1.  Operator, 10byte */
	for (i = 0; i < 10; i++)
		buf[len++] = prpt->repeater_model_name[i];

	/*2. Repeater type , 10byte */
	for (i = 0; i < 10; i++)
		buf[len++] = prpt->repeater_type[i];

	/*3. Repeater Maker , 10byte */
	for (i = 0; i < 10; i++)
		buf[len++] = prpt->repeater_maker[i];

	/*4. Repeater ID, 2byte */
	swap16 = swap2(prpt->repeater_ID);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*5. MU ID, 1byte */
	buf[len++] = prpt->MU_ID;

	/* 6. Boot Initialization, 1byte */
	buf[len++] = prpt->boot_initilization;

	/*7. Is Booted, 1byte */
	buf[len++] = prpt->is_booted;

	/*8. Topology Set, 1byte */
	buf[len++] = prpt->Topology_Set;

	/*9. AGS On/Off, 1byte */
	buf[len++] = prpt->AGS_onoff;

	/*10. ISO On/Off, 1byte */
	buf[len++] = prpt->ISO_onoff;

	/*11. Serial Number(S/N), 20byte*/
	for (i = 0; i < 20; i++)
		buf[len++] = prpt->repeater_sn[i];

	/*12. Firmware version, 2byte */
	swap16 = swap2(prpt->firmware_version);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*13. FPGA version, 2byte */
	swap16 = swap2(prpt->fpga_version);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*14.Temperature, 2byte*/
	swap16 = swap2(prpt->temperature);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*15. RU_Connection_State, 4byte*/
	for (int i = 0; i < 4; i++) {
		buf[len++] = prpt->ru_con_status[i];
	}

	/*16.DC_Volt, 2byte */
	swap16 = swap2(prpt->dc_volt);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*17.BRx_OnOff, 4byte */
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->br_onoff[i];
	}
	/*21. Number_of_RUs, 1byte*/
	buf[len++] = prpt->ru_nu;

	/*22. Number_of_BRx, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->ru_br_nu[i];
	}
	/*26. Number_of_RUs_Alarm, 1byte */
	buf[len++] = prpt->ru_alarm_nu;

	/*27. High Temperature Alarm, 1byte*/
	buf[len++] = prpt->temperatureAlarm;

	/*28. BRx_PWR_Alarm, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->br_pwr_alarm[i];
	}

	/*32. RUx_x_LINK_Fail, 16byte*/
	for (int i = 0; i < BR_NUM; i++) {
		for(int j =0;j<RU_BR_NUM; j++)
		buf[len++] = prpt->ru_link_fail[i][j];
	}
	/* 48 . Ru Isolation */

	for(int i=0;i<16;i++){
		swap16 = swap2(prpt->RU_Iso[i]);
			memcpy(&buf[len], &swap16, 2);
			len += 2;
	}

	/*49. BAND Use, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->band_in_use[i];
	}
	/*53. Temp High, 2byte */
	swap16 = swap2(prpt->temp_high);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*54. Temp Offset, 2byte*/
	swap16 = swap2(prpt->temp_offset);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*55. Alarm Duration, 2byte*/
	swap16 = swap2(prpt->alarmDuration);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*56. TR_PWM, 2byte*/
	swap16 = swap2(prpt->tr_pwm);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*57. TR_SEL, 1byte*/
	buf[len++] = prpt->tr_sel;

	/*58. IR_PWM_BRx, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		swap16 = swap2(prpt->ir_pwm_br[i]);
		memcpy(&buf[len], &swap16, 2);
		len += 2;
	}

	/*62.IR_SEL_BRx, 4byte*/
	for (int i = 0; i < BR_NUM; i++) {
		buf[len++] = prpt->ir_pwm_sel[i];
	}

	/*66. TR_TxCount, 2byte*/
	swap16 = swap2(prpt->tr_txcount);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*67.TR_RxCount, 2byte*/
	swap16 = swap2(prpt->tr_rxcount);
	memcpy(&buf[len], &swap16, 2);
	len += 2;

	/*68. IR_BRx_TxRxCount, 16byte*/
	for (int i = 0; i < BR_NUM; i++) {
		swap16 = swap2(prpt->ir_br_txcount[i]);
		memcpy(&buf[len], &swap16, 2);
		len += 2;
		swap16 = swap2(prpt->ir_br_rxcount[i]);
		memcpy(&buf[len], &swap16, 2);
		len += 2;
	}

	/* 76. PSU_STATUS (1 byte) */
	buf[len++] = prpt->psu_status;

	/*77. LED Status, 20byte*/
	for (int i = 0; i < 20; i++) {
		buf[len++] = prpt->led_status[i];
	}

	/*78. LED_Test, 1byte*/
	buf[len++] = prpt->led_test;

	/*79. Reserved, 20byte*/
	for (int i = 0; i < 20; i++) {
		buf[len++] = prpt->reserved[i];
	}

	/* length */
	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;		// CRC

//	UART_Writes(uart_index, band,2);
	return len;
}



/* MainCtrlReq()
 */
void MainCtrlReq(char *buf, short *source_id, short  *dest_id)
{
	int						i;
	unsigned int 			swap32;
	unsigned int 			len = 0;
	unsigned char 			mask[5];
	unsigned short			swap16;
	struct repeater_struct	*prpt;
	struct ics_struct		*idev;
	char					*data;

	prpt = getRptDev();

	/* sync (4byte) */
	len+=4;

	/* 4 bytes source ID */
	memcpy((void *)source_id, &buf[len], 4);
	len += 4;

	/* 4 bytes destination ID */
	memcpy((void *)dest_id, &buf[len], 4);
	len += 4;

	/*	1Byte : CMD 	*/
	len+=1;

	/* 	2Byte : Length	*/
	len+=2;

	/* 1. Mask : 6byte */
	for(i=0;i<6;i++)
		mask[i] = buf[len++];

	/*	7. Reserved : 1Byte	*/
	len+=2;

	/*	8. Boot Initialization : 1Byte */
	if (mask[0] & 0x80) prpt->boot_initilization = buf[len];	
	len+=1;

	/*	9. Topology_Set : 1Byte */
	if (mask[0] & 0x40) prpt->Topology_Set = buf[len];	
	len+=1;

	/*	9. AGS_onoff : 1Byte */
	if (mask[0] & 0x20) prpt->AGS_onoff = buf[len];	
	len+=1;


	/*	10. ISO_onoff : 1Byte */
	if (mask[0] & 0x10) prpt->ISO_onoff = buf[len];	
	len+=1;

	// 11. serial number,(S/N) (R/W)*/
	if (mask[0] & 0x08) {
		for(int j=0;j<20;j++)
			prpt->repeater_sn[j] = buf[len+j];
	}
	len+=20; 

	if(mask[1] & 0x80){
		prpt->br_onoff[0] = buf[len];	
	}
	len+=1;

	if(mask[1] & 0x40){
		prpt->br_onoff[1] = buf[len];	
	}
	len+=1;

	if(mask[1] & 0x20){
		prpt->br_onoff[2] = buf[len];	
	}
	len+=1;

	if(mask[1] & 0x10){
		prpt->br_onoff[3] = buf[len];	
	}
	len+=1;


	if(mask[2] & 0x80){
		prpt->band_in_use[0] = buf[len];	
	}
	len+=1;
	
	if(mask[2] & 0x40){
		prpt->band_in_use[1] = buf[len];	
	}
	len+=1;
	
	if(mask[2] & 0x20){
		prpt->band_in_use[2] = buf[len];	
	}
	len+=1;
	
	if(mask[2] & 0x10){
		prpt->band_in_use[3] = buf[len];	
	}
	len+=1;
	
	if(mask[2] & 0x08){
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->temp_high = (s16)swap2(swap16);
	}
	len+=2;

	if(mask[2] & 0x04){
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->temp_offset = (s16)swap2(swap16);
	}
	len+=2;
	
	if(mask[2] & 0x02){
	
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->alarmDuration = (s16)swap2(swap16);
	}
	len+=2;

	if(mask[3] & 0x80){
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->tr_pwm = (s16)swap2(swap16);
	}
	len+=2;
	
	if(mask[3] & 0x40){
		// memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->tr_sel = buf[len];
	}
	len+=1;
	
	if(mask[4] & 0x80){
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->ir_pwm_br[0] = (s16)swap2(swap16);
	}
	len+=2;
	
	if(mask[4] & 0x40){
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->ir_pwm_br[1] = (s16)swap2(swap16);
	}
	len+=2;
	
	if(mask[4] & 0x20){
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->ir_pwm_br[2] = (s16)swap2(swap16);
	}
	len+=2;
	
	if(mask[4] & 0x10){
		memcpy((void *)&swap16, (void *)&buf[len], 2);
		prpt->ir_pwm_br[3] = (s16)swap2(swap16);
	}
	len+=2;

	if(mask[4] & 0x08){
		prpt->ir_pwm_sel[0] = buf[len];
		//memcpy((void *)&swap16, (void *)&buf[len], 2);
		//prpt->alarmDuration = (s16)swap2dev(swap16);
	}
	len+=1;

	if(mask[4] & 0x04){
//		memcpy((void *)&swap16, (void *)&buf[len], 2);
//		prpt->ir_pwm_sel[1] = (s16)swap2(swap16);
		prpt->ir_pwm_sel[1] = buf[len];
	}
	len+=1;

	if(mask[4] & 0x02){
//		memcpy((void *)&swap16, (void *)&buf[len], 2);
//		prpt->ir_pwm_sel[2] = (s16)swap2(swap16);
		prpt->ir_pwm_sel[2] = buf[len];
	}
	len+=1;

	if(mask[4] & 0x01){
//		memcpy((void *)&swap16, (void *)&buf[len], 2);
//		prpt->ir_pwm_sel[3] = (s16)swap2(swap16);
		prpt->ir_pwm_sel[3] = buf[len];
	}
	len+=1;

	if(mask[5] & 0x80){
		prpt->led_test = buf[len];
	}
	len+=1;


	/* Save Control Info */
	if (mask[0] != 0 || mask[1] != 0 || mask[2] != 0 || mask[3] != 0 || 
		mask[4] != 0 ) {
	//	saveRepeaterInfo(prpt);
	}

}

void AdcStatReq(char *buf, short source_id, short dest_id, short *band)
{
	unsigned int 	len = 0;
	unsigned int	bd=0;

	/* 3 bytes source ID */
	memcpy((void *)source_id, &buf[len], 1);
	len += 3;

	/* 3 bytes destination ID */
	memcpy((void *)dest_id, &buf[len], 1);
	len += 3;

	/*	1Byte : CMD 	*/
	len+=1;

	/* 	2Byte : Length	*/
	len+=2;

	memcpy((void *)&bd, &buf[len], 1);	len+=1;
	*band = bd;
}


int AdcStatAck(char *buf, frame_info *fr_info, unsigned char cmd) {

	unsigned short len = 0;
	unsigned short dlen;
	unsigned char sync = 0x16;
	unsigned short crc, swap16;
	int i;
	unsigned int swap32;

	struct repeater_adc_struct *prpt;
	prpt = getAdcRptDev();
	TestAdc(prpt);

	/* Message header */
	len = MsgHeaderCommon(buf, fr_info, cmd);
	swap16 = swap2(prpt->DL_DET_High_Band);
	memcpy(&buf[len], &swap16, 2);
	len+=2;
	swap16 = swap2(prpt->DL_DET_Low_Band);
	memcpy(&buf[len], &swap16, 2);
	len+=2;
	swap16 = swap2(prpt->UL_DET_High_Band);
	memcpy(&buf[len], &swap16, 2);
	len+=2;
	swap16 = swap2(prpt->UL_DET_Low_Band);
	memcpy(&buf[len], &swap16, 2);
	len+=2;
	swap16 = swap2(prpt->DC_DET);
	memcpy(&buf[len], &swap16, 2);
	len+=2;
	/* length */
	dlen = len - 15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	/* CRC */
	crc = Generate_CRC((char *)&buf[4], len - 4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2);
	len += 2;
	return len;
}

int TableStatAck(char *rx_buf, char *buf, frame_info *fr_info, unsigned char cmd) {
	unsigned short len = 0;
	unsigned short dlen;
	unsigned char sync = 0x16;
	unsigned short crc, swap16;
	int i;
	unsigned int swap32;
	struct repeater_table_query_struct *table;
	struct repeater_table_struct *prpt;

	/* 	Source ID */
	len += 4;

	/* Dest ID */
	len += 4;

	/* Command type */
	len+=1;

	/* lengh */
	len+=2;

	table = getTableQueryDev();
	table->Current_Band_in_Use = rx_buf[len++];
	table->type = rx_buf[len++];

	prpt = getTableRptDev();

	/* Fill test data */

	if (is_table_init == false){
		TestTable(prpt);
		is_table_init = true;
	}
	prpt->Table_Types = table->type;

	/* Message header */
	len = MsgHeaderCommon(buf, fr_info, cmd);
	buf[len++] = prpt->Table_Types;
	buf[len++] = prpt->Table_Size;
	int total_shorts = prpt->Table_Size * 2;
	for(int i =0;i<total_shorts; i++){
		if(i%2==0)
			swap16 = swap2(prpt->Actual_Table_Value[i]);
		else
			swap16 = swap2(prpt->Actual_Table_Value[i]);
		memcpy(&buf[len],&swap16,2);
		len+=2;
	}

	/* length */
	dlen = len - 15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	/* CRC */
	crc = Generate_CRC((char *)&buf[4], len - 4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2);
	len += 2;

	return len;
}
int TableCtrlReq(char *rx_buf, char *buf,  frame_info *fr_info, unsigned char cmd) {
	unsigned short len = 0;
	unsigned short dlen;
	unsigned char sync = 0x16;
	unsigned short crc, swap16;
	int i;
	unsigned int swap32;

	struct repeater_table_control_struct *table;
	struct repeater_table_struct *prpt;

	/* sync (4byte) */
	len+=4;

	/* 	Source ID */
	len += 4;

	/* Dest ID */
	len += 4;

	/* Command type */
	len+=1;

	/* lengh */
	len+=2;

	table = getTableControlDev();
	table->Current_Band_in_Use = rx_buf[len++];
	table->control_Varibale_Mask = rx_buf[len++];
	table->Table_types = rx_buf[len++];
	table->Table_Size = rx_buf[len++];

	for(int i =0;i<(table->Table_Size);i++){
		memcpy(&swap16, &rx_buf[len], 2);
		table->Table_Value[i*2+1] = swap2(swap16);
		len+=2;
	}

	prpt = getTableRptDev();

	prpt->Table_Types = table->Table_types;
	prpt->Table_Size = table->Table_Size;
	for(int i =0;i<(prpt->Table_Size);i++){
		memcpy(&prpt->Actual_Table_Value[i*2+1], &table->Table_Value[i*2+1], 2);
	}

	/* Message header */
	len = MsgHeaderCommon(buf, fr_info, cmd);
	buf[len++] = prpt->Table_Types;
	buf[len++] = prpt->Table_Size;
	int total_shorts = prpt->Table_Size * 2;
	for(int i =0;i<total_shorts; i++){
		swap16 = swap2(prpt->Actual_Table_Value[i]);
		memcpy(&buf[len],&swap16,2);
		len+=2;
	}

	/* length */
	dlen = len - 15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	/* CRC */
	crc = Generate_CRC((char *)&buf[4], len - 4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2);
	len += 2;

	return len;
}

int sysLmtMaxBuffSize(void)
{
	return (1024);
}

struct fpga_desc	fpga_list[6] = {
	{"FPGA_X",0x0      , 0x0600000,0x00600000,'X'},
	{"FPGA_Y",0x0600000, 0x0C00000,0x00600000,'Y'},
	{"FPGA_Z",0x0C00000, 0x1200000,0x00600000,'Z'},
	{"FPGA_U",0x1200000, 0x1800000,0x00600000,'U'},
	{"FPGA_V",0x1800000, 0x2000000,0x00600000,'V'},
	{"FPGA_W",0x2000000, 0x2400000,0x00600000,'W'},
};

int DownKickReq(char *buf, struct downDesc *pdesc, struct downParam *param)
{
	struct repeater_struct *prpt;
	prpt = getRptDev();

	unsigned int	len = 0;
	unsigned int	swap;
	int 			i;

	/* sync (4byte) */
	len+=4;

	/*	Source ID */
	len += 4;

	/* Dest ID */
	len += 4;

	/* Command type */
	len+=1;

	/* lengh */
	len+=2;

	memset((void *)pdesc, 0, sizeof(struct downDesc));
	memcpy((void *)&swap, &buf[len], 2);	len+=2; 			/*	Total Frame   */
	param->totFrame = swap2(swap);

	param->max_size = sysLmtMaxBuffSize();

	//	1: Modem firmware
	//	2: MB App
	//	3: FPGA App

	memcpy((void *)&pdesc->filetype, &buf[len], 1); len+=1; 	/*	File Type	*/

	prpt->filetype = pdesc->filetype;


	param->max_size = sysLmtMaxBuffSize();

	memcpy((void *)&swap, &buf[len], 4);	len+=4; 			/*	File Size	*/	
	pdesc->filesize = swap4(swap);
	param->filesize = pdesc->filesize;
	param->totCRC = swap2(swap);
//	if(Debug_Prn)
		printf("0 totCRC [%x]>[%d]\n",param->totCRC,param->filesize);

	param->down_status = 0;
	param->f_offset = 0;
	param->realFileDataSize = 0;

	return 0;
}



int DownKickAck(char *buf, short *source_id, short *dest_id, unsigned char cmd, struct downDesc *pdesc, struct downParam *param, int ack)
{
	unsigned int	len = 0;
	unsigned short	dlen;
	unsigned char	sync = 0x16;
	unsigned short	swap16;
	unsigned int	crc;
	unsigned long	swap32;

	/* SYNC */
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;


	/*	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* CMD */
	buf[len++] = cmd;

	/* lengh */
	len+=2;

	swap16 = swap2(param->totFrame);
	memcpy((void *)&buf[len], (void *)&swap16, 2); len+=2;

	buf[len++] = ack;

	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2; 	// CRC

	return(len);
}


int DownDataReq(char *buf, struct downDesc *pdesc, struct downParam *param)
{
	unsigned int	len = 0;
	unsigned int	swap;
	short			realsize;
	short			totFrame;
	short			curFrame;
	short			fileReset, share, rest, i;
	unsigned int	msgSize;
	unsigned int	fileDataSize, realFileDataSize;
	unsigned int	tSize;
	unsigned char	dnBuf[1024];

	/* sync (4byte) */
	len+=4;

	/*	Source ID */
	len += 4;

	/* Dest ID */
	len += 4;

	/* Command type */
	len+=1;

	memcpy((void *)&swap, &buf[len], 2); len+=2;		/* length */
	msgSize = swap2(swap);

	memcpy((void *)&swap, &buf[len], 2);	len+=2; 	/*buf[4]	Total Frame   */
	totFrame = swap2(swap);

	memcpy((void *)&swap, &buf[len], 2);	len+=2; 	/*buf[6]	Current Frame */
	curFrame = swap2(swap);

	param->curFrame = curFrame;
	
	// Verify file data length 
	fileDataSize = msgSize - 4; 						// this value calculated from msg size parameter
	
	memcpy(dnBuf, buf+len, fileDataSize);
	realFileDataSize = param->realFileDataSize + fileDataSize;

	param->realFileDataSize = realFileDataSize;
	pdesc->realFileDataSize = realFileDataSize;

	if (fileDataSize != realFileDataSize)
	{
//		return 1;
	}

	// Copy data to the allocated buffer
	param->f_offset = (realFileDataSize - fileDataSize);
//	if(Debug_Prn)
//		printf("DownKickReq [%d][%d][%d]:[%d][%d]\n",totFrame,curFrame,fileDataSize,realFileDataSize,param->f_offset );

	memcpy((void *)&download_pool[param->f_offset], &buf[len], fileDataSize);	

	return 0;
}


int DownDataAck(char *buf, short *source_id, short *dest_id, unsigned char cmd, struct downDesc *pdesc, struct downParam *param, int ack)
{
	unsigned int	len = 0;
	unsigned short	dlen;
	unsigned char	sync = 0x16;
	unsigned short	swap16;
	unsigned int	crc;

	/* SYNC */
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/*	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* CMD */
	buf[len++] = cmd;

	/* lengh */
	len+=2;
	
	swap16 = swap2(param->totFrame);
	memcpy((void *)&buf[len], (void *)&swap16, 2); len+=2;
	swap16 = swap2(param->curFrame);
	memcpy((void *)&buf[len], (void *)&swap16, 2); len+=2;
	/* ACK */
	buf[len++] = ack;		//0:ACK, 1:NACK, 2:Ready

	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2; 	// CRC

	return(len);
}


int DownTermReq(char *buf, struct downDesc *pdesc, struct downParam *param)
{
	struct repeater_struct *prpt;
	prpt = getRptDev();

	unsigned int			len = 0;
	unsigned int			swap;
	unsigned int size;	
	u32 FlashAddr;
//	unsigned char			pl_mode,app_mode;
		
	unsigned short			crc,crc_gen;

	/* sync (4byte) */
	len+=4;

	/*	Source ID */
	len += 4;

	/* Dest ID */
	len += 4;

	/* reserved */
	len+=1;
	/* cmd */
	len+=1;
	/* length */
	len+=2;
	
	memcpy((void *)&swap, &buf[len], 2);	len+=2; 	/*	Total Frame   */
	memcpy((void *)&swap, &buf[len], 2);	len+=2; 	/*	Current Frame	*/

	crc_gen=Generate_CRC((char *)download_pool, param->realFileDataSize);
//	crc_gen=Generate_CRC((char *)download_pool, param->filesize);

	printf("0 totCRC [%x][%x]>[%d][%d]\n",param->totCRC,crc_gen,param->realFileDataSize,param->filesize);

	prpt->realFileDataSize = param->realFileDataSize;
	prpt->crc_gen = crc_gen;

	//if(Debug_Prn)
	//	printf("DownTermReq [%d][%d]> CRC[%x] filetype[%d]totFrame[%d]\n",pdesc->filesize,param->realFileDataSize,crc_gen,pdesc->filetype,param->totFrame);

	{
		//if(Debug_Prn)
		//	printf("FPGA filetype[%d] pl_mode[%d],app_mode[%d]\n",pdesc->filetype, prpt->pl_mode, prpt->app_mode);

		if(prpt->filetype == FILETYPE_FPGA)
		{
			if( prpt->pl_mode == 0x00)
			{
				prpt->pl_mode = 0x02;
			}
			else
			{
				prpt->pl_mode = 0x00;
			}
	
			if( prpt->pl_mode == 0x00)
			{
				Serial_Download_write(SPIF_PLX_IMAGE_BASE,pdesc->realFileDataSize,&download_pool[0],FILETYPE_FPGA);
			}
			else
			{
				Serial_Download_write(SPIF_PLZ_IMAGE_BASE,pdesc->realFileDataSize,&download_pool[0],FILETYPE_FPGA);
			}
			BootInfoTbl(prpt->pl_mode,prpt->app_mode);
			
		//	if(Debug_Prn)
		//		printf("FPGA filetype[%d] pl_mode[%d],app_mode[%d]\n",pdesc->filetype, prpt->pl_mode, prpt->app_mode);
		}
		else if(prpt->filetype == FILETYPE_FW_MB)
		{
			if( prpt->app_mode == 0x00)
			{
				prpt->app_mode = 0x01;
			}
			else
			{
				prpt->app_mode = 0x00;
			}

			Serial_Download_write(SPIF_APPX_IMAGE_BASE+SPIF_APP_IMAGE_SIZE*prpt->app_mode,pdesc->realFileDataSize,&download_pool[0],FILETYPE_FW_MB);

			CrcInfoTbl(SPIF_APPX_IMAGE_BASE+SPIF_APP_IMAGE_SIZE*prpt->app_mode, pdesc->realFileDataSize,crc_gen);
			BootInfoTbl(prpt->pl_mode,prpt->app_mode);

		//	if(Debug_Prn)
		//		printf("App filetype[%d] pl_mode[%d],app_mode[%d]\n",pdesc->filetype, prpt->pl_mode, prpt->app_mode);

		}
		else if(prpt->filetype == FILETYPE_PL_Y)
		{
			Serial_Download_write(SPIF_PLY_IMAGE_BASE,pdesc->realFileDataSize,&download_pool[0],FILETYPE_PL_Y);
		//	printf("PL_Y filetype[%d] \n",pdesc->filetype);
		}
		else if(prpt->filetype == FILETYPE_uMon)
		{
		//	printf("uMon filetype[%d] \n",pdesc->filetype);
			Serial_Download_write(SPIF_UMON_IMAGE_BASE,pdesc->realFileDataSize,&download_pool[0],FILETYPE_uMon);
		}
		else if(prpt->filetype == FILETYPE_ALL)
		{
		//	printf("All filetype[%d] \n",pdesc->filetype);
			Serial_Download_write(SPIF_APPX_IMAGE_BASE,pdesc->realFileDataSize,&download_pool[0],FILETYPE_ALL);
		}
	}
	//		GUIDownload_complte();
	return 0;
}


int Flash_write(void)
{

	return 1;
}


int DownTermAck(char *buf, short *source_id, short *dest_id, short totFrame, short curFrame, unsigned char cmd,struct downParam *param, int ack)
{
	unsigned int	len = 0;
	unsigned short	dlen;
	unsigned char	sync = 0x16;
	unsigned short	swap16;
	unsigned int	crc;
	extern	int Reboot();

	/* SYNC */
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/*	Source ID */
	memcpy((void *)&buf[len], (void *)source_id, 4);
	len += 4;

	/* Dest ID */
	memcpy((void *)&buf[len], (void *)dest_id, 4);
	len += 4;

	/* CMD */
	buf[len++] = cmd;

	/* lengh */
	len+=2;

//	swap16 = swap2(totFrame);
	swap16 = swap2(param->totFrame);
	memcpy((void *)&buf[len], (void *)&swap16, 2); len+=2;
//	swap16 = swap2(curFrame);
	swap16 = swap2(param->curFrame);
	memcpy((void *)&buf[len], (void *)&swap16, 2); len+=2;

	/* ACK */
	buf[len++] = ack;//0:ACK, 1:NACK, 2:Ready

	dlen = len-15;
	dlen = swap2(dlen);
	memcpy((void *)&buf[13], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2; 	// CRC

	return(len);
}

int TblStatReq(char *buf, short *source_id, short *dest_id, short *band, char *tblType, struct tbl_desc **pdesc)
{
	int 	len = 0;
	uchar	type;
	unsigned short	bd=0;

	/* 3 bytes source ID */
	memcpy((void *)source_id, &buf[len], 4);
	len += 3;

	/* 3 bytes destination ID */
	memcpy((void *)dest_id, &buf[len], 4);
	len += 3;

	/*	1Byte : CMD 	*/
	len+=1;

	/* 	2Byte : Length	*/
	len+=2;

	memcpy((void *)&bd, &buf[len], 1);	len+=1;  	 /*	band select 1byte*/
	*band = bd;

	memcpy((void *)&type, &buf[len], 1);	len+=1;

	if (type >= NUMB_TBL) {
		*tblType = 0;
		*pdesc = 0;
		return (-1);
	}
	type += bd *20;

	*tblType = type;
	*pdesc = getTblDesc(type);

	//(*pdesc)->Cal((*pdesc)->fname, (*pdesc)->table);
	return (0);

}

void SysResetAck(char *buf, unsigned char cmd , char chan)
{
	unsigned int 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned int 	crc;
	short 			uart_index = UART_TR;

	extern 	int Reboot();

	/* SYNC */
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* reserved */
	len+=1;

	/* CMD */
	buf[len++] = cmd;

	/* Length */
	len+=2;

	/* CRC Insertion */
	dlen = len-8;
	dlen = swap2(dlen);
	memcpy((void *)&buf[6], (void *)&dlen, 2);

	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;

	if(!chan)
		uartWrites(uart_index, (u8*) buf,len);
	//else
	//	putbytes2(buf,len);

	//OSTimeDly(1000);
	Reboot();

}


uint16_t logDataCnt()
{
	return 0;
}

int errorStatus(char *buf, char *errMsg)
{
	unsigned int 	len = 0;
	unsigned short 	dlen;
	unsigned char 	sync = 0x16;
	unsigned int 	crc;
	unsigned short  msgLen = strlen(errMsg);

	/* SYNC */
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;
	memcpy((void *)&buf[len], (void *)&sync, 1); len+=1;

	/* reserved */
	len+=1;

	/* CMD */
	buf[len++] = ERROR_STATUS;

	/* Length */
	len+=2;

	/* Error Message */
	memcpy((void *)&buf[len], (void *) errMsg, msgLen); len+=msgLen;

	// Msg data length calculation
	dlen = len-8;
	dlen = swap2(dlen);
	memcpy((void *)&buf[6], (void *)&dlen, 2);

	/* CRC Insertion */
	crc=Generate_CRC((char *)&buf[4],len-4);
	crc = swap2(crc);
	memcpy((void *)&buf[len], (void *)&crc, 2); len+=2;

	return(len);
}

void StartTopologyBuilding(mu_dev_info *p_mu_ptr)
{
    frame_info fw_fr_info;
    unsigned int tx_len;
//    u8 tx_buf[TOPOLOGY_MSG_SIZE];

	// Reset MU information
	memset(p_mu_ptr, 0, sizeof(mu_dev_info));

    fw_fr_info.srcID_type = 0x13;
    fw_fr_info.srcID_branch = 0;
	fw_fr_info.srcID_port = 0;
    fw_fr_info.srcID_id = 0;	

    fw_fr_info.destID_type = DEV_TYPE_RU;
	fw_fr_info.destID_branch = p_mu_ptr->mu.branch_id;
	fw_fr_info.destID_id = 0;  // first RU always id 0

	// Initialize branch status
	for (int index = 0; index < RU_NUM_MAX; index++)
	{
		p_mu_ptr->top_status[index] = TOPOLOGY_STARTED;
	}

	// Set tick start time
	p_mu_ptr->top_start = xTaskGetTickCount();

    // Send TOPOLOGY_REQ to first RU BR1
    fw_fr_info.destID_port = 0;
    tx_len = createForwardFrame(TOPOLOGY_REQ, NULL, 0, tx_buf, &fw_fr_info);	
    uartWrites(UART_IR_BR1, tx_buf, tx_len);
	// xil_printf("<-- [BR1] [%d] : ", TOPOLOGY_REQ);
    // logPackage(tx_buf, tx_len);

	
    // Send TOPOLOGY_REQ to first RU BR2
    fw_fr_info.destID_port = 1;
    tx_len = createForwardFrame(TOPOLOGY_REQ, NULL, 0, tx_buf, &fw_fr_info);
    uartWrites(UART_IR_BR2, tx_buf, tx_len);
	// xil_printf("<-- [BR2] [%d] : ", TOPOLOGY_REQ);
    // logPackage(tx_buf, tx_len);

    // Send TOPOLOGY_REQ to first RU BR3
    fw_fr_info.destID_port = 2;
    tx_len = createForwardFrame(TOPOLOGY_REQ, NULL, 0, tx_buf, &fw_fr_info);
    uartWrites(UART_IR_BR3, tx_buf, tx_len);
	// xil_printf("<-- [BR3] [%d] : ", TOPOLOGY_REQ);
    // logPackage(tx_buf, tx_len);

    // Send TOPOLOGY_REQ to first RU BR4
    fw_fr_info.destID_port = 3;
    tx_len = createForwardFrame(TOPOLOGY_REQ, NULL, 0, tx_buf, &fw_fr_info);
    uartWrites(UART_IR_BR4, tx_buf, tx_len);   
	// xil_printf("<-- [BR4] [%d] : ", TOPOLOGY_REQ);
    // logPackage(tx_buf, tx_len);
}

void logMuInfo(mu_dev_info *mu_info)
{
	int index;

	xil_printf("------------- MU info ---------\r\n");
	xil_printf("MU device id: %d \r\n", mu_info->mu.device_id);
	
	xil_printf("BR1: %d RUs \r\n  ", mu_info->br1_ru_number);
	for (index = 0; index < mu_info->br1_ru_number; index++)
	{
		xil_printf("[%d]:%d,%d,%d ", index, mu_info->ru_br1[index].branch_id, mu_info->ru_br1[index].port_id, mu_info->ru_br1[index].device_id);
	}
	xil_printf("\r\n");

	xil_printf("BR2: %d RUs \r\n  ", mu_info->br2_ru_number);
	for (index = 0; index < mu_info->br2_ru_number; index++)
	{
		xil_printf("[%d]:%d,%d,%d ", index, mu_info->ru_br2[index].branch_id, mu_info->ru_br2[index].port_id, mu_info->ru_br2[index].device_id);
	}
	xil_printf("\r\n");

	xil_printf("BR3: %d RUs \r\n  ", mu_info->br3_ru_number);
	for (index = 0; index < mu_info->br3_ru_number; index++)
	{
		xil_printf("[%d]:%d,%d,%d ", index, mu_info->ru_br3[index].branch_id, mu_info->ru_br3[index].port_id, mu_info->ru_br3[index].device_id);
	}
	xil_printf("\r\n");	

	xil_printf("BR4: %d RUs \r\n  ", mu_info->br4_ru_number);
	for (index = 0; index < mu_info->br4_ru_number; index++)
	{
		xil_printf("[%d]:%d,%d,%d ", index, mu_info->ru_br4[index].branch_id, mu_info->ru_br4[index].port_id, mu_info->ru_br4[index].device_id);
	}
	xil_printf("\r\n");	

	xil_printf("Topology status: ");
	for (index = 0; index < RU_NUM_MAX; index++)
	{
		xil_printf(" [%d] ", mu_info->top_status[index]);
	}
	xil_printf("\r\n");

	TickType_t curTick = xTaskGetTickCount();
	xil_printf("Time from topology start: %d ms\r\n", pdTICKS_TO_MS((curTick - mu_info->top_start)));

	xil_printf("---------------------------\r\n");
}


int createForwardFrame(int cmd, char *rx_buf, unsigned int rx_len, u8 *tx_buf, const frame_info *fr_info)
{
    unsigned int len = 0;
    unsigned int crc, swap16;

	len = MsgHeaderCommon(tx_buf, fr_info, cmd);

    // Payload length (2 bytes)
    swap16 = swap2(rx_len); // Swap bytes for little-endian
    memcpy((void *)&tx_buf[13], (void *)&swap16, 2);

    // Copy payload
    if (rx_len > 0)
	{		
        memcpy((void *)&tx_buf[len], (void *)rx_buf, rx_len); len += rx_len;
    }

    // Generate CRC16 for the entire frame (excluding sync)
    crc = Generate_CRC((char *)&tx_buf[4], len - 4);

    // Append CRC16 (2 bytes)
    swap16 = swap2(crc); // Swap bytes for little-endian
    memcpy((void *)&tx_buf[len], (void *)&swap16, 2); len += 2;

    return len;
}


void fillTestMuInfo(mu_dev_info *mu_info)
{
	int index = 0;

	mu_info->mu.type = DEV_TYPE_MU;
	mu_info->mu.branch_id = 0x2;
	mu_info->mu.device_id = 0x10;

	mu_info->br1_ru_number = 4;
	for (index = 0; index < RU_NUM_MAX; index++)
	{
		mu_info->ru_br1[index].device_id = index;
		mu_info->ru_br1[index].type = DEV_TYPE_RU;
	}

	mu_info->br2_ru_number = 4;
	for (index = 0; index < RU_NUM_MAX; index++)
	{
		mu_info->ru_br2[index].device_id = index + 4;
		mu_info->ru_br2[index].type = DEV_TYPE_RU;
	}

	mu_info->br3_ru_number = 4;
	for (index = 0; index < RU_NUM_MAX; index++)
	{
		mu_info->ru_br3[index].device_id = index + 8;
		mu_info->ru_br3[index].type = DEV_TYPE_RU;
	}

	mu_info->br4_ru_number = 4;
	for (index = 0; index < RU_NUM_MAX; index++)
	{
		mu_info->ru_br4[index].device_id = index + 12;
		mu_info->ru_br4[index].type = DEV_TYPE_RU;
	}

	for (index = 0; index < RU_NUM_MAX; index++)
	{
		mu_info->top_status[index] = TOPOLOGY_COMPLETED;
	}
}



void IR_TopologyStatReq(char *buf, unsigned char *source_id, unsigned char *dest_id)
{
	struct repeater_struct *prpt;
	prpt = getRptDev();

	unsigned int 	len = 0;
	unsigned int	bd=0;

	/* sync (4byte) */
	len+=4;

	/*	4 Bytes : Source Id , we read first byte only	*/
	memcpy((void *)source_id, &buf[len], 4);
	len += 4;

	/* 	4 Bytes : Destination ID, we read first byte only 	*/
	memcpy((void *)dest_id, &buf[len], 4);
	len += 4;

	/* 	1 Byte : Command	*/
	len += 1;

	/* 	2 Byte : Length	*/
	len += 2;


//	prpt->ir_ru_id 				= buf[len];
//	len+=1;
	prpt->ir_ru_id 				= buf[len++];
//	prpt->children_ru_number 	= buf[len++];
	prpt->ir_children_ru_number 	= buf[len++];


	prpt->ir_buff[0] 	= buf[len++];
	prpt->ir_buff[1] 	= buf[len++];
	prpt->ir_buff[2] 	= buf[len++];
	prpt->ir_buff[3] 	= buf[len++];


//	printf("IR Topolog req [%d][%d]>[%x][%x][%x][%x]\n", prpt->ir_ru_id, prpt->children_ru_number,prpt->ir_buff[0],prpt->ir_buff[1],prpt->ir_buff[2],prpt->ir_buff[3]);

	printf("IR Topolog req [%d]:[%d][%d]>[%x][%x][%x][%x]\n", prpt->ir_ru_id, prpt->children_ru_number[3], prpt->ir_children_ru_number,prpt->ir_buff[0],prpt->ir_buff[1],prpt->ir_buff[2],prpt->ir_buff[3]);

}

