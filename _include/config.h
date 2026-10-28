/* config.h 
 * Configuration file. 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define INCLUDE_FLASH 1

#include "repeater_id.h"
//extern void pthread_free(void);

#define GOLDEN_IMAGE		0

#define CHT_ICS_DUO			1
#define SKT_RF_DUO			0

#define DEBUG_PRINT			1

/* Specify CPU/PLATFORM type and name so that common code can be used
 *  for a similar cpu, on different platforms.
 *  The 'TYPE' definition is used for ifdefs in the code and the 'NAME'
 *  is used for printfs in the code. 
 */

#define CPU_NAME        		"ZYNQ-7000 @XC7Z020(ARM_Cortex_A9)"
#define PLATFORM_NAME   		"AD3T MU"
#define VERSION_NAME			"AD3T_MU_V1.0"
#define CPU_LE  				1

#define RPT_MAKER 				"TJ"
#define RPT_TYPE 				"AD3T"

#define MAX_BAND				4

#define BAND0					0
#define BAND1					1
#define BAND2					2
#define BAND3					3

#define MAX_SB					4
#define SB0						0
#define SB1						1
#define SB2						2
#define SB3						3


#define MAX_PATH                2
#define DL						0
#define UL						1

#define MAX_FA					4      
#define FA1				        0
#define FA2				        1
#define FA3				        2
#define FA4				        3

#define __DEBUG							0		/* */  

#define HW_TEMP_LM74   						0

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#define INCLUDE_ICS         				1			/* ICS Function 						*/
#define INCLUDE_LMT    						0			/* Local GUI 							*/
#define INCLUDE_NMS    						0			/* Modem 								*/
#define INCLUDE_SNMP    					0
#define INCLUDE_HTTP    					0			/* Web GUI 								*/
#define INCLUDE_ENET    					0			/* Local TCP GUI 						*/
#define INCLUDE_APP    						0			/* USB Device I/F For Smart App(Android)*/
#define INCLUDE_FAT32    					0			/* USB Device I/F for USB Stick Memory 	*/
#define INCLUDE_REPEATER    				1
#define BEAGLE_BONE_USE    				    1
#define INCLUDE_REPEATER_LOG   				0
#define INCLUDE_REPEATER_SDN   				0
#define INCLUDE_REPEATER_ALC   				1
#define INCLUDE_REPEATER_ILC   				1
#define INCLUDE_HW_ADC    					0
#define INCLUDE_HW_TEMP    					1
#define INCLUDE_HW_ATT						1
#define INCLUDE_HW_PLL						1
#define INCLUDE_HW_HPA						1
#define INCLUDE_HW_LED    					0
#define INCLUDE_HW_ANT						0	

#define INCLUDE_DAS_MU						1
#define	INCLUDE_MU_TASK						1
#define INCLUDE_POSIX_MSG_QUE               1
#define INCLUDE_MU_REPEATER                 1
#define	INCLUDE_IR_TASK						1
#define INCLUDE_FPGA_TEST					0
#define INCLUDE_LMK_TEST					0
#define INCLUDE_ADC_TEST					0
#define INCLUDE_DAC_TEST					0

#define CONFIG_DAS_SDCARD_FILE				0
#define CONFIG_DAS_EMMC_FILE				1



/* PSR, PS-DAS, Commercial DAS���� ����ϴ� ����̽�
 * CLK : LMK04806 (TI)
 * ADC : ADS54J69 (TI)
 * DAC : DAC38J84 (TI)
 */
#define CONFIG_DAS_HW_INIT_LMK04828			0

#define CONFIG_DAS_HW_INIT_ADC32J45			0
#define CONFIG_DAS_HW_INIT_ADS54J69			1

#define CONFIG_DAS_HW_INIT_DAC37J84			0
#define CONFIG_DAS_HW_INIT_DAC38J84			1

#define CONFIG_DAS_HW_INIT_GPIO				1
#define CONFIG_DAS_HW_INIT_FPGA				1
#define CONFIG_DAS_MEMORY_TEST_DRIVING		1

#define CONFIG_DAS_POLLDEV					1
#define CONFIG_TEMP_MSGBOX					1

#define MU_NCC_TRANSFER_TEST                0
#define INCLUDE_JSON_WEB                    1

#define INCLUDE_LINUX_TMP         			0			/* ������ ���� �� ������ ��� ���� 						*/


#define INCLUDE_SANITY_TEST						1			/* Sanity Test					*/
#define APP_KERNEL_DRIVING						0 			/* Application Kernel Code Driving 		*/
#define TEMP_SENSOR_DRIVING						0			/* Temperature Sensor Driving 	*/
#define MEMORY_TEST_DRIVING						1			/* Physical memory test Driving 	*/
#define PSR_HW_INIT_LMK04806					0			/* PSR HW Init.(PLL, ADC, DAC)      */
#define PSR_HW_INIT_AD9652						0			/* PSR HW Init.(PLL, ADC, DAC)      */
#define PSR_HW_INIT_DAC3484						1			/* PSR HW Init.(PLL, ADC, DAC)      */
#define PSR_HW_INIT_ICS							0			/* PSR HW Init.(PLL, ADC, DAC)      */
#define PSR_HW_INIT_RTC							0
#define	CONFIG_PSR_LTE							0
#define PSR_HW_INIT_GPIO						0
#define PSR_HW_MODEM_UART						0
#define PSR_HW_INIT_MODEM						0
#define PSR_EEPROM_TEST							0

//#define INCLUDE_LMT    						1			/* Local GUI 							*/
//#define INCLUDE_REPEATER    					1
//#define INCLUDE_REPEATER_ILC					1
//#define INCLUDE_REPEATER_ALC   				0
//#define INCLUDE_REPEATER_ALARM				1
//#define INCLUDE_REPEATER_ATT					0

#define INCLUDE_REPEATER_API   					0

#define INCLUDE_SNMP    						0
#define INCLUDE_HTTP    						0			/* Web GUI 								*/
#define INCLUDE_ENET    						0			/* Local TCP GUI 						*/
#define INCLUDE_APP    							0			/* USB Device I/F For Smart App(Android)*/
#define INCLUDE_FAT32    						0			/* USB Device I/F for USB Stick Memory 	*/
#define INCLUDE_BLUETOOTH    					0			/* Bluetooth Communication				*/
#define PLATFORM_TRIO_D							0			/* Trio_dyne Platform					*/
#define PLATFORM_MOT_D							0			/* Mot_Dual Platform					*/
#define INCLUDE_HW								0			/* HardWare Interface					*/
//#define INCLUDE_HW_TEMP						0


#define INCLUDE_SPIF							1


#define Off					1
#define On 					0

#define _RESET				0
#define _SET 				1

#define _Off				0
#define _On 				1
#define _LED_MODE_TOGGLE		2

#define _Alarm				1

#define _Normal				0
#define _Sleep				1

#define _USE				0x01
#define _NOTUSE				0x00

#define _link_fail			0
#define _Connected			1
///0 : link_fail, 1 : Connected

#define _Enable 			0
#define _Disable 			1

#define _Install			1
#define _Uninstall			0

#define _Manual 			0
#define _Auto 				1

#define _Normal 			0
#define _Bypass 			1

#define _NR_PLL 			0
#define _ALC_Mode 			0
#define _NR_ALC 			1

#define _Att_tbl 			0

#define _ACK				0
#define _NACK 				1

#define _Orb_Offset			1100

//#define FALSE 0
//#define TRUE 1


#define _OnOff_Reversal		1

#define _sb_max				1
#define _Modem_init				0
#define _SingTel_Type			0

#define _5MHz				0
#define _10MHz				1
#define _15MHz				2
#define _20MHz				3


#define LED_Normal 			0
#define LED_Alarm			1
#define LED_Off				2
#define LED_All				3

#define _Semaphore			1


#endif

