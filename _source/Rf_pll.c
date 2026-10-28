#include <math.h>
#include "config.h"
#include "genlib.h"
#include "genlib.h"
#include "stddefs.h"
#include "psa.h"
#include "ics.h"
#include "rf.h"
#include "adf4106.h"

#include "repeater.h"
#include "protocol_private.h"
#include "fpga.h"

#define	PLL_CTRL	0x0000
#define	PLL_STAT	0x0004
#define	PLL_DATA	0x0018

#define	PLL_INIT	0x0100
#define	PLL_DONE	0x0200
#define	PLL_SEL		0xF000
#define	PLL_LOCK0	0x00010000
#define	PLL_LOCK1	0x00020000
#define	PLL_LOCK2	0x08040000
#define	PLL_LOCK3	0x10080000

#define __RFRVAL_16		15  ////14
#define __RFRVAL_12		12  ////10
#define __RFRVAL_8		8   ////10

#define RefFreq_245M		245760			// 245.76MHz
#define RefFreq_153M		153600			// 153.6MHz
#define RefFreq_61M			61440			// 61.44MHz
#define RefFreq_30M			30720			// 30.72MHz


#define FreqStep_10K		10L				// 0.1MHz = 100KHz
#define FreqStep_40K		40L				// 0.1MHz = 100KHz
#define FreqStep_50K		50L				// 0.1MHz = 100KHz
#define FreqStep_60K		60L				// 0.1MHz = 100KHz
#define FreqStep_20K		20L				// 0.1MHz = 100KHz
#define FreqStep_80K		80L				// 0.1MHz = 100KHz
#define FreqStep_100K		100L			// 0.1MHz = 100KHz
#define FreqStep_240K		240L			// 0.1MHz = 100KHz

#define	__PRE_32			32
#define	__PRE_16			16


#define	PreScl_8			8				// 0 : 8/9
#define	PreScl_16			16				// 1 : 16/17
#define	PreScl_32			32				// 2 : 32/33
#define	PreScl_64			64				// 3 : 64/65

#define DL_EN				0x00
#define UL_EN				0x01


#define PLL_ALARM_BASE 		0x00000
#define PLL_REG_BASE 		0x00004

#define	PLL_CTRL			0x0000
#define	PLL_STAT			0x0004
#define	PLL_DIN				0x000C
                        	
#define	PLL_INIT			0x0100
#define	PLL_DONE			0x0200
#define	PLL_NWR				0x0400
#define	PLL_SEL				0xF000


int __isPllLocked(int band)
{
    int					iodata;
    unsigned int 		mask[4] = {PLL_LOCK0,PLL_LOCK1,PLL_LOCK2,PLL_LOCK3};
    struct psa_struct 	*ppsa;

	ppsa = getPsaDev();
	//if (FPGA_DONE() != 1) {
	//	/* FPGA Done�� �ȶ߸� Lock���� �����Ѵ�.*/
	//	return (1);
	//}

	iodata = FPGA_inl(ppsa->psa_reg_base + PLL_STAT);
	return (iodata & mask[band]) ? 1:0;
}

int isPllLocked(int band)
{
	return (isPllLocked_Band(band)) ? 1:0;
}
extern void PLL_ClkWrite(unsigned char val);
extern void PLL_DataWrite(unsigned char val);
extern void PLL_EnWrite(unsigned char id, unsigned char val);

static void spiWritePllDev(int band, unsigned int val)
{
	int 				i;
	unsigned int 		dword;
    
	
	PLL_ClkWrite(0);
	if(band== 0)
		PLL_EnWrite(0,0);
	else
		PLL_EnWrite(1,0);

	dword = val;
	for (i=23;i>=0;i--) {
	    /* Clock low */
	    PLL_ClkWrite(0);
    
	    PLL_DataWrite((dword >> i) & 0x1);
	    
	    /* Clock high */
	    PLL_ClkWrite(1);
	}
	
	PLL_ClkWrite(0);
	
	if(band== 0)
	{
		PLL_EnWrite(0,1);	
		PLL_EnWrite(0,0);	
	}
	else
	{
		PLL_EnWrite(1,1);	
		PLL_EnWrite(1,0);	
	}
}





void confPllDef(int band, unsigned int local, unsigned int _NCNT, unsigned int _RCNT, unsigned int _CTRL, unsigned int _FUNC, int verbose)
{
	spiWritePllDev(band,_FUNC);    	//	3
//	spiWritePllDev(band,_CTRL);    	//	2
	spiWritePllDev(band,_RCNT);    	//	1
	spiWritePllDev(band,_NCNT);    	//	0
//#if DEBUG_PRINT
	if (verbose) {
		printf("%s Local=%d.%d,[0x%08x:0x%08x:0x%08x:0x%08x]\n",band_name[band],local/1000000,local%1000000,_NCNT,_RCNT,_CTRL,_FUNC);
	}
//#endif
}


void Pll4106_Output(unsigned band, unsigned int fvco, unsigned int ref, unsigned int _Step, unsigned char _PRE_Val)
{

	static 	int 	init = 0;
//	unsigned int CurrFreg = 0;
//	unsigned int CurrTmp = 0;

//	printf("\n ADF4113 fvco[%d], dStep[%d], dPre[%d]", fvco,_Step,ref/_Step);

	if (!init) {
//		printf("\n ADF4106 init\n");
	}

	fvco = fvco/_Step;	// 1.8Ghz = 18000 -> 1800000000

	{
		_RCounterReg RCounter;
		_NCounterReg NCounter;
		_FUNCReg 	Func;

		RCounter.Data = 0;				// init
		NCounter.Data = 0;				// init
		Func.Data = 0;

		// Reference Counter
		RCounter.Bit.control = __RCounter;


		RCounter.Bit.r_count = ref/_Step;	// RefFreq / FreqStep
		RCounter.Bit.DLY = 0;				// RCounter.Bit.DLY = 1;
//		RCounter.Bit.DLY = 1;				// RCounter.Bit.DLY = 1;

		// AB Counter
		NCounter.Bit.control = __NCounter;
		NCounter.Bit.a_count = fvco%_PRE_Val;
		NCounter.Bit.b_count = fvco/_PRE_Val;
//		NCounter.Bit.a_count = fvco%8;
//		NCounter.Bit.b_count = fvco/8;

		NCounter.Bit.cp_gain = 0;			// NCounter.Bit.CPGAIN = 1;

		// Function
		Func.Bit.control = __FUNCSet;
		Func.Bit.mux_sel = __LDDIG;
		Func.Bit.pd_pol = __POS;
		Func.Bit.CUR1SET = 7;
		Func.Bit.CUR2SET = 7;
//		Func.Bit.PLVALUE = PreScl;


		if(_PRE_Val == PreScl_8)
		{
			Func.Bit.PLVALUE = 0;
		}
		else if(_PRE_Val == PreScl_16)
		{
			Func.Bit.PLVALUE = 1;
		}
		else if(_PRE_Val == PreScl_32)
		{
			Func.Bit.PLVALUE = 2;
		}
		else if(_PRE_Val == PreScl_64)
		{
			Func.Bit.PLVALUE = 2;
		}
		{
		//	//printf("\n B [%d] P [%d] A[%d] PFD[%d]\n", NCounter.Bit.b_count,_PRE_Val,NCounter.Bit.a_count,_Step);
		//	CurrFreg  = (NCounter.Bit.b_count*_PRE_Val+NCounter.Bit.a_count)*_Step;
		//	printf("\n fvco[%d], CurrFreg[%d]\n", fvco*_Step,CurrFreg);
		//	printf("R Counter: %03x \n", RCounter.Grp.tot & 0xffffff);
		//	printf("N Counter: %03x \n", NCounter.Grp.tot & 0xffffff);
		//	printf("F Counter: %03x \n", Func.Grp.tot & 0xffffff);
		}

		spiWritePllDev(band,RCounter.Grp.tot);
		spiWritePllDev(band,NCounter.Grp.tot);
		spiWritePllDev(band,Func.Grp.tot);
	}
}



float convert_b3_earfcn_to_freq(int earfcn)
{
    // Band 3 Downlink ����: 1200 ~ 1949
    if (earfcn >= 1200 && earfcn <= 1949) {
        return 1805.0f + 0.1f * (earfcn - 1200);
    }

    return -1.0f; // Band 3 ������ ���
}

// 1803 ~ 1880
// 1805 MHz ~ 1880 MHz
// (1200 - 1949)
// C1 1825.46 ~ 1844.54 (1405 ~ 1595) (1825.5 ~ 1844.5) 19M
// C2 1860.46 ~ 1879.54 (1755 ~ 1945)
// Rakuten : 1825.0 ~ 1845.0  1500 (1400 ~ 1599)
// Docomo : 1860.1 ~ 1879.9 (1751 ~ 1949)

int __convert_b3_earfcn_to_freq(int earfcn)
{
    // Band 3 Downlink ����: 1200 ~ 1949
    if (earfcn >= 1200 && earfcn <= 1949) {
        return 1805000 + 100 * (earfcn - 1200);
    }

    return -1; // Band 3 ������ ���
}


#define KDDI_Center 2006100
#define RKT_Center 	2017900
#define SBM_Center 2035400
#define NTT_Center 2044700


void init_pll(void)
{
 	
}
//// C1 1500  1835 1557.1   277.9
//// C2 1850  1870 1583.9   286.1


///C1 2017.9
///C2 2061.1

void C2_PLL_Control(unsigned _Val)  ///C2
{
	
}


/* setLocalOsc()
 */
void setLocalOsc(uchar band, unsigned int lo)
{
 	
}


void setLocalTest(uchar band, unsigned int lo)
{
	

}


/*
KDDI 1805~1825	1815M
	 Earfcn	1200 ~ 1400	1300
RKT	 1825 ~ 1845	1835M
	 Earfcn	1400 ~ 1600	1500
SBM	 1845 ~ 1860	1852.5M
	 Earfcn	1600 ~ 1750	1675
NTT	 1860 ~ 1880	1870M
	 Earfcn	1750 ~ 1950	1850

KDDI_Center  	1300		2006.1
RKT_Center  	1500		2017.9
SBM_Center  	1675		2035.4
NTT_Center  	1850		2044.7

*/
////
int _Earfcn_Control(uint32_t _earfcn_val)
{

}

char *PllHelp[] = {
    "Pll",
    "pll : 15dB,0.5dB",
    0
};


extern int isPllLocked_Band(int band);

int
Pll(int argc, char *argv[])
{
	char *arg1,*arg2;
    int 			opt;
	unsigned int 	val;
 	struct repeater_struct 	*prpt;
	struct ics_struct 	*idev;

    if (argc < 2) {
#if DEBUG_PRINT
        printf("No arguments, exiting... use help\n");
#endif
        return(0);
    }

    while((opt=getopt(argc,argv,"v:")) != -1) {
        switch (opt) {
        case 'v':
        	val = (unsigned int)strtoul(optarg,0,0);
            break;
        default:
            return(0);
        }
    }
    arg1 = argv[optind];
    arg2 = argv[optind+1];
    prpt = getRptDev();
	
	idev = getIcsDev(CurrentBand);

    if (strcmp(arg1, "band") == 0) {
        int tmpband;
        if (argc == 3) {
            tmpband = atoi(argv[2]);
            if (tmpband < 4) {
                CurrentBand = tmpband;
            	printf("Subsequent Pll ops apply to Band[%d]\n",CurrentBand);
			}
        }
        else {
           	printf("Current PLL Band[%d]\n",CurrentBand);
        }
	}
    else if (strcmp(arg1, "set") == 0) {
		Pll4106_Output(CurrentBand, val, RefFreq_153M, FreqStep_50K, PreScl_8);
		printf("Set PLL [%d]\n",val);
	}
	else if (strcmp(arg1, "info") == 0) {
		int locked;

		locked = isPllLocked(CurrentBand);
		printf("%s PLL is %s[%d]\n", band_name[CurrentBand],locked ? "Lock":"Unlock", locked);
	}
/*	
	else if (strcmp(arg1, "earfcn") == 0) {
		printf("earfcn [%d]\n",val);

		_Earfcn_Control(val);
	}
*/	
	else if (strcmp(arg1, "earfcn") == 0) {
		static int		earfcn;
		int				rf_fc,rf_lo,bb_fc,bb_of;
		uchar			mno;
		extern int getB3FromEarfcn(int earfcn, int *rf_fc, int *rf_lo, int *bb_fc, int *bb_of, unsigned char *mno);
		extern void setLocalOsc(uchar band, unsigned int lo);

		if (strcmp(arg2, "set") == 0) {
			earfcn = val;
			if (getB3FromEarfcn(earfcn,&rf_fc,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				printf("Out of Range : %d\n", earfcn);
				return 0;
			}
			setLocalOsc(CurrentBand, rf_lo*100);
			setIcsMno(idev,mno);
			setIcsNco(idev,bb_of);
		}

	}


	return 0;
}



