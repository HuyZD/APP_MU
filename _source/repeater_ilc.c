//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"

#define	SYS_ILC_RANGE	4500
#define	DSP_ILC_RANGE	4500
 
#if INCLUDE_REPEATER_ILC

static	short		sys_dl_ilc_over_attn[2]={0,0};
static	short		sys_ul_ilc_over_attn[2]={0,0};


static void runSysDLIlc(struct repeater_struct *prpt)
{


}

static void runSysULIlc(struct repeater_struct *prpt)
{

}

void runSysilc(struct repeater_struct *prpt)
{
  
}
#endif

