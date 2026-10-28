#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include "config.h"
#include "repeater.h"
//#include "lte.h"
#include "fpga.h"

typedef enum {
	dlInputLowerAlarmCnt_700M = 0,	//0
	dlInputLowerAlarmCnt_1700M,		//1
	dlInputOverAlarmCnt_700M,		//2
	dlInputOverAlarmCnt_1700M,		//3
	dlOutputOverSDAlarmCnt_700M,	//4
	dlOutputOverSDAlarmCnt_1700M,	//5
	ulInputOverAlarmCnt_700M,		//6
	ulInputOverAlarmCnt_1700M,		//7
	ulOutputOverSDAlarmCnt_700M,	//8
	ulOutputOverSDAlarmCnt_1700M,	//9
	lowIsolationAlarmCnt_700M,		//10
	lowIsolationAlarmCnt_1700M,		//11
	isolationSDAlarmCnt_700M,		//12
	isolationSDAlarmCnt_1700M,		//13
	pllAlarmCnt_700M,				//14
	pllAlarmCnt_1700M,				//15
	temperatureAlarmCnt,			//16

} alarm_cnt;			


unsigned char getCurAlarm(unsigned char idx);
extern int	reset_flag;

extern void rpt_Ctrl(struct repeater_struct *pdev);
int			CurrAlarmCnt[20]= {0,};
unsigned short alarm_max_cnt= 0;
unsigned short debug_init = 0;

char boot_mode = 0;

static char totalAlarm;
static char AlarmLed[MAX_BAND];
extern void ShutDown_AmpOnOff(int band, int val);


void run_alarm_rpt(struct repeater_struct *pdev)
{
	
}


void enableDummyAlarms(struct repeater_struct *pdev, bool enable)
{
	
}



extern void ALMLed(unsigned char val);
extern void B3_Led(unsigned char val);
extern void B28_Led(unsigned char val);

void runLedAlarm(void) {

}
