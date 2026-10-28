#include "FreeRTOS.h"
#include <semphr.h>
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "xparameters.h"
#include "FreeRTOSConfig.h"
#include "config.h"
#include "main.h"
#include "uart_queue.h"
#include "uart.h"
#include "console.h"
#include "timers.h"
#include "task_config.h"
#include "printf.h"
#include "xuartlite_l.h"
#include "xil_cache.h"
#include "monlib.h"
#include <semphr.h>


#include "xscugic.h"
#include "xqspips.h"

#define WDT_RESTART_REG    0xF8005008
#define WDT_RESTART_KEY    0x00001999



static XScuGic pst_IntC;

// Address of moncomptr, refer to memory map file of umon
#define MONCOMPTR_ADDR			0x00000000800002ec

static xSemaphoreHandle mon_mutex; // = xSemaphoreCreateMutex();

static void monLock()
{
   xSemaphoreTake(mon_mutex, portMAX_DELAY);
}

static void monUnlock()
{
	xSemaphoreGive(mon_mutex);
}

void init2(void)
{
//#if INCLUDE_FLASH
//    FlashInit();        /* Init flashop data structures.  This MUST be */
//                        /* done prior to turning on cache!!! */
//#endif
////    cacheInit();        /* Initialize cache. */
////    delayInit();
//
//#if INCLUDE_ETHERNET
////    	enreset();          /* Clear the ethernet interface. */
//#endif
//    /* ??? */
//#if INCLUDE_TFS
//    tfsstartup();       /* Init tfs data structures. */
//#endif
//
//#if INCLUDE_ETHERNET
////    EthernetStartup(0,1);
//#endif

	/* Initialize all App data structures and arrays..  */
	{
		int offset=0;
		//extern struct monCommand cmdlist[];
	    //extern char cmdUlvl[];
		extern char ApplicationInfo[];

		//for (j=0;cmdlist[j].name;j++) {
		//	cmdUlvl[j] = (char)0;
		//}
		memset(ApplicationInfo, 0, 256);
		offset += sprintf(&ApplicationInfo[0],"Monitor Ver 2.0-%s @ %s\n",__DATE__,__TIME__);
		offset += sprintf(&ApplicationInfo[offset],"-----------------------------------------------\n");
		offset += sprintf(&ApplicationInfo[offset],"- TJ Innovation\n");
		offset += sprintf(&ApplicationInfo[offset],"- (RF&DSP Solution)\n");
		offset += sprintf(&ApplicationInfo[offset],"- #201/1412,Chunui Techno Park II,202 Chunui-Dong\n");
		offset += sprintf(&ApplicationInfo[offset],"- Wonmi-gu,Bucheon-City,Gyeonggi-do,Korea\n");
		offset += sprintf(&ApplicationInfo[offset],"- T:82+32-623-0648\n");
		offset += sprintf(&ApplicationInfo[offset],"- F:82+32-623-0649\n");
		offset += sprintf(&ApplicationInfo[offset],"- gjjang@tj-innovation.com\n");
		offset += sprintf(&ApplicationInfo[offset],"-----------------------------------------------\n");
		offset += sprintf(&ApplicationInfo[offset],"\n");
	}
}

static void vAppTaskStart();

int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	xTaskCreate(vAppTaskStart, "Main Task", APP_TASK_START_STK_SIZE, NULL, APP_TASK_START_PRIO, NULL);

	vTaskStartScheduler();
	for( ;; );

	return 1;
}

static void vAppTaskStart ()
{
	int Status;

	Status = init_UART_CLI();
	if (Status != XST_SUCCESS) {
		xil_printf("CLI Uart Init failed!!\r\n");
	}

	Status = init_Uartlite(UART_TR);	
	if (Status != XST_SUCCESS)  {
		xil_printf("UART0 initialization failed!!\r\n");
	}

	Status = init_Uartlite(UART_IR_BR1);
	if (Status != XST_SUCCESS)  {
		xil_printf("UART1 initialization failed!!\r\n");
	}

	Status = init_Uartlite(UART_IR_BR2);
	if (Status != XST_SUCCESS)  {
		xil_printf("UART2 initialization failed!!\r\n");
	}

	Status = init_Uartlite(UART_IR_BR3);
	if (Status != XST_SUCCESS)  {
		xil_printf("UART3 initialization failed!!\r\n");
	}

	Status = init_Uartlite(UART_IR_BR4);
	if (Status != XST_SUCCESS)  {
		xil_printf("UART4 initialization failed!!\r\n");
	}

	Xil_ExceptionEnable();

	init2();
	init3();

	init_console();
	init_flash();

	init_psa();

	i2c_init();
	gpio_init();
	LED_GPIO_Init();

	init_repeater();
	init_Ir_server();
	init_Ir_client();
	init_Tr_server();

 	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS (500);

	while (1) {

		xLastWakeTime = xTaskGetTickCount();

		vTaskDelayUntil(&xLastWakeTime, xPeriod);
		//printf("on[%d]\n",xPeriod);
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
		//printf("off[%d]\n",xPeriod);

		/* Feed the Watchdog Timer */
		*((volatile unsigned int *)WDT_RESTART_REG) = WDT_RESTART_KEY;
	}
}
