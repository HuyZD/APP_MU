#include "genlib.h"
#include "xparameters.h"
#include "stdlib.h"
#include <xil_printf.h>
#include <string.h>
//#include "xgpiops.h"
#include "xstatus.h"
#include "user.h"
#include "uart.h"
//#include "sgdma.h"
//#include "sd_dma.h"

uint8_t monitor_cpri = 0;
uint8_t	monitor_hdlc = 0;
int 	set			 = 0;

// Easter Eggs
char *WhatHelp[] = {
		":D :D :D :D",
		"what",
        0,
};

int
What(int argc,char*argv[])
{
	printf(";;;;;;;+++++++++++++++++++++xXX$$X$XXXXXx+++++++++++++++++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;;;;++++++++xxxxX$$$$$$$XXXXx++++++++++++++++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;;;;;++++++++xxxxxxxxxxxxxXXX++;+++++++;;++++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;;;;;;;+++++XXxxxxxxxxxx+xxXX++;;++++++;;;;++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;;;;;;;;++++XXxXXXXxxxxx+xxXx++;;;;+++++;++++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;;;;;+++++++xxxXXXXXXxXXxxxXx++++;;++++;;;+++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;;;;;+++++xx++xxxxXXxxxxxxx++++++++++++++++++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;;++++++++xxxxxxXXXXXxx++xx++++++++++++++++++++++++++++++++++\n");
	printf(";;;;;;;;;;;;;;+++++++;++xxxxxxXxxx+xxxxx+++;+++++++++++++;;;;+++++++++++++\n");
	printf(";;;;;;;;;;;;;++++++;;;;;;+xxxxXXxXXxxx++;;;;;++++++++++++;;;;;;+++++++++++\n");
	printf(";;;;;;;;;;;;;;++;;;;;;;;;+xxxxxXXxxxx+;;;;;;;;++++++++++;;;;;;;;;+++++++++\n");
	printf(";;;;;;;;;;;++++;++++++++++xxxxxxxxxxx+++++++;;+;;++++++;;;;;;;;;;;;;++++++\n");
	printf(";;;;;;;;++++++++++++++++++xxxxXxxxxxxxXx++++++++++++++++++++++++++++++++++\n");
	printf(";;;;;;;;;;+++++;;;;;;;+++xxxxxxxxxxx+x$$$Xxx++++;+++++++;;;;;;;;;;;;;+++++\n");
	printf(";;;;;;;;;++++++;;+++xxx++xxxxxXXxxx++x$$$$$XXXXXXXx++++++++++;+;;;;+++++++\n");
	printf(";;;;;;;;;+++++++xXX$$$X++xxxxxXXXx+++X$$$$$$$$$$$$$$$$X+++++++;+;;;+++++++\n");
	printf(";;;;;;;;;+++xxXX$$$$$$X++XxxxxXx++++X$$$$$$$$$$$$$$$$$$X++++++;;;;;+++++++\n");
	printf(";;;;;;;;+++X$$$$$$$$$$$+xxxx++x++++X$$$$$$$$$$$$$$$$$$$X++++++++;;++++++++\n");
	printf(";;;;;;;;+++X$$$$$$$$$$$$$X+++xxxxxXX$$$$$$$$$X$$$$$$$$$$++++++++++++++++++\n");
	printf(";;;;;;;;+++$$$$$$$$$$$$$$X++xxxxxxxX$$$$$$$$$XxX$$$$$$$X++++++++++++++++++\n");
	printf(";;;;+++++++X$$$$$$$$$$$$$XxxxXXxxxxX$$$$$$$$$XXX$$XxX$X+++++++++++++++++++\n");
	printf(";;;;;;;++++X$$$$$$$$$$$$$$x+XXXXxxxX$$$$$$$$$XXxXxxxX$$++++++++++++++++;+;\n");
	printf(";;;;+;;++++x$$$$$$$$$$$$$$XX$XXXXXXX$$$$$$$$$XXXXxxx$$x+++++++++++++++;+;;\n");
	printf("+++++++++++X$$$$$$$$$$$$$$Xx$$$XXXXX$$$$$$$$$XXxxxxx$$$$++++++++++++++++++\n");
	printf(";;;;+;+++++X$$$$$$$$$$$$$$XX$$$$XXXX$$$$$$$$XXXXxxxx$$$$$++++++++++++++;++\n");
	printf(";;;++++++++x$$$$$$$$$$$$$$XX$$$$XXXXX$$$$$$$$XxxxxxX$$$$$X++++++++++++++++\n");
	printf("++++++++++++$$$$$$$$$$$$$$XX$$$$$$XXXX$$$$$$$$$XXXxX$$$$$$X+++++++++++++++\n");
	printf(";;++++++++++$$$$$$$$$$$$$$XXXX$$$$$$$$$$$$$&&&&$$$$$$$$$$$$x++++++++++++++\n");
	printf(";;++++++++++$$$$$$$$$$$$$$XXXXX$$$$$$$$$$$$&&&&&$$$$$$$$$$$X++++++++++++++\n");
	printf(";;+++++++++x$$$$$$$$$$$$$$XXXXXXX$$$$$$$$$$$$&&&&&$$$$XX$$$X++++++++++++++\n");
	return 0;
}

// Read/write Register of FPGA
char *RegHelp[] = {
		"Read/Write the Register in FPGA",
        "-r 	[Adress]",
		"-[s/c] [Pin Number]",
        "Options:",
        " -r   Read the Register",
		" -s   Set EMIO Pins Number",
		" -c   Clear EMIO Pins Number",
        0,
};

int
Reg(int argc, char *argv[])
{
    int     		opt;
    u32 			value_x = 0;
    u32 			value_w = 0;
    u32				reg;
    int				pin_nb;
    char 			*arg3;

    arg3 = argv[optind+2];

    while((opt=getopt(argc,argv,"r:s:c:w:")) != -1) {
        switch(opt) {

        case 'r':
        	value_x = (u32) strtol(optarg,NULL,16);
//        	reg = Xil_In32(value_x);
        	printf("Address %s return: 0x%x\n", optarg, reg);
        	break;

        case 'w':
            value_x = (u32) strtol(optarg,NULL,16);
            value_w = (u32) strtol(arg3,NULL,16);
//            Xil_Out32(value_x,value_w);
            printf("Write address %s value : %d\n", optarg, value_w);
            break;

//        case 's':
//        	pin_nb = atoi(optarg);
//        	write_pin(pin_nb, 0x1);
//        	printf("OK %d is %x \n", pin_nb, read_pin(pin_nb));
//        	break;
//
//        case 'c':
//        	pin_nb = atoi(optarg);
//        	write_pin(pin_nb, 0x0);
//        	printf("OK %d is %x \n", pin_nb, read_pin(pin_nb));
//        	break;

        case '?':
            printf("Unknown option\n");
            break;
        default:
            return 0;
        }
    }

    return 1;
}

char *MonHelp[] = {
		"Monitor CPRI Status",
		"mon cpri",
		"Turn off monitor by command 's'",
        0,
};

int
mon(int argc, char *argv[])
{
	char 			*arg1;

	if (argc < 2) {
	    printf("No arguments, exiting... use help\n");
	    return(0);
	}

	arg1 = argv[optind];

	if (strcmp(arg1,"cpri") == 0) {
			monitor_cpri = 1;
			printf("Monitor CPRI Status: ON\n");
	}

	if (strcmp(arg1,"hdlc") == 0) {
			monitor_hdlc = 1;
			printf("Monitor HDLC RX Data: ON\n");
	}

	return 1;
}

char *hdlcHelp[] = {
		"HDLC Tx and Rx data",
		"hdcl tx/rx [data] [size]",
        0,
};

int
hdlc(int argc, char *argv[])
{
	char 			*arg1;
	char			*arg2;
	char			*rx_payload;
//	int				size;
	int 			numb;
//	char			data = 'abcdefgh';
//	u8 				*data_a = "abcdefgh";

	if (argc < 2) {
	    printf("No arguments, exiting... use help\n");
	    return(0);
	}

	arg1 = argv[optind];
	arg2 = argv[optind+1];

	if (strcmp(arg1,"tx") == 0) {
		if (strcmp(arg2,"") == 0){
			printf("Tx value can't be empty\n \r");
		} else {
		/*numb = UART1_Puts(arg2,8);*/}
//		numb = UART1_Puts(arg2,size);
		printf("Transfer size: %d byte\n \r",numb);
	}
	else if (strcmp(arg1,"rx") == 0) {
//		UART1_Return(rx_payload);
//		uart_r = UART1_ReturnChar;
//		printf("Rx: %c\n \r",(char)UART1_ReturnChar());
		printf("Rx: %s\n \r",rx_payload);
//		printf("Rx EMPTY!\n \r");

	}

	return 1;
}

char *clcHelp[] = {
	"Clear screen",
     0,
};

int clc(int argc, char *argv[])
{
	printf("\033[2J\033[H");
	return 1;
}

int s(int argc, char *argv[])
{
	set = 0;
	monitor_cpri = 0;
	monitor_hdlc = 0;
	return 1;
}

char *sHelp[] = {
	"Stop",
     0,
};

//char *dataHelp[] = {
//	"Set and On DMA",
//	 0,
//};
//
//int data(int argc, char *argv[])
//{
////	int     		opt;
////	unsigned int	time_m;
//	char 			*arg1,*arg2;
//	u64 			*Buf;
//	u64 			BufLength = 128;
//	u32 			fileSize;
//
//	Buf = (u64 *) TX_BUFFER_BASE;
//
//	if (argc < 2) {
//	    printf("No arguments, exiting... use help\n");
//	    return(0);
//	}
//
//	arg1 = argv[optind];
//	arg2 = argv[optind+1];
//
//	if (strcmp(arg1,"on") == 0) {
//		if(strcmp(arg2,"1") == 0){
//
//			for(int i=0;i<128;i++){
//				Buf[i] = sine_lut[i] * 0x100000;
//			}
//
//			SGDMA_Send(Buf, BufLength*sizeof(*Buf));
//			printf("Send data Sine LUT\n");
//		}
//		if(strcmp(arg2,"2") == 0){
//
//			for(int i=0;i<1024;i++){
//				Buf[i] = sine_lut_iq[i];
//			}
//
//			SGDMA_Send(Buf, BufLength*sizeof(*Buf));
//			printf("Send data Sine LUT IQ\n");
//		}
//		if(strcmp(arg2,"5G") == 0){
//
//			fileSize = load_data("tm3p1a_fdd.bin",TX_BUFFER_BASE);
//			printf("Load Data 5G Completed: %d Byte\n",fileSize);
//		}
//	}
////	else if (strcmp(arg1,"on") == 0){
////		dma_send(axidma,TX_BUFFER_BASE,sizeof(sine_lut_iq));
////		printf("Data Enable \n");
////	}
//
//	return 1;
//}





