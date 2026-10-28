/*
 * task_config.h
 *
 *  Created on: 2023. 6. 8.
 *      Author: LJH
 */

#ifndef SRC_TASK_CONFIG_H_
#define SRC_TASK_CONFIG_H_





#define  APP_TASK_START_PRIO					6u

#define  ETHERNET_DRV_PRIO						APP_HIGH_PRIORITY + 4
#define  ETHERNET_TSK_PRIO						APP_HIGH_PRIORITY + 5
#define  LWIP_TSK_PRIO							APP_HIGH_PRIORITY + 6


//configMAX_PRIORITIES 8

#define OS_TASK_GUI_PRIO                 	5
#define OS_TASK_CONSOLE_PRIO                4
#define OS_TASK_REPEATER_PRIO				3
#define	OS_TASK_TR_SERVER_PRIO				2
#define	OS_TASK_TR_CLIENT_PRIO				6
#define	OS_TASK_IR_SERVER_PRIO				7
#define	OS_TASK_IR_CLIENT_PRIO				8
//#define OS_TASK_NMS_PRIO                	10
//#define OS_TASK_SNMP_PRIO					12
//#define OS_TASK_APP_PRIO					13
//#define OS_TASK_FAT32_PRIO				14
#define  OS_TASK_MODEM_RX_PRIO				10
#define  OS_TASK_MODEM_TX_PRIO				11
#define OS_TASK_STATUS_REQ_PRIO				12



#define  APP_TASK_START_STK_SIZE			1024
#define  TASK_STK_SIZE						2048

#define OS_TASK_CONSOLE_STK_SIZE          	2048
#define	OS_TASK_DOWNLOAD_STK_SIZE			2048



#endif /* SRC_TASK_CONFIG_H_ */
