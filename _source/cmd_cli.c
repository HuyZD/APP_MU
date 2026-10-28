#include <FreeRTOS.h>
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xil_io.h"
#include "stdlib.h"
#include "config.h"
#include <xil_printf.h>
#include <string.h>
#include "stddefs.h"
//#include "tfs.h"
//#include "tfsprivate.h"
#include "cli.h"
#include "uart.h"
//#include "xgpiops.h"
//#include "uartlite.h"


char *NmsHelp[] = {
    "NMS Modem Test",
    "-[s] [string]",
    "Options:",
    " -s   Send custom AT string (default: AT\\r\\n)",
    0,
};

int Nms(int argc, char *argv[])
{
    char *cmd = "AT\r\n";
    int ch;
    TickType_t start_time;

    if (argc > 1 && strcmp(argv[1], "-s") == 0 && argc > 2) {
        cmd = argv[2];
    }

    xil_printf("Sending to NMS: %s", cmd);
    uartWrites(UART_TR, (u8 *)cmd, strlen(cmd));

    xil_printf("Waiting for response (2s timeout)...\r\n");
    start_time = xTaskGetTickCount();
    
    while ((xTaskGetTickCount() - start_time) < (2000 / portTICK_PERIOD_MS)) {
        ch = uartRead(UART_TR);
        if (ch != -1) {
            putchar(ch);
        }
    }
    xil_printf("\r\n--- NMS Test Done ---\r\n");
    return 0;
}

char *AtHelp[] = {
    "ATC Modem Test",
    "-[s] [string]",
    "Options:",
    " -s   Send custom AT string (default: AT\\r\\n)",
    0,
};

int At(int argc, char *argv[])
{
    char *cmd = "AT\r\n";
    int ch;
    TickType_t start_time;

    if (argc > 1 && strcmp(argv[1], "-s") == 0 && argc > 2) {
        cmd = argv[2];
    }

    xil_printf("Sending to ATC: %s", cmd);
    uartWrites(UART_IR_BR1, (u8 *)cmd, strlen(cmd));

    xil_printf("Waiting for response (2s timeout)...\r\n");
    start_time = xTaskGetTickCount();
    
    while ((xTaskGetTickCount() - start_time) < (2000 / portTICK_PERIOD_MS)) {
        ch = uartRead(UART_IR_BR1);
        if (ch != -1) {
            putchar(ch);
        }
    }
    xil_printf("\r\n--- ATC Test Done ---\r\n");
    return 0;
}

// Easter Eggs
//char *WhatHelp[] = {
//		"This won't help mann",
//        "is this, IDK",
//        "Options:",
//        " -c   I haven't code this function yet",
//        0,
//};
//
//int
//What(int argc,char*argv[])
//{
//	printf("I don't know what i'm doing \n\r");
//	return(1);
//}


// Read/write Register of FPGA
//char *RegHelp[] = {
//		"Read/Write the Register in FPGA",
//        "-[r/w] [Address] [Value]",
//        "Options:",
//        " -r   Read the Register",
//		" -w   Write the Register",
//        0,
//};

//int
//Reg(int argc, char *argv[])
//{
////    char   	*opt;
//    char 	*addr;
//    char 	*value;
//    u8 		value_int;
//    u8 		addr_int;
//
////    u8 		SendBuffer[TEST_BUFFER_SIZE];
//
//    addr 	= argv[optind];
//    value	= argv[optind+1];
//
//    XStatus Status;
////    unsigned int SentCount;
////    unsigned int ReceivedCount;
////    unsigned int SendLength;
////    XUartLite UartLite;
////    static u8 RecvBuffer[TEST_BUFFER_SIZE];
////
////    Status = XUartLite_Initialize(&UartLite, XPAR_UARTLITE_0_DEVICE_ID);
////    if (Status != XST_SUCCESS)
////    {
////        return XST_FAILURE;
////    }
//
////	if(strcmp(addr, "send") == 0){
////		 SendLength = (unsigned int) strlen(value);
//////		 SentCount = XUartLite_Send(&UartLite, (u8 *) 0x5a, SendLength);
////		 SentCount = XUartLite_Send(&UartLite, (u8 *) value, SendLength);
//////		 SentCount += XUartLite_Send(&UartLite, (u8 *) 0x33, SendLength);
////		 if (SentCount != SendLength) {
////			 printf("Send fail: %d\n\r",SentCount);
//////			 return XST_FAILURE;
////		 }
////		 printf("Send message: ");
////		 for(int i=0;i<SendLength;i++){
////			printf("%c", value[i]);
////		 }
////		 printf("\n\r");
////	}
////	else if (strcmp(addr, "recv") == 0){
//////		value_int 	= (u8) strtol(value,NULL,10);
////		ReceivedCount = XUartLite_Recv(&UartLite, RecvBuffer, TEST_BUFFER_SIZE);
////		if (ReceivedCount != TEST_BUFFER_SIZE)
////		{
////			printf("Receive fail: %d\n\r",ReceivedCount);
//////			return XST_FAILURE;
////		}
////		printf("Receive message: ");
////		for(int i=0;i<TEST_BUFFER_SIZE;i++){
////			printf("%c", RecvBuffer[i]);
////		}
////		printf("\n\r");
////	}
////	else {
////		XGpioPs PS_GPIO;	/* The driver instance for GPIO Device. */
////		XGpioPs_Config *ConfigPtr;
////		ConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
////		XGpioPs_CfgInitialize(&PS_GPIO, ConfigPtr, ConfigPtr->BaseAddr);
////
////		for(int i=0;i<32;i++){
////			XGpioPs_SetDirectionPin(&PS_GPIO, i+78, 1);
////			XGpioPs_SetOutputEnablePin(&PS_GPIO, i+78, 1);
////		}
////
////		value_int 	= (u8) strtol(value,NULL,10);
////		addr_int 	= (u8) strtol(addr,NULL,10);
////		XGpioPs_WritePin(&PS_GPIO, 78+addr_int, value_int && 0x1);
////		printf("Write EMIO addr: %d, value: %d\n\r",addr_int,value_int);
////	}
//
////	if(strcmp(addr, "uart") == 0){
////		UartLitePolledExample(XPAR_UARTLITE_0_DEVICE_ID, value);
////	}
////	else if (strcmp(addr, "uart1") == 0){
////		UartLitePolledExample(XPAR_UARTLITE_1_DEVICE_ID, value);
////	}
////	else{
////	    XGpioPs PS_GPIO;	/* The driver instance for GPIO Device. */
////		XGpioPs_Config *ConfigPtr;
////		ConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
////		XGpioPs_CfgInitialize(&PS_GPIO, ConfigPtr, ConfigPtr->BaseAddr);
////
////		for(int i=0;i<32;i++){
////			XGpioPs_SetDirectionPin(&PS_GPIO, i+78, 1);
////			XGpioPs_SetOutputEnablePin(&PS_GPIO, i+78, 1);
////		}
////
////		value_int 	= (u8) strtol(value,NULL,10);
////		addr_int 	= (u8) strtol(addr,NULL,10);
////		XGpioPs_WritePin(&PS_GPIO, 78+addr_int, value_int && 0x1);
////		printf("Write EMIO addr: %d, value: %d\n\r",addr_int,value_int);
////	}
//
////	if(strcmp(addr, "clc") == 0){
////		printf("\033[2J");
////		printf("\033[H");
////	}
////    while((opt=getopt(argc,argv,":wr:")) != -1) {
////        switch(opt) {
////        case 'w':
////        	printf("Not Development Yet !!!\n\r");
////            break;
////        case 'r':
////        	if(strcmp(optarg,"hard_code") == 0){
////				printf("Selected hard_code mode\n\r");
////				Xil_Out32(0xA0070000,1);				// DAC input select: 0: Loopback, 1: CPRI
////				Xil_Out32(0xA0060000,1);				// DUC input select: 0: CPRI, 1: Hard code
////			}
////			else if(strcmp(optarg,"loopback") == 0){
////				printf("Selected loopback mode\n\r");
////				Xil_Out32(0xA0070000,0);				// DAC input select: 0: Loopback, 1: CPRI
////				Xil_Out32(0xA0060000,0);				// DUC input select: 0: CPRI, 1: Hard code
////			}
////			else if(strcmp(optarg,"cpri") == 0){
////				printf("Selected CPRI mode\n\r");
////				Xil_Out32(0xA0070000,1);				// DAC input select: 0: Loopback, 1: CPRI
////				Xil_Out32(0xA0060000,0);				// DUC input select: 0: CPRI, 1: Hard code
////			}
////			else{
////				printf("Please select mode: hard_code, loopback or cpri\n\r");
////			}
////
//////        	address = (u32) strtol(optarg,NULL,16);
//////        	printf("Optarg %s\n", optarg);
////        	break;
////        case ':':
////            printf("Option needs a value\n");
////			break;
////        case '?':
////            printf("Unknown option\n");
////            break;
////        default:
////            return 0;
////        }
////    }
//
//    return 1;
//
//}



