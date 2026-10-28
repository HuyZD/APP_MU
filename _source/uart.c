#include <FreeRTOS.h>
#include <semphr.h>
#include "FreeRTOSConfig.h"
#include "portmacro.h"

#include "xparameters.h"
#include "xuartlite.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xstatus.h"

#include "uart.h"
#include "console.h"
#include "xuartlite.h"
#include "xuartlite_l.h"
#include "xuartps.h"
#include "ring_buffer.h"
#include "stddefs.h"

#include "device.h"

#include "xscugic.h"
extern XScuGic xInterruptController;

typedef struct {
	unsigned int      	tx_busy;
	ring_buffer_t 		tx;
	xSemaphoreHandle    tx_sem;
	ring_buffer_t 		rx;
	xSemaphoreHandle    rx_sem;
} serial_port_t;

// UART_DEBUG: CLI/DEBUG
#define UART_DEBUG_DEVICE_ID    XPAR_PS7_UART_0_DEVICE_ID
// UART devices 1-5
#define UART0_DEVICE_ID    		XPAR_AXI_UARTLITE_0_DEVICE_ID   // SAU
#define UART1_DEVICE_ID    		XPAR_AXI_UARTLITE_1_DEVICE_ID   // RU BR1
#define UART2_DEVICE_ID    		XPAR_AXI_UARTLITE_2_DEVICE_ID   // RU BR2
#define UART3_DEVICE_ID    		XPAR_AXI_UARTLITE_3_DEVICE_ID   // RU BR3
#define UART4_DEVICE_ID    		XPAR_AXI_UARTLITE_4_DEVICE_ID   // RU BR4

// Interrupt IDs for the UART devices
#define UART_DEBUG_INT_IQR_ID   XPAR_XUARTPS_0_INTR
#define UART0_INT_IQR_ID   		XPAR_FABRIC_UARTLITE_0_VEC_ID
#define UART1_INT_IQR_ID   		XPAR_FABRIC_UARTLITE_1_VEC_ID
#define UART2_INT_IQR_ID   		XPAR_FABRIC_UARTLITE_2_VEC_ID
#define UART3_INT_IQR_ID   		XPAR_FABRIC_UARTLITE_3_VEC_ID
#define UART4_INT_IQR_ID   		XPAR_FABRIC_UARTLITE_4_VEC_ID

// Base addresses for the UART devices
#define UART_DEBUG_BASEADDR 	XPS_UART0_BASEADDR
#define UART0_BASEADDR   		XPAR_AXI_UARTLITE_0_BASEADDR
#define UART1_BASEADDR   		XPAR_AXI_UARTLITE_1_BASEADDR
#define UART2_BASEADDR   		XPAR_AXI_UARTLITE_2_BASEADDR
#define UART3_BASEADDR   		XPAR_AXI_UARTLITE_3_BASEADDR
#define UART4_BASEADDR   		XPAR_AXI_UARTLITE_4_BASEADDR

volatile uint8_t			Uart_Tx_Done[MAX_UART_TYPE];
volatile unsigned int		Uart_send_len[MAX_UART_TYPE];

XUartPs CLI_PsUart;            /* The instance of the PS UART Device for CLI */
static long xHigherPriorityTaskWoken;

dev_type_name   dev_map_array[MAX_UART_TYPE] = { "CLI", "TR_SAU", "IR_BR1", "IR_BR2", "IR_BR3", "IR_BR4"};
serial_port_t 	uart_ports[MAX_UART_TYPE];
XUartLite 		uartLites[MAX_UART_TYPE];
unsigned char   uart_rxdata[MAX_UART_TYPE][BUF_SIZE];

static const u16 uart_devices[MAX_UART_TYPE]   = { 0 /*dummy*/, UART0_DEVICE_ID, UART1_DEVICE_ID,  UART2_DEVICE_ID, UART3_DEVICE_ID, UART4_DEVICE_ID};
static const u16 uart_intr[MAX_UART_TYPE]      = { 0 /*dummy*/, UART0_INT_IQR_ID, UART1_INT_IQR_ID, UART2_INT_IQR_ID, UART3_INT_IQR_ID, UART4_INT_IQR_ID };
static const u32 uart_baseaddr[MAX_UART_TYPE]  = { 0 /* dummy*/, UART0_BASEADDR, UART1_BASEADDR, UART2_BASEADDR, UART3_BASEADDR, UART4_BASEADDR };

int init_UART_CLI(void)
{
	int status;
    XUartPs_Config *p_config;

	uart_ports[UART_CLI].rx_sem = xSemaphoreCreateCounting(BUF_SIZE, 0);
	if(uart_ports[UART_CLI].rx_sem == NULL){
		xil_printf("CLI Semaphore created fail\r\n");
	}

	ring_buffer_init(BUF_SIZE, (char *)uart_rxdata[UART_CLI], &uart_ports[UART_CLI].rx);

    /* 1. Initialize the PS UART driver for CLI (UART 1) */
    p_config = XUartPs_LookupConfig(UART_DEBUG_DEVICE_ID);
    if (NULL == p_config) {
        return XST_FAILURE;
    }

    status = XUartPs_CfgInitialize(&CLI_PsUart, p_config, p_config->BaseAddress);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /* 2. Set Baudrate to 921600 (Match uMON exactly) */
    XUartPs_SetBaudRate(&CLI_PsUart, 921600);

    /* 3. Setup Interrupt System for PS UART (ID 82) */
    xPortInstallInterruptHandler(UART_DEBUG_INT_IQR_ID, (XInterruptHandler)XUartPs_InterruptHandler, &CLI_PsUart);
    vPortEnableInterrupt(UART_DEBUG_INT_IQR_ID);

    /* 4. Set the callback handler */
    XUartPs_SetHandler(&CLI_PsUart, (XUartPs_Handler)CLI_PsUartHandler, &CLI_PsUart);

    /* 5. Enable RX/Timeout Interrupts */
    XUartPs_SetInterruptMask(&CLI_PsUart, XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXEMPTY | XUARTPS_IXR_TOUT);
    XUartPs_SetFifoThreshold(&CLI_PsUart, 1);
    XUartPs_SetRecvTimeout(&CLI_PsUart, 8);

	return XST_SUCCESS;
}

void CLI_PsUartHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
    XUartPs *InstancePtr = (XUartPs *)CallBackRef;
    u8 data;

    if (Event == XUARTPS_EVENT_RECV_DATA || Event == XUARTPS_EVENT_RECV_TOUT) {
        while (XUartPs_IsReceiveData(InstancePtr->Config.BaseAddress)) {
            data = XUartPs_ReadReg(InstancePtr->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
            ring_buffer_add(&uart_ports[UART_CLI].rx, data);
            xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(uart_ports[UART_CLI].rx_sem, &xHigherPriorityTaskWoken);
        }
    }
}

int init_Uartlite(UART_TYPE uart_idx)
{
	int status;

	if (uart_idx < UART_TR)
	{
		xil_printf("UART port %d is not from type UARTLITE\r\n", uart_idx);
		return XST_FAILURE;
	}
	
	uart_ports[uart_idx].rx_sem = xSemaphoreCreateCounting(BUF_SIZE, 0);
	if (uart_ports[uart_idx].rx_sem == NULL){
		xil_printf("UARTLITE Semaphore created fail\r\n");
		return XST_FAILURE;
	}

	ring_buffer_init(BUF_SIZE, (char *)uart_rxdata[uart_idx], &uart_ports[uart_idx].rx);

	status = XUartLite_Initialize(&uartLites[uart_idx], uart_devices[uart_idx]);
	if (status != XST_SUCCESS) {
		xil_printf("XUartLite_Initialize port %d fail\r\n", uart_idx);
		return XST_FAILURE;
	}
	resetUartFifo(uart_idx);

	XUartLite_SetSendHandler(&uartLites[uart_idx], uartliteSendHandler, &uartLites[uart_idx]);
	XUartLite_SetRecvHandler(&uartLites[uart_idx], uartliteRecvHandler, &uartLites[uart_idx]);

	/* Setup the interrupt system */
	status = SetupInterruptSystem(&xInterruptController, &uartLites[uart_idx], uart_intr[uart_idx]);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XUartLite_EnableInterrupt(&uartLites[uart_idx]);

	/* Diagnostic: Send a boot message to verify TX */
	// char msg[64] = {0, };
	// sprintf(msg, "UART_LITE port %d ready\r\n", uart_idx);
	// uartWrites(uart_idx, msg, strlen(msg));

	return XST_SUCCESS;
}

void resetUartFifo(UART_TYPE uart_idx){
	XUartLite_ResetFifos(&uartLites[uart_idx]);
}

UART_TYPE getUartliteIndex(XUartLite *InstancePtr) {
    for (int i = 0; i < MAX_UART_TYPE; i++) {
        if (InstancePtr->RegBaseAddress == uartLites[i].RegBaseAddress) {
            return i;
        }
    }
    return UART_UNDEFINED;
}

void uartliteSendHandler(void *CallBackRef, unsigned int EventData)
{
    XUartLite *InstancePtr = (XUartLite *)CallBackRef;
    UART_TYPE uart_idx;

    if (InstancePtr == NULL)
        return;

    uart_idx = getUartliteIndex(InstancePtr);

    if (uart_idx == UART_UNDEFINED)
        return;

    Uart_Tx_Done[uart_idx] = 1;
    Uart_send_len[uart_idx] = EventData;
}

void uartliteRecvHandler(void *CallBackRef, unsigned int EventData)
{
	u8 StatusRegister;
	XUartLite *InstancePtr = (XUartLite *)CallBackRef;
	UART_TYPE uart_idx = getUartliteIndex(InstancePtr);
	u32 data;

	if (uart_idx == UART_UNDEFINED) {
		return;
	}

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	StatusRegister = XUartLite_GetSR(InstancePtr);

	while (StatusRegister & XUL_SR_RX_FIFO_VALID_DATA) {
		data = XUartLite_ReadReg(InstancePtr->RegBaseAddress,XUL_RX_FIFO_OFFSET);
    	if(uart_idx != -1){
			if (ring_buffer_add(&uart_ports[uart_idx].rx, (uint8_t)data) == 0) {
				xSemaphoreGiveFromISR(uart_ports[uart_idx].rx_sem, &xHigherPriorityTaskWoken);
			}
		}
    	StatusRegister = XUartLite_GetSR(InstancePtr);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int UART0_empty(void){
	if(ring_buffer_count(&uart_ports[UART_CLI].rx) == 0) return 1;
	else return 0;
}

int SetupInterruptSystem(INTC *IntcInstancePtr, XUartLite *UartLiteInstancePtr, u16 UartLiteIntrId)
{
	int status;

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs.
	 * Using xPortInstallInterruptHandler is recommended for FreeRTOS on Zynq to 
	 * ensure proper kernel integration.
	 */
	status = xPortInstallInterruptHandler(UartLiteIntrId, 
					(XInterruptHandler)XUartLite_InterruptHandler, 
					UartLiteInstancePtr);
	
	if (status != pdPASS) {
		return XST_FAILURE;
	}

	/* Set priority and trigger type as in the standard example.
	 * 0xA0 is a medium priority, 0x3 is rising edge sensitivity. */
	XScuGic_SetPriorityTriggerType(IntcInstancePtr, UartLiteIntrId, 0xA0, 0x3);

	/* Enable the interrupt for the device in the GIC */
	vPortEnableInterrupt(UartLiteIntrId);

	return XST_SUCCESS;
}

void uartWrites(UART_TYPE uart_index, u8 *buf, int len)
{
    int idx;

    if (uart_index == UART_CLI)
    {
        for (int i = 0; i < len; i++)
        {
            XUartPs_SendByte(XPAR_PS7_UART_0_BASEADDR, buf[i]);
        }
    }
    else
    {
        idx = uart_index - UART_TR;

        __setTrIrSel(idx, 1);
//        usleep(1);
        usleep(100);

        Uart_Tx_Done[uart_index] = 0;

        XUartLite_Send(&uartLites[uart_index], buf, len);

        while (!Uart_Tx_Done[uart_index])
        {
            taskYIELD();
        }

        usleep(5);
        __setTrIrSel(idx, 0);
    }
}

void uartWrite(UART_TYPE uart_index, u8 buf)
{
    uartWrites(uart_index, &buf, 1);
}

char uartRead(UART_TYPE uart_index)
{
    int ch;

	xSemaphoreTake(uart_ports[uart_index].rx_sem, portMAX_DELAY);
	taskENTER_CRITICAL();
	ch = ring_buffer_remove(&uart_ports[uart_index].rx);
	taskEXIT_CRITICAL();
	
	return (char)ch;
}

int uartReads(UART_TYPE uart_index,char *buf, int cnt)
{
    int i;

    for (i=0; i < cnt; i++) {
    	buf[i] = uartRead(uart_index);
    }

    return i;
}
