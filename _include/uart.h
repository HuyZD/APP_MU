#ifndef __UART_H__
#define __UART_H__

#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include "xuartlite.h"
#include "xscugic.h"

#define CTLC    0x03    /* control + C */
#define SYNC    0x16    /* protocol sync */

#define INTC XScuGic

#define BUF_SIZE			    2048
#define MAX_UARTLITE        5

typedef enum {
    UART_UNDEFINED = -1,
    UART_CLI       = 0,
	UART_TR        = 1,
	UART_IR_BR1    = 2,
 	UART_IR_BR2    = 3,
 	UART_IR_BR3    = 4,
 	UART_IR_BR4    = 5,
    MAX_UART_TYPE  = 6
} UART_TYPE;

extern int init_UART_CLI();

extern int Nms(int argc, char *argv[]);
extern char *NmsHelp[];
extern int At(int argc, char *argv[]);
extern char *AtHelp[];

void vConsoleTask(void *pvParameters);

/*
    Send data through the specified UART port.
    Parameters:
        uart_index: The index of the UART port to send data through.
        buf: Pointer to the buffer containing the data to send.
        len: Length of the data to send.
*/
void uartWrites(UART_TYPE uart_index, u8 *buf, int len);

/*
    Send 1 byte through the specified UART port.
    Parameters:
        uart_index: The index of the UART port to send data through.
        buf: the data to send.
*/
void uartWrite(UART_TYPE uart_index, u8 buf);

/*
    Read one character from the specified UART port. This function blocks until a character is available.
    Parameters:
        uart_index: The index of the UART port to read from.
    Returns:
        The character read from the UART port.
*/
char uartRead(UART_TYPE uart_index);

/*
    Read multiple characters from the specified UART port. This function blocks until the requested number of characters are available.
    Parameters:
        uart_index: The index of the UART port to read from.
        buf: Pointer to the buffer where the read characters will be stored.
        cnt: Number of characters to read.
    Returns:
        The number of characters actually read.
*/
int uartReads(UART_TYPE uart_index,char *buf, int cnt);

/*
    Init UARTLITE (TR, BR1-4) port
*/
int init_Uartlite(UART_TYPE idx);

/*
    Receive handler for UART PS (CLI)
*/
void CLI_PsUartHandler(void *CallBackRef, u32 Event, unsigned int EventData);

/*  
    Reset receiver FIFO for the UART port
*/
void resetUartFifo(UART_TYPE uart_index);

/*
*/
int SetupInterruptSystem(INTC *IntcInstancePtr, XUartLite *UartLiteInstancePtr, u16 UartLiteIntrId);

/*
    Receive handler function for UARTLITE
*/
void uartliteRecvHandler(void *CallBackRef, unsigned int EventData);
void uartliteSendHandler(void *CallBackRef, unsigned int EventData);

#endif
