//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"
#include "ics.h"

/* DL ADC �Է� ������ 22dBm 
 * DL ADC Max  = -10dBFs
 * DL ADC ���� = -36dBFs
 
 * UL ADC �Է� ������ 25dBm 
 * UL ADC Max  = -10dBFs
 * UL ADC ���� = -35dBFs  
 */
#define	SYS_AGC_RANGE		 2000
#define	DL_IN_TARGET		-2000		/* ADC ��ȭ ���� */
#define	UL_IN_TARGET		-2000		/* ADC ��ȭ ���� */

#define	DL_AGC_OFFSET		  500		/* DL IN-AGC OFFSET */
#define	UL_AGC_OFFSET		  500		/* UL IN-AGC OFFSET */



static void runIcsDlInAgc(struct repeater_struct *prpt)
{
	
   
}

static void runIcsUlInAgc(struct repeater_struct *prpt)
{
	
} 

void runSysInAgc(struct repeater_struct *prpt) 
{
	runIcsDlInAgc(prpt);
	runIcsUlInAgc(prpt);    
}
