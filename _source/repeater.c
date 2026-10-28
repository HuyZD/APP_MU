#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"
#include "ics.h"
#include "tbl.h"
#include "protocol_private.h"
#include <errno.h>
#include "task_config.h"
#include "task.h"
///////////////////////////
#include "mod.h"
//#include "info_structure.h"

extern int	getTempSensor();

#define	MAX(att1, att2) (att1 > att2) ? att1:att2

void milliseconds_sleep(unsigned long mSec); // 200

struct repeater_struct rpt_dev;
struct repeater_adc_struct adc_rpt_dev;
struct repeater_table_struct	table_rpt_dev;
struct repeater_table_query_struct table_query_dev;
struct repeater_table_control_struct table_control_dev;
struct repeater_ags_query_struct	ags_query_dev;

struct repeater_struct *getRptDev(void)
{
	return (&rpt_dev);
}
struct repeater_adc_struct *getAdcRptDev (void){
	return (&adc_rpt_dev);
}
struct repeater_table_struct *getTableRptDev (void){
	return (&table_rpt_dev);
}
struct repeater_table_query_struct *getTableQueryDev(void){
	return (&table_query_dev);
}
struct repeater_table_control_struct *getTableControlDev(void){
	return (&table_control_dev);
}
struct repeater_ags_query_struct *getAgsQueryDev(void){
	return (&ags_query_dev);
}

static void run_rpt(struct repeater_struct *pdev)
{
	get_temp(0, &pdev->temperature);
	pdev->temperature = (pdev->temperature >> 8); // only the first higher byte
	pdev->temperature = pdev->temperature*100;
//	pdev->temperature += pdev->temp_offset/100;
}


/* setIcsDspAttn()
 * DL/UL LTE Dsp Att/Usr Att
 * DSp Att :att gain, dsp alc attn, ssaw attn, in agc attn 
 * USR ATT: sys attn1, sys attn2, temp reward
 */
static void setIcsDspAttn(struct repeater_struct *prpt)
{	

}


/* setSysAttn()
 * DL/UL DNC Atten ?????
 * DNC Atten : dnc alc atten, dnc attn, dnc attn offset , in agc attn
 */
static void setSysAttn(struct repeater_struct *prpt)
{
    
}
typedef enum {
    LED_COLOR_OFF    		= 0x0, // ²¨Áü
    LED_COLOR_GREEN  		= 0x1, // ±×¸° ÄÑÁü
    LED_COLOR_RED    		= 0x2, // ·¹µå ÄÑÁü
    LED_COLOR_YELLOW 		= 0x3,  // µÑ ´Ù ÄÑÁü (³ë¶õ»ö/ÁÖÈ²»ö µî)
	LED_COLOR_GREEN_BLINK  	= 0x4  // ±×¸° Á¡¸ê ¸ðµå Ãß°¡    
} LED_Color_t;


extern void runLedAlarm(void);
extern void LED_Mode(uchar onoff);

static void vRepeaterTask(void *p_arg)
{
	struct ics_struct 	*idev;
	struct repeater_struct *prpt;
	prpt = getRptDev();

	char 				staus;
	int 				timeout = 0;
	char 				timemax;
	static char			init_led = 1;
	unsigned int		led_tog = 0;
	static int 			init_Led = 1;
	static char 		bootcnt = 0;

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(100);

	timemax = 10;	///// 1??

	xLastWakeTime = xTaskGetTickCount();
	for(;;) {

		/* It must be set to 100ms. */
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
//		printf("0 init_Led\n");

		if (init_Led) 
		{
			if (!(timeout % 5)) 
			{	//1s
				if((bootcnt++ < 20)||(prpt->boot_initilization == 0))
				{
					LED_Mode(_LED_MODE_TOGGLE);
				}
				else
				{
					init_Led = 0;
					LED_Mode(_On);

					printf("1 init_Led[%d]\n",prpt->boot_initilization);
				}
			 }	
		}


        /* ---------------------------------------------------------------------*/
        /*		ICS System 											 			*/
        /* ---------------------------------------------------------------------*/
		run_ics();
		run_rpt(&rpt_dev);

		/* ----------------------------------------------------------------------
			* Run Algorithm
			*	1.  runSysAttn
			*	2.  runSysAlc
			*	3.  runSysInAgc
			*	4.  setIcsDspAttn
			*	5.  setLteDspAttn
			*	6. 	setSysAttn
			*	7.  runShutDown
			*	8.  OverTempShutdown
			*	9.  UnLockPllShutdown
			* ----------------------------------------------------------------------*/
		/* ----------------------------------------------------------------------
		*	ILC
		* ----------------------------------------------------------------------*/
//		runSysilc(&rpt_dev);
		/* ----------------------------------------------------------------------
		*	SYS-ATT
		* ----------------------------------------------------------------------*/
//		runSysAttn(&rpt_dev);
		/* ----------------------------------------------------------------------
		*	ALC
		* ----------------------------------------------------------------------*/
//		runSysAlc(&rpt_dev);

		/* ----------------------------------------------------------------------
		*	control ATTEN
		* ----------------------------------------------------------------------*/
//		setIcsDspAttn(&rpt_dev);
//		setSysAttn(&rpt_dev);

		/* ----------------------------------------------------------------------
		*	Alarm
		* ----------------------------------------------------------------------*/

		run_alarm_rpt(&rpt_dev);

		/* ----------------------------------------------------------------------
		*																		*
		* ----------------------------------------------------------------------*/
		/* Shut Down */
		if (!(timeout % timemax)) {
//			runShutDown(&rpt_dev);
		}

		/* ----------------------------------------------------------------------
		 *	LED timeout 5 1.0Sec
		 * ----------------------------------------------------------------------*/
		if (!(timeout % 10))  {

//		printf("1 temperature[%d]\n",prpt->temperature);

			if( init_Led == 0)
				runLedAlarm();
		}	

        timeout++;
        //continue;
	}
}

xTaskHandle xRepeaterHandle;

void init_repeater(void)
{
	char 				msg[] = "repeater thread";
	unsigned char			pl_mode,app_mode;

	struct repeater_struct *prpt;
	prpt = getRptDev();

	loadRepeaterInfo(&rpt_dev);
	
	getBootInfo((char *)&pl_mode, (char *)&app_mode);
	printf("pl_mode[%d],app_mode[%d]\n",pl_mode, app_mode);
	prpt->pl_mode = pl_mode;
	prpt->app_mode = app_mode;
	
#if 1
/// Branch Power Init
	BrPowerSet(0, 1); ///1 On
	BrPowerSet(1, 1);
	BrPowerSet(2, 1);
	BrPowerSet(3, 1);
#endif	

#if 1
  setPwmDac(0, (uint16_t)rpt_dev.tr_pwm);
#if 1
	rpt_dev.tr_pwm = 0x07c1;
	rpt_dev.ir_pwm_br[0] = 0x07c1;
	rpt_dev.ir_pwm_br[1] = 0x07c1;
	rpt_dev.ir_pwm_br[2] = 0x07c1;
	rpt_dev.ir_pwm_br[3] = 0x07c1;
#endif	


	prpt->Tree.sau_pri_mu = 1;

    for (int i = 0; i < 4; i++) {
        setPwmDac(i + 1, (uint16_t)rpt_dev.ir_pwm_br[i]);
    }

#endif

	prpt->boot_initilization = 1;

	rpt_dev.repeater_ID = AD3T_MU_Type;


	rpt_dev.lmt_bypass = 0;
	rpt_dev.firmware_version = ((FIRMWARE_MAIN_VERSION << 8) & 0xff00) |((FIRMWARE_SUB_VERSION ) & 0x00FF);
	rpt_dev.fpga_version = ((FPGA_MAIN_VERSION << 8) & 0xff00) |((FPGA_SUB_VERSION ) & 0x00FF);

	printf("[AD3T MU] Firmware Ver_[%04x]\n",rpt_dev.firmware_version );

	strcpy((char *)rpt_dev.repeater_type, RPT_TYPE);
	strcpy((char *)rpt_dev.repeater_maker, RPT_MAKER);
	memset(rpt_dev.repeater_sn, 0, sizeof(rpt_dev.repeater_sn));
	strcpy((char *)rpt_dev.repeater_sn, "AD3TMU26100001");


	/* Repeater Task Create */
	xTaskCreate(vRepeaterTask, "Repeater Task", APP_TASK_START_STK_SIZE, NULL, OS_TASK_REPEATER_PRIO, &xRepeaterHandle);
}

