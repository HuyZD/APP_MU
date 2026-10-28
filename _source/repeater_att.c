//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"

#define SYS_ATTEN_MAX_RANGE 3000 // 30dB, The maximum control range of System Atten is generally the same as the maximum control range of ALC
#define SYS_ATTEN_DSP_RANGE 1500 // 15dB, This value is equivalent to the ALC DSP control range.

#define	MAX(att1, att2) (att1 > att2) ? att1:att2

////////////////////////////////////////  LPR FPGA Path //////////////////////////////////////////////////////
//
//	 |----------|    |----------|    |----------|    |----------|    |----------|    |----------|
//   | DDC pwr  | => | LMS pwr  | => | ATT Gain | => | ATT Pwr  | => | ALC Gain | => | USR Gain |   		
//   |----------|    |----------|    |----------|    |----------|    |----------|    |----------|
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*	System Atten controls the USR Gain within the FPGA
	USER controlled atten
	Functions controlled from the Gui Main screen,
	In the GUI, it is expressed as (System Atten) or (Set Gain)
	Set Gain is the same concept as Max Gain minus System Atten
	(ex. System_Atten = Max_Gain - Set_Gain)

	In the System Atten function.
	The atten position of the control target is usr(DSP) attem of UPC RF Atten.
	In the case of LPR,  since UPC RF Atten is not used, usr(DSP) atten is controlled.

	For LPR, the maximum control range of System Atten is 35dB.
	Do not treat 35dB as a single variable.
	Process it by dividing it into two variables.
	This is related to the operation of the System ALC.

	sys_dl_sys_att1
	sys_dl_sys_att2

	"sys_dl_sys_att1" is only assigned to the control range of ALC's dsp atten
	"sys_dl_sys_att2" control range applies the same RF DNC control range as ALC
	"sys_dl_sys_att1" + "sys_dl_sys_att2", the combined value must not exceed 35dB
*/
static void runSysDlAttn(struct repeater_struct *prpt)
{
	
}


/*	For uplink, it is affected by UL AGC On/Off(= DL/UL Balanced On/Off)
	If UL AGC is Off, the uplink operates independently of the downlink.
	If UL AGC is On, the Sys_att value set for the downlink is also applied to the uplink.
*/
static void runSysUlAttn(struct repeater_struct *prpt)
{
	

}

void runSysAttn(struct repeater_struct *prpt)
{
	runSysDlAttn(prpt);
	runSysUlAttn(prpt);
}
