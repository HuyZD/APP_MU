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

static char tx_buf[2500]; 	 //2048
static char rx_buf[2500];    //2048


extern mu_dev_info mu_info;
char *RuHelp[] = {
		"Ru		<branchId>		<RuId>",
		"Send main packet to Ru",
		"<branchId> : BR1 ... BR4 or 1, 2, 3, 4",
		"<RuId>: 0 ... 3",
		0
};

int RuCmd(int argc, char *argv[])
{
    short uart_index = 0;
    frame_info fr_info;
//    unsigned char tx_buf[256];
    int len = 0;
    int id;

    if (argc < 3) {
        xil_printf("Usage: Ru <BR1...BR4 / TR> <RuId>\r\n");
        xil_printf("Example: Ru Br1 1\r\n");
        return -1;
    }

    id = atoi(argv[2]);

	if (strcasecmp(argv[1], "Br1") == 0 || strcmp(argv[1], "1") == 0) {
        uart_index = UART_IR_BR1;
        fr_info.destID_port = 0x00;
    }
	else
	{
		xil_printf("Not supported\r\n");
		return -1;
	}

    mu_info.mu.type = DEV_TYPE_MU;	
	mu_info.mu.branch_id = 1;
	mu_info.mu.port_id = 1;
    mu_info.mu.device_id = 0; 

    fr_info.srcID_type   = mu_info.mu.type;
    fr_info.srcID_branch = mu_info.mu.branch_id;
    fr_info.srcID_port   = mu_info.mu.port_id;
    fr_info.srcID_id     = mu_info.mu.device_id;

    fr_info.destID_type   = DEV_TYPE_RU;
    fr_info.destID_branch = mu_info.mu.branch_id;;
    //fr_info.destID_port   = 1;
    fr_info.destID_id     = (unsigned char)id;

    len = MainStatReq(tx_buf, &fr_info);

    if (len > 0) {
        uartWrites(uart_index, tx_buf, len);
        xil_printf("[RuCmd] Send frame MainStatReq len=%d to %s (UART %d), RU_ID=%d\r\n",
                   len, argv[1], uart_index, id);
    } else {
        xil_printf("[RuCmd] Error frame (len <= 0)!\r\n");
        return -1;
    }

    return 0;
}
