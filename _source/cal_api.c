/*	cal_api.c
 *	Repeater API
 */
//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "ics_private.h"
#include "psa.h"
#include "cal.h"

#define	PRE_UP	0x50000000
#define	PRE_LD	0x30000000
#define	PRE_WE	0x10000000

struct tbl_cal_info_struct		cinfo;

#if 1

void setCalOnOff(struct cal_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
	ioaddr = pdev->cal_reg_base[path] + CAL_CONFIG0;
	ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~ICS_ON) : (ctrl | ICS_ON);
	
	FPGA_outl(ioaddr,ctrl);
}
void setCalStepNow(struct cal_struct *pdev, int path, int set)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->cal_reg_base[path] + CAL_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x00ff0000;
    iodata |= (set << 16);
    FPGA_outl(ioaddr, iodata);
}          
int getCalStepDef(struct cal_struct *pdev, int path)					/* 실험에 의해 결정된 LMS_STEP 디폴트 값을 일거온다. ISO 확보시 Step 값 		*/
{          																/* ISO 확보시에는 Step 값을 키워서 Coef를 거의 0에 가깝게 바꿔놔야 EVM이 좋다.	*/
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->cal_reg_base[path] + CAL_LMS_STEP;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xff;
    return iodata;
}          
int getCalStepRun(struct cal_struct *pdev, int path)					/* 실험에 의해 결정된 LMS_STEP RUN 값을 일거온다. G=ISO+15 Step 값 		*/
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->cal_reg_base[path] + CAL_LMS_STEP;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xff;
    return iodata;
}          
int getCalStepNow(struct cal_struct *pdev, int path)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->cal_reg_base[path] + CAL_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xff;
    return iodata;
}          

////////////////////////////////////////////////////////////////////////////////////////
////	AGC API
////////////////////////////////////////////////////////////////////////////////////////
void setCalAgcOnOff(struct cal_struct *pdev, int path, int onoff)
{
    unsigned short ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->cal_reg_base[path] + CAL_CONFIG0;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~AGC_ON) : (ctrl | AGC_ON);
    FPGA_outl(ioaddr,ctrl);
}
int getCalAgcOnOff(struct cal_struct *pdev, int path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->cal_reg_base[path] + CAL_CONFIG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & AGC_ON) ? 1:0;
}
void setCalAgcSetp(struct cal_struct *pdev, int path, int set)
{          
    double set_point;
    unsigned int set_word;
    unsigned int ioaddr;
    
    ioaddr = pdev->cal_reg_base[path] + CAL_AGC_SETP;  
    set_point = ((double)set/2000.0);
   	set_word = (unsigned int)(pow(10.0,set_point) * 32767);
       
    FPGA_outl(ioaddr, set_word*set_word);
}          
int getCalAgcSetp(struct cal_struct *pdev, int path)
{        
    double target;
    unsigned int ioaddr;
           
   	ioaddr = pdev->cal_reg_base[path] + CAL_AGC_SETP;
         
    target = (double)FPGA_inl(ioaddr);
    target = sqrt(target);
    target = target / 32767.0;
         
    target = 20*log10(target);
    return (int)(target * 100);
}        
//////////////////////////////////////////////////////////////////////////////////////
//	Power Meter API
//////////////////////////////////////////////////////////////////////////////////////
int getCalDdcPwr(struct cal_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr;
    
    ioaddr = pdev->cal_pwr_base[path] + CAL_DDC_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getCalLmsPwr(struct cal_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr;
    
    ioaddr = pdev->cal_pwr_base[path] + CAL_LMS_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getCalManPwr(struct cal_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
    ioaddr = pdev->cal_pwr_base[path] + CAL_MAN_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);

    return (pwr);
}
int getCalRefPwr(struct cal_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
    ioaddr = pdev->cal_pwr_base[path] + CAL_REF_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);

    return (pwr);
}
//////////////////////////////////////////////////////////////////////////////////////
//	Delay1,2 API
//////////////////////////////////////////////////////////////////////////////////////
void setCalDelay1(struct cal_struct *pdev, int delay)
{
    unsigned int ioaddr, iodata;
    
   	ioaddr = pdev->cal_reg_base[DL] + CAL_DELAY1;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x00000fff;
    iodata |= (delay << 0);
    FPGA_outl(ioaddr,iodata);
}
int getCalDelay1(struct cal_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
   	ioaddr = pdev->cal_reg_base[DL] + CAL_DELAY1;
   	iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 0) & 0xfff;
    
    return (iodata);
}
void setCalDelay2(struct cal_struct *pdev, int delay)
{
    unsigned int ioaddr, iodata;
    
   	ioaddr = pdev->cal_reg_base[DL] + CAL_DELAY2;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0fff0000;
    iodata |= (delay << 16);
    FPGA_outl(ioaddr,iodata);
}

int getCalDelay2(struct cal_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
   	ioaddr = pdev->cal_reg_base[DL] + CAL_DELAY2;
   	iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xfff;
    return (iodata);
}
//////////////////////////////////////////////////////////////////////////////////////
//	Gain API
//////////////////////////////////////////////////////////////////////////////////////
/* 	getCalManGain()
 *	현재 각 FA의 DSP ALC의 이득값을 구한다. 
 *	ALC의 최대 이득은 0dB이다.
 */
int getCalManGain(struct cal_struct *pdev, int path)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
           
    ioaddr = pdev->cal_reg_base[path] + CAL_MAN_GAIN;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0)& 0xffff;
    if (!iodata) {
    	gain = 0xffff;
		gain = gain / (2047.0);		//ALC 0dB Gain
    }
    else {
    	gain = (double)iodata;
		
		gain = gain / (2047.0);		//ALC 30dB Gain
		//gain = gain / (4095.0);		//ALC 24dB Gain
		//gain = gain / (8191.0);			//ALC 18dB Gain
		//gain = gain / (16383.0);		//ALC 12dB Gain
		//gain = gain / (32767.0);		//ALC 6dB Gain
		//gain = gain / (65535.0);		//ALC 0dB Gain
    }
    
    gain = 20*log10(gain);
    	
    return (int)(gain * 100);
}
int getCalRefGain(struct cal_struct *pdev, int path)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
           
    ioaddr = pdev->cal_reg_base[path] + CAL_REF_GAIN;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16)& 0xffff;
    if (!iodata) {
    	gain = 0xffff;
		gain = gain / (2047.0);			//ALC 0dB Gain
    }
    else {
    	gain = (double)iodata;
		
		gain = gain / (2047.0);			// 30dB Gain
		//gain = gain / (4095.0);		// 24dB Gain
		//gain = gain / (8191.0);		// 18dB Gain
		//gain = gain / (16383.0);		// 12dB Gain
		//gain = gain / (32767.0);		// 6dB Gain
		//gain = gain / (65535.0);		// 0dB Gain
    }
    
    gain = 20*log10(gain);
    	
    return (int)(gain * 100);
}
int getCalMgcGain(struct cal_struct *pdev, int path)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
	ioaddr = pdev->cal_reg_base[path] + CAL_MGC_GAIN;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0)& 0xffff;

    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;
    	
    	gain = gain / (2047.0);		//AGC 30dB Gain
    	//gain = gain / (4095.0);		//AGC 24dB Gain
    	//gain = gain / (8191.0);		//AGC 18dB Gain
    	//gain = gain / (16383.0);		//AGC 12dB Gain
    	//gain = gain / (32767.0);		//AGC 6dB Gain
    	//gain = gain / (65535.0);		//AGC 0dB Gain    
	}

    gain = 20*log10(gain);

    return (int)(gain * 100);
}
int getCalRgcGain(struct cal_struct *pdev, int path)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
    ioaddr = pdev->cal_reg_base[path] + CAL_RGC_GAIN;
    
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16)& 0xffff;
    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;
    	
    	gain = gain / (2047.0);		//AGC 30dB Gain
    	//gain = gain / (4095.0);		//AGC 24dB Gain
    	//gain = gain / (8191.0);		//AGC 18dB Gain
    	//gain = gain / (16383.0);		//AGC 12dB Gain
    	//gain = gain / (32767.0);		//AGC 6dB Gain
    	//gain = gain / (65535.0);		//AGC 0dB Gain    
	}

    gain = 20*log10(gain);

    return (int)(gain * 100);
}





#endif



static void COEF_WE(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr;
    unsigned int iodata;

    ioaddr = pdev->ics_reg_base[path] + 0x0;
	iodata = FPGA_inl(ioaddr);
	iodata |= PRE_WE;
    FPGA_outl(ioaddr, iodata);
	iodata &= ~PRE_WE;
    FPGA_outl(ioaddr, iodata);
}
static void COEF_LD(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr;
    unsigned int iodata;

    ioaddr = pdev->ics_reg_base[path] + 0x0;
	iodata = FPGA_inl(ioaddr);
	iodata |= PRE_LD;
    FPGA_outl(ioaddr, iodata);
	iodata &= ~PRE_LD;
    FPGA_outl(ioaddr, iodata);
}
static void COEF_UP(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr;
    unsigned int iodata;

    ioaddr = pdev->ics_reg_base[path] + 0x0;
	iodata = FPGA_inl(ioaddr);
	iodata |= PRE_UP;
    FPGA_outl(ioaddr, iodata);
	iodata &= ~PRE_UP;
    FPGA_outl(ioaddr, iodata);
}

/* loadCalCoeDef()
 * Coef °ª?? ??????¾? ??´? Æ????? ¾ø?¸¸? ??±? °ª?¸·? Coef°ª?? ¼³?¤?Ø??¾ß ??·??¸·? ½???°¡ ??¾?¿?´?. 
 * ??±?°ª?º ?¹¹ø?° Tap¿¡ 1?? ¼³?¤??°?, ´?¸? Tap¿¡´? 0?? ¼³?¤??´?. 
 */
int loadCalCoeDef(struct ics_struct *pdev)
{
    int 			i=0,j=0,path=0;
	unsigned int 	iodata;
	int 			numb_tap;
	int 			unit_tap;
	int				offset;
	int				coef;
	
	iodata = FPGA_inl(pdev->ics_reg_base[DL] + 0x3C);  	//	B_REG7
	offset = (iodata >> 27) & 0x1f;		offset += 1;
	numb_tap = (iodata >> 21) & 0x3f;	numb_tap += 1;
	unit_tap = (iodata >> 16) & 0x1f;	unit_tap += 1;

#if 1
	printf("NUMB_TAP : %2d\n",numb_tap);
	printf("UNIT_TAP : %2d\n",unit_tap);
	printf("OFFSET   : %2d\n",offset);
#endif

    for(path=0;path<2;path++) {
    	for(i=0;i<numb_tap;i++) {
    		for(j=0;j<unit_tap;j++) {
    			if (i==0 && j==2) {
    				/* ?¹¹ø?° Coef ´? 1?? ³??½.*/
    				coef = (0 << 16) | 16363;
    				FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, coef);
    				COEF_WE(pdev,path);
    			}
    			else if (i==numb_tap-1 && j==unit_tap-1) {
    				/* ¸¶??¸· Coef ´? LD */
    				coef = (0 << 16) | 0;
    				FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, coef);
    				COEF_LD(pdev,path);
    			}
    			else if (j==unit_tap-1) {
    				/* ¸¶??¸· UNIT_TAP¿¡´? Wrap */
    				coef = (0 << 16) | 0;
    				FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, coef);
					COEF_UP(pdev,path);
    			}
    			else {
    				coef = (0 << 16) | 0;
    				FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, coef);
    				COEF_WE(pdev,path);
    			}
    		}
    	}
    }
    return (0);
}

short hi_Def[32] = {	-46,   212, 3597, -331,  190,  -37,    0,	19,  -34,	 0,    3,	-9,    7,	 0,   -2,	 1,
1119,-3882, 6917, -146,-1042, 1476,-1092,  499,    0, -272,  293, -158,    0,	87,  -94,	50};

short hq_Def[32] = { 65,	-133, -8938,   636,  -145,	   0,	 10,   -13, 	0,	  20,	-22,	 0,    10,	 -13,	  7,	 0,
-37, -1329,  9470, -3177,  3604, -2472,  1052,	   0,  -527,   598,  -424,	 180,	  0,   -69,    65,	 -29};

/// DL/UL 
static int hn[2][16][2] = {
{	
	{  -201,  -166},
	{   665,   708},
	{ 15362,  6511},
	{ -1402,  -877},
	{   459,   194},
	{  -117,   -15},
	{     0,   -67},
	{    23,    32},
	{    -4,    -9},
	{     0,     0},
	{    19,    11},
	{   -16,   -11},
	{     0,     0},
	{     1,     2},
	{     0,     0},
	{     0,     0},
},                  
{	                
	{  -800,  -412},
	{  2774,  2934},
	{ -2846,-17487},
	{ -1432,  -190},
	{  2550,  -237},
	{ -1540,     0},
	{   644,    52},
	{     0,     0},
	{  -271,   -71},
	{   285,    64},
	{  -127,   -35},
	{     0,     0},
	{    82,    32},
	{   -82,   -36},
	{    49,    13},
	{   -10,     0} 
}
};

int loadCalCoeForce(struct ics_struct *pdev, int path, short hi[], short hq[])
{
    int 			i=0,j=0;
	unsigned int 	iodata;
	int 			numb_tap;
	int 			unit_tap;
	int				offset;
	
	iodata = FPGA_inl(pdev->ics_reg_base[DL] + 0x3C);  	//	B_REG7
	offset = (iodata >> 27) & 0x1f;		offset += 1;
	numb_tap = (iodata >> 21) & 0x3f;	numb_tap += 1;
	unit_tap = (iodata >> 16) & 0x1f;	unit_tap += 1;

#if 1
	printf("NUMB_TAP : %2d\n",numb_tap);
	printf("UNIT_TAP : %2d\n",unit_tap);
	printf("OFFSET   : %2d\n",offset);
#endif

	if (hi == 0 && hq == 0) {
    	for(i=0;i<numb_tap;i++) {
    		for(j=0;j<unit_tap;j++) {
    			if (i==0 && j==0) {
    				/* ?¹¹ø?° Coef ´? 1?? ³??½.*/
    				iodata = 0;
    				iodata |= (hn[DL][i*unit_tap+j][1] & 0xffff) << 16; /// hi
    				iodata |= (hn[DL][i*unit_tap+j][0] & 0xffff) <<  0; /// hq
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_WE(pdev,DL);
    				iodata = 0;
    				iodata |= (hn[UL][i*unit_tap+j][1] & 0xffff) << 16;
    				iodata |= (hn[UL][i*unit_tap+j][0] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_WE(pdev,UL);
    			}
    			else if (i==numb_tap-1 && j==unit_tap-1) {
    				/* ¸¶??¸· Coef ´? LD */
    				iodata = 0;
    				iodata |= (hn[DL][i*unit_tap+j][1] & 0xffff) << 16;
    				iodata |= (hn[DL][i*unit_tap+j][0] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_LD(pdev,DL);
    				iodata = 0;
    				iodata |= (hn[UL][i*unit_tap+j][1] & 0xffff) << 16;
    				iodata |= (hn[UL][i*unit_tap+j][0] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_LD(pdev,UL);
    			}
    			else if (j==unit_tap-1) {
    				/* ¸¶??¸· UNIT_TAP¿¡´? Wrap */
    				iodata = 0;
    				iodata |= (hn[DL][i*unit_tap+j][1] & 0xffff) << 16;
    				iodata |= (hn[DL][i*unit_tap+j][0] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
					COEF_UP(pdev,DL);
    				iodata = 0;
    				iodata |= (hn[UL][i*unit_tap+j][1] & 0xffff) << 16;
    				iodata |= (hn[UL][i*unit_tap+j][0] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
					COEF_UP(pdev,UL);
    			}
    			else {
    				iodata = 0;
    				iodata |= (hn[DL][i*unit_tap+j][1] & 0xffff) << 16;
    				iodata |= (hn[DL][i*unit_tap+j][0] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_WE(pdev,DL);
    				iodata = 0;
    				iodata |= (hn[UL][i*unit_tap+j][1] & 0xffff) << 16;
    				iodata |= (hn[UL][i*unit_tap+j][0] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_WE(pdev,UL);
    			}
    			printf("%2d : %6d, %6d, %6d, %6d\n",i*unit_tap+j, hn[DL][i*unit_tap+j][0], hn[DL][i*unit_tap+j][1], hn[UL][i*unit_tap+j][0], hn[UL][i*unit_tap+j][1]);
    		}
    	}
    	printf("\n");
    } 
    else {
    	for(i=0;i<numb_tap;i++) {
    		for(j=0;j<unit_tap;j++) {
    			if (i==0 && j==0) {
    				iodata = 0;
    				iodata |= (hq[i*unit_tap+j] & 0xffff) << 16;
    				iodata |= (hi[i*unit_tap+j] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_WE(pdev,UL);
    			}
    			else if (i==numb_tap-1 && j==unit_tap-1) {
    				iodata = 0;
    				iodata |= (hq[i*unit_tap+j] & 0xffff) << 16;
    				iodata |= (hi[i*unit_tap+j] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_LD(pdev,UL);
    			}
    			else if (j==unit_tap-1) {
    				iodata = 0;
    				iodata |= (hq[i*unit_tap+j] & 0xffff) << 16;
    				iodata |= (hi[i*unit_tap+j] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
					COEF_UP(pdev,UL);
    			}
    			else {
    				iodata = 0;
    				iodata |= (hq[i*unit_tap+j] & 0xffff) << 16;
    				iodata |= (hi[i*unit_tap+j] & 0xffff) <<  0;
   					FPGA_outl(pdev->ics_reg_base[UL] + 0x3C, iodata);
    				COEF_WE(pdev,UL);
    			}
    			printf("tbl %2d : %6d, %6d\n",i*unit_tap+j, hi[i*unit_tap+j], hq[i*unit_tap+j]);
				
    		//	printf("tbl %2d : %6d, %6d, %6d, %6d\n",i*unit_tap+j, hn[DL][i*unit_tap+j][0], hn[DL][i*unit_tap+j][1], hn[UL][i*unit_tap+j][0], hn[UL][i*unit_tap+j][1]);
    		}
    	}
    	printf("\n");
    }
    return (0);
}

/* loadCalCoe()
 * ?Ø´? Band?? DL/UL °æ·?¿¡ Æ???·? ??????¾? ??´? coef °ª?? ¼³?¤??´?. 
 * Full Band?? °æ¿? SB´? ?×?? 0??¾?¾ß ??´?. ??¶§ NCO°ª?º ??¹?°¡ ¾ø?½. 
 */
char *fname[2] = {"Downlink","Uplink"};

int loadCalCoe(struct cal_struct *pdev)
{
//	char 				buf[32];
//	int					fd,fsize;
//	int					i,j,path;
//	unsigned int 		iodata;
//	int 				numb_tap;
//	int 				unit_tap;
//	int					offset;
//	unsigned int		hi,hq,coef;
//	struct cal_file 	coe;

//	fd = open("/data/app/cal.coe",O_RDWR|O_SYNC|O_CREAT,777);
//	if(fd > 0) {
//    	fsize=read(fd,(void *)&coe,sizeof(struct cal_file));
//		close(fd);
//		for (i=0;i<coe.size;i++) {
//			printf("%4d> %6d %6d|%6d %6d|\n",i,coe.hi[DL][i],coe.hq[DL][i],coe.hi[UL][i],coe.hq[UL][i]);
//		}
//
//		iodata = FPGA_inl(pdev->psa_reg_base + 0x54);  	//	C_REG5
//		offset = (iodata >> 10) & 0x3f;
//		numb_tap = (iodata >> 5) & 0x1f;
//		unit_tap = (iodata >> 0) & 0x1f;
//		
//    	for(path=0;path<2;path++) {
//    		for(i=0;i<numb_tap;i++) {
//    			for(j=0;j<unit_tap;j++) {
//    				if (i==numb_tap-1 && j==unit_tap-1) {
//    					/* ¸¶??¸· Coef ´? LD */
//    					hi = coe.hi[path][i*unit_tap+j];
//    					hq = coe.hq[path][i*unit_tap+j];
//						coef = (hq << 16) | hi;
//    					FPGA_outl(pdev->psa_reg_base + 0x58, coef);
//    					COEF_LD(pdev,path);
//    				}
//    				else if (j==unit_tap-1) {
//    					/* ¸¶??¸· UNIT_TAP¿¡´? Wrap */
//    					hi = coe.hi[path][i*unit_tap+j];
//    					hq = coe.hq[path][i*unit_tap+j];
//						coef = (hq << 16) | hi;
//    					FPGA_outl(pdev->psa_reg_base + 0x58, coef);
//						COEF_UP(pdev,path);
//    				}
//    				else {
//    					hi = coe.hi[path][i*unit_tap+j];
//    					hq = coe.hq[path][i*unit_tap+j];
//						coef = (hq << 16) | hi;
//    					FPGA_outl(pdev->psa_reg_base + 0x58, coef);
//    					COEF_WE(pdev,path);
//    				}
//    			}
//    		}
//    	}
//	}
//    else {
//		printf( "There is no cal.coe file, Now force coe to defaut..\n");
//    	loadCalCoeDef(pdev);
//    }
    return (0);
}

void putCoeKey(struct cal_struct *pdev)
{
	unsigned int iodata,ioaddr;

	ioaddr = pdev->cal_reg_base[DL] + CAL_CONFIG1;
	iodata = FPGA_inl(ioaddr);
	iodata &= ~0x08000000;
	FPGA_outl(ioaddr,iodata);
}
void getCoeKey(struct cal_struct *pdev)
{
	unsigned int iodata,ioaddr;

	ioaddr = pdev->cal_reg_base[DL] + CAL_CONFIG1;
	iodata = FPGA_inl(ioaddr);
	iodata |= 0x08000000;
	FPGA_outl(ioaddr,iodata);
}

/* showCalCoe()
 * ?? API´? Coef¸? ?ß????±? ?§??°??¸·? PE_TEST FPGAÆ???¿¡¼­¸¸ ±??¿??. 
 * ´?¼ø Coef¸? ??¼?¿¡¼­ ??·???±? ?§??. 
 * ¹???½? SB=0¿¡¼­¸¸ ±??¿??¾?¾ß ??. 
 */
void showCalCoe(struct cal_struct *pdev)
{
#if DEBUG_PRINT
	unsigned int d_addr,u_addr;
	unsigned int iodata,ioaddr;
	int i,j;
	int numb_tap;
	int unit_tap,offset;
	int real,imag;
	
	ioaddr   = pdev->cal_reg_base[DL] + CAL_CONFIG1;
	iodata   = FPGA_inl(ioaddr);
	offset 	 = (iodata >> 26) & 0x3f;
	numb_tap = (iodata >> 21) & 0x1f;
	unit_tap = (iodata >> 16) & 0x1f;

	printf("NUMB_TAP : %d\n",numb_tap);
	printf("UNIT_TAP : %d\n",unit_tap);
	printf("OFFSET   : %d\n",offset);

	getCoeKey(pdev);
	
	d_addr = pdev->cal_coe_base[DL];
	u_addr = pdev->cal_coe_base[UL];

	printf("\n%5s"," ");
	printf("%s\n","|<--Downlink-->|<---Uplink-->|");
	printf("\n");

    for (i=0;i<numb_tap;i++) {
        for (j=0;j<unit_tap;j++) {
			printf("%4d > ",i*unit_tap+j);
            iodata = FPGA_inl(d_addr | ((i*offset+j)<<2));		/* 8 -> 8FA */
			real = (iodata & 0xffff);
			real = (real & 0x8000) ? (real | 0xffff0000) : (real & 0xffff);
			imag = (iodata >> 16) & 0xffff;
			imag = (imag & 0x8000) ? (imag | 0xffff0000) : (imag & 0xffff);
			printf("%6d %6d|",real,imag);
            iodata = FPGA_inl(u_addr | ((i*offset+j)<<2));		/* 8 -> 8FA */
			real = (iodata & 0xffff);
			real = (real & 0x8000) ? (real | 0xffff0000) : (real & 0xffff);
			imag = (iodata >> 16) & 0xffff;
			imag = (imag & 0x8000) ? (imag | 0xffff0000) : (imag & 0xffff);
			printf("%6d %6d|",real,imag);
			printf("\n");
        }
    }
	printf("\n");
	putCoeKey(pdev);
#endif
}   

/* getCalCoe()
 * ?? API´? Coef¸? ?ß????±? ?§?? °??¸·? PE_TEST FPGAÆ???¿¡¼­¸¸ ±??¿??. 
 * Coef¸? Æ???·? ??????±? ?§??. 
 * ¹???½? SB=0¿¡¼­¸¸ ±??¿??¾?¾ß ??. 
 */
int getCalCoe(struct cal_struct *pdev, struct cal_file *pfile)
{
	unsigned int ioaddr;
	unsigned int iodata;
	int i,j;
	int numb_tap;
	int unit_tap,offset;
	
	ioaddr   = pdev->cal_reg_base[DL] + CAL_CONFIG1;
	iodata   = FPGA_inl(ioaddr);
	offset 	 = (iodata >> 26) & 0x3f;
	numb_tap = (iodata >> 21) & 0x1f;
	unit_tap = (iodata >> 16) & 0x1f;

	//pfile->nco = getCalNco(pdev,0);
	pfile->size = numb_tap*unit_tap;
	//printf("Nco : %2d\n",(int)pfile->nco);
	//printf("Size : %2d\n",(int)pfile->size);
	getCoeKey(pdev);
	ioaddr = pdev->cal_coe_base[DL];
    for (i=0;i<numb_tap;i++) {
        for (j=0;j<unit_tap;j++) {  
            iodata = FPGA_inl(ioaddr | ((i*offset+j)<<2));
			pfile->hi[DL][i*unit_tap+j] = (iodata >>  0) & 0xffff;
			pfile->hq[DL][i*unit_tap+j] = (iodata >> 16) & 0xffff;
        }   
    }
	ioaddr = pdev->cal_coe_base[UL];
    for (i=0;i<numb_tap;i++) {
        for (j=0;j<unit_tap;j++) {  
            iodata = FPGA_inl(ioaddr | ((i*offset+j)<<2));
			pfile->hi[UL][i*unit_tap+j] = (iodata >>  0) & 0xffff;
			pfile->hq[UL][i*unit_tap+j] = (iodata >> 16) & 0xffff;
        }   
    }
	putCoeKey(pdev);
	return (numb_tap*unit_tap);
}   

void saveCalInfoDef(void)
{
	struct ics_struct 	*pdev;
	pdev = getIcsDev(0);

	for(int i=0; i<16; i++){
		cinfo.tbl_ul_cal_struct_B28.hi[0][i]  = hi_Def[i];
		cinfo.tbl_ul_cal_struct_B28.hq[0][i]  = hq_Def[i];
	}
	for(int i=16; i<32; i++){
		cinfo.tbl_ul_cal_struct_B28.hi[1][i-16]  = hi_Def[16+i];
		cinfo.tbl_ul_cal_struct_B28.hq[1][i-16]  = hq_Def[16+i];
	}

	loadCalCoeForce(pdev,UL,cinfo.tbl_ul_cal_struct_B28.hi,cinfo.tbl_ul_cal_struct_B28.hq);
	saveCalTbl(&cinfo);
}

void saveCalInfo(void)
{
	struct ics_struct 	*pdev;
	pdev = getIcsDev(0);

	for(int i=0; i<16; i++){
		cinfo.tbl_ul_cal_struct_B28.hi[0][i]  = hn[DL][i][0];
		cinfo.tbl_ul_cal_struct_B28.hq[0][i]  = hn[DL][i][1];
	}
	for(int i=0; i<16; i++){
	
		cinfo.tbl_ul_cal_struct_B28.hi[1][i]  = hn[UL][i][0];
		cinfo.tbl_ul_cal_struct_B28.hq[1][i]  = hn[UL][i][1];
	}

	saveCalTbl(&cinfo);
}


void loadCalInfo(int path)
{
	struct ics_struct 	*pdev;
	pdev = getIcsDev(0);
	loadCalTbl(&cinfo);

	if( path == 0)
	{
		loadCalCoeForce(pdev,UL,0,0);
	}
	else
	{
		loadCalCoeForce(pdev,UL,&cinfo.tbl_ul_cal_struct_B28.hi[1][0],&cinfo.tbl_ul_cal_struct_B28.hq[1][0]);
	}
}



