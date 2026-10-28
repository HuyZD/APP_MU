//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"
#include "ics.h"
#include "protocol_private.h"

//#define	SYS_ALC_RANGE	3500
#define	SYS_ALC_RANGE	3000
#define	DSP_ALC_RANGE	1500

short sys_dl_alc_over_attn[MAX_BAND]={0,0};
short sys_ul_alc_over_attn[MAX_BAND]={0,0};

//////////////////////////////////////// LPR FPGA Path //////////////////////////////////////////////////////
//
//	 |----------|    |----------|    |----------|    |----------|    |----------|    |----------|
//   | DDC pwr  | => | LMS pwr  | => | ATT Gain | => | ATT Pwr  | => | ALC Gain | => | USR Gain |   		
//   |----------|    |----------|    |----------|    |----------|    |----------|    |----------|
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/* System ALC
	The System ALC acquires the peak power at the ATT PWR position and controls the ATT Gain within FPGA and RF DNC Atten.
*/
static void runSysDLAlc(struct repeater_struct *prpt)
{

}

static void runSysULAlc(struct repeater_struct *prpt)
{

	return;	
}

void runSysAlc(struct repeater_struct *prpt)
{
	int 				band;

	runSysDLAlc(prpt);
	runSysULAlc(prpt);

}
