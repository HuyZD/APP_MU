#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "ics_private.h"
#include "psa.h"
#include "cal.h"
#include "repeater.h"
#include "gpio.h"

char *onoff[2] = {"Off","On"};
char *enable[2] = {"Disabled","Enabled"};

int CurrentDev = 0;

extern struct tbl_cal_info_struct		cinfo;

static void showLmsCoef(struct ics_struct *pdev, int path)
{
	unsigned int ioaddr;
	unsigned int iodata;
	int i,j,k;
	int numb_tap;
	int unit_tap,offset;
	int real,imag;

	offset   = getIcsOffset(pdev,DL) + 1;
	numb_tap = getIcsNumbTap(pdev,DL) + 1;
	unit_tap = getIcsUnitTap(pdev,DL) + 1;

	printf("OFFSET   : %2d\n",offset);
	printf("NUMB_TAP : %2d\n",numb_tap);
	printf("UNIT_TAP : %2d\n",unit_tap);

	printf("\n%5s"," ");
	printf("%s\n","|<--Downlink->|<---Uplink-->|\n");

    for (i=0;i<numb_tap;i++) {
        for (j=0;j<unit_tap;j++) {
        	printf("%4d |",i*unit_tap+j);
        	for (k=0;k<2;k++) {
				ioaddr = pdev->coe_reg_base[k];

            	iodata = FPGA_inl(ioaddr | ((i*offset+j)<<2));
				real = (iodata & 0xffff);
				real = (real & 0x8000) ? (real | 0xffff0000) : (real & 0xffff);
				imag = (iodata >> 16) & 0xffff;
				imag = (imag & 0x8000) ? (imag | 0xffff0000) : (imag & 0xffff);
				if (real) {
					printf("%6d ",real);
				}
				else {
					printf("%6s ","0");
				}
				if (imag) {
					printf("%6d|",imag);
				}
				else {
					printf("%6s|","0");
				}
				//if (!(j%1))
				//	printf("%d\t0x%x\t\t%d\t%d\r\n",i*unit_tap+j,ioaddr+((i*unit_tap+j)<<2),real,imag);
				//}
			}
			printf("\n");
        }
    }
	printf("\n");
}   

static void showPwrInfo(struct ics_struct *pdev,int path)
{
	unsigned int i;
	char buf[32];

	if (pdev->band_info.type == 'l' || pdev->band_info.type == 'n') {
		printf("\n%10s","Path |");
		printf("%17s        |%17s        |\n","DownLink","UpLink");
		printf("%10s%13s%13s%13s%13s\n","Power |","Peak ","RMS |","Peak ","RMS |");
		printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");

		
		printf("%10s","adc |");
		for (path=0;path<2;path++) {
			printf("%20s dBm |",showfloat(buf,getIcsAdcPwr(path)));	
		}
		printf("\n");
		
		//printf("%10s","out |");
		//for (path=0;path<2;path++) {
		//	printf("%20s dBm |",showfloat(buf,pdev->outband_pwr[path]));	
		//}
		//printf("\n");
	
		printf("%10s","ddc |");
		for (path=0;path<2;path++) {
			printf("%6s dBm  ",showfloat(buf,getIcsDdcPkk(pdev,path)));
			printf("%6s dBm |",showfloat(buf,getIcsDdcPwr(pdev,path)));
		}
		printf("\n");
        
		printf("%10s","lms |");
		for (path=0;path<2;path++) {
			printf("%6s dBm  ",showfloat(buf,getIcsLmsPkk(pdev,path)));
			printf("%6s dBm |",showfloat(buf,getIcsLmsPwr(pdev,path)));
		}
		printf("\n");

		printf("%10s","fbk |");
		for (path=0;path<2;path++) {
			printf("%6s dBm  ",showfloat(buf,getIcsFbkPkk(pdev,path)));
			printf("%6s dBm |",showfloat(buf,getIcsFbkPwr(pdev,path)));
		}
		printf("\n");

		printf("%10s","att |");
		for (path=0;path<2;path++) {
			printf("%6s dBm  ",showfloat(buf,getIcsAttPkk(pdev,path)));
			printf("%6s dBm |",showfloat(buf,getIcsAttPwr(pdev,path)));
		}
		printf("\n");

		printf("%10s","usr |");
		for (path=0;path<2;path++) {
			printf("%6s dBm  ",showfloat(buf,getIcsUsrPkk(pdev,path)));
			printf("%6s dBm |",showfloat(buf,getIcsUsrPwr(pdev,path)));
		}
		printf("\n");

		printf("%10s","SSB |");
		printf("%6s dBm  ",showfloat(buf,getSsbPkk(pdev)));
		printf("%6s dBm  ",showfloat(buf,getSsbPwr(pdev)));
		printf("\n");		

		printf("%10s","CSI |");
		printf("%6s dBm  ",showfloat(buf,getCsiPkk(pdev)));
		printf("\n");		

	}
}

static void showIsoInfo(struct ics_struct *pdev, int path)
{
	int i,Bool=0;
	char buf[32];

	printf("%10s","Isolation|");
	for (i=0;i<2;i++) {
			printf(" %20s dB |",showfloat(buf,getIcsIso(pdev,i)));
	}
	printf("\n");
	printf("%10s","Ilc On |");
	Bool = getIcsIlcOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsIlcOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","Ilc Gain |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsIlcGain(pdev,i)));
	}
	printf("\n");

	printf("%10s","Ilc X+1 |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsIlcXth(pdev)));
	}
	printf("\n");

	printf("\n");
	printf("%10s","Ilc Tget |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsIlcTget(pdev)));
	}
	printf("\n");

	printf("%10s","Ilc Tatt |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsIlcTatt(pdev,i)));
	}
	printf("\n");
	printf("%10s","Ilc Setp |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsIlcSetp(pdev,i)));
	}
	printf("\n");
}

int signext(short val)
{
	int ret;
	
	ret = val;
	
	ret = (ret & 0x8000) ? (ret | 0xffff0000) : ret;
	return (ret);
}

static void showGainInfo(struct ics_struct *pdev, int path)
{
	int i,Bool=0;
	char buf[32];

	printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
	printf("%10s","Ics On |");
	Bool = getIcsOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
	printf("%10s","OVP/SLEEP|");
	Bool = getIcsFaMode(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsFaMode(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","Level |");
	for (i=0;i<1;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsFaThrLvl(pdev,DL)));
		printf(" %20s dB |",showfloat(buf,getIcsFaThrLvl(pdev,UL)));
	}
	printf("\n");
	printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
	printf("%10s","Alc On |");
	Bool = getIcsAlcOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsAlcOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","alc |");
	for (i=0;i<2;i++) {
		printf(" %7s dB |",showfloat(buf,getIcsAlcSetp(pdev,i)));
		printf(" %7s dB |",showfloat(buf,getIcsAlcGain(pdev,i)));
	}
	printf("\n");

	printf("%10s","Lim On |");
	Bool = getIcsAlcOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsAlcOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","lim |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsLimGain(pdev,i)));
	}
	printf("\n");

	printf("%10s","Agc On |");
	Bool = getIcsAgcOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsAgcOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","man |");
	for (i=0;i<2;i++) {
		printf(" %7s dB |",showfloat(buf,getIcsAgcSetp(pdev,DL)));
		printf(" %7s dB |",showfloat(buf,getIcsManGain(pdev,i)));
	}
	printf("\n");

	printf("%10s","ref |");
	for (i=0;i<2;i++) {
		printf(" %7s dB |",showfloat(buf,getIcsAgcSetp(pdev,UL)));
		printf(" %7s dB |",showfloat(buf,getIcsRefGain(pdev,i)));
	}
	printf("\n");

	printf("%10s","att |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsAttGain(pdev,i)));
	}
	printf("\n");

	printf("%10s","usr |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsUsrGain(pdev,i)));
	}
	printf("\n");
}

void showLteInfo(struct ics_struct *pdev)
{
	printf("LMS Tget Def: %7d | %7d |\n",getLmsTgetDef(pdev,DL),getLmsTgetDef(pdev,UL));
	printf("LMS Tget Run: %7d | %7d |\n",getLmsTgetRun(pdev,DL),getLmsTgetRun(pdev,UL));
	printf("LMS Step Def: %7d | %7d |\n",getLmsStepDef(pdev,DL),getLmsStepDef(pdev,UL));
	printf("LMS Step Run: %7d | %7d |\n",getLmsStepRun(pdev,DL),getLmsStepRun(pdev,UL));
	printf("LPC Step Def: %7d | %7d |\n",getLpcStepDef(pdev,DL),getLpcStepDef(pdev,UL));
	printf("LPC Step Run: %7d | %7d |\n",getLpcStepRun(pdev,DL),getLpcStepRun(pdev,UL));
	printf("\n");
	printf("LMS Tget Now: %7d | %7d |\n",getLmsTgetNow(pdev,DL),getLmsTgetNow(pdev,UL));
	printf("LMS Step Now: %7d | %7d |\n",getLmsStepNow(pdev,DL),getLmsStepNow(pdev,UL));
	printf("LPC Step Now: %7d | %7d |\n",getLpcStepNow(pdev,DL),getLpcStepNow(pdev,UL));
}

void showDlyInfo(struct ics_struct *pdev)
{
	printf("%10s","delay1");
	printf("%10d |",getIcsDelay1(pdev,DL));
	printf("%10d |",getIcsDelay1(pdev,UL));
	printf("\n");
	printf("%10s","delay2");
	printf("%10d |",getIcsDelay2(pdev,DL));
	printf("%10d |",getIcsDelay2(pdev,UL));
	printf("\n");
	printf("%10s","delay3");
	printf("%10d |",getIcsDelay3(pdev,DL));
	printf("%10d |",getIcsDelay3(pdev,UL));
	printf("\n");
}

/* showRegInfo()
 * fpga 내부의 레지스터 값들을 읽는다. 
 */
void showRegInfo(const char *desc[], unsigned int reg_base, unsigned int reg_size)
{
    int i;

    for (i=0;i<reg_size;i++) {
    	if (i%8 == 0) {
    		printf("0x%05X: ", (reg_base+i*4));
    	}

	    printf("%08x ", FPGA_inl(reg_base+i*4));

    	if (i%8 == 7) {
    		printf("\n");
    	}
    }
}
void showNcoInfo(const char *desc[], unsigned int reg_base, unsigned int reg_size)
{
    int i;
	
    for (i=0;i<reg_size;i++) { 
    	if (i%8 == 0) {
    		printf("0x%08X: ", (reg_base+i*4));
    	}

	    printf("%08x ", FPGA_inl(reg_base+i*4));

    	if (i%8 == 7) {
    		printf("\n");
    	}
    }
}

char *IcsHelp[] = {
    "Ics",
    "-[c:] -[v:] -[p:]",
    "",
    "Options:",
    "-c channel  specifiy fa 1 ~ 4",
    "-v value    specifiy value",
    "-p path     fwd or rvs path",
    "",
    "Operations:",
    "dev, on, off, fa, coe, atn, alc, info, reg",
    "ics -v 8 dly1 set",
    "ics -v 71 dly2 set",
    "ics -v -3400 agc man",
    "ics -v -2400 agc ref",
	"ics -v 0 fir set -> Filter Off",
	"ics -v 1 fir set -> 5M Filter Set",
	"ics -v 2 fir set -> 10M Filter Set",
	"ics -v 3 fir set -> 15M Filter Set",
	"ics -v 4 fir set -> 20M Filter Set",
	"ics -v 100 nco set  -> Fc+10M Freq Set",
	"ics -v -100 nco set -> Fc-10M Freq Set",
	"ics -v 0 nco set    -> Fc Freq Set",    
    "",
    "NOTE: none",
    0
};

extern void C2_PLL_Control(unsigned _Val);

int 
Ics(int argc, char *argv[])
{
	char 				*arg1,*arg2;
	int 				opt;
	int 				rval=0;
	int 				i;
	int 				band,sb,path=0,chan=0,val=0;
	struct ics_struct 	*pdev;
	char 				buf[32];

	int ival,fval;
	
	if (argc < 2) {
	    printf("No arguments, exiting... use help\n");
	    return(0);
	}

	while((opt=getopt(argc,argv,"duc:v:")) != -1) {
		switch (opt) {
		case 'd':
		    path = DL;
		    break;
		case 'u':
		    path = UL;
		    break;
		case 'c':
		    chan = (int)strtoul(optarg,0,0);
		    break;
		case 'v':
			val = (int)strtoul(optarg,0,0);
		    break;
		default:
		    return(0);
		}
	}
    
	arg1 = argv[optind];
	arg2 = argv[optind+1];

	pdev = getIcsDev(CurrentDev);
    
	if (strcmp(arg1, "band") == 0 || strcmp(arg1, "dev") == 0) {
        int tmpdev;
        if (argc == 3) {
            tmpdev = atoi(argv[2]);
            if (tmpdev < MAX_BAND) {
               	CurrentDev = tmpdev;
           		printf("Subsequent ics ops apply to band %d..\n",CurrentDev);
			}
        }
        else {
            printf("Current ics band: %d..\n",CurrentDev);
        }
	    return(0);
	}
	else if ((strcmp(arg1, "version") == 0) || (strcmp(arg1, "ver") == 0)) {
		val = getIcsVersion();
    	ival = (val >> 8) & 0xff;
    	fval = (val >> 4) & 0x0f;
    	printf("ICS Version : %d.%d\n",ival,fval);
	    return(0);
	}

	/* 아래 명령들은 특정 Band의 특정 SB에 대한 명령이다. 그렇기 때문에 정확한 Band와 SB가 명시되어야 한다. 
	 * 아래 명령이 돌아가기전 CurrentDev와 CurrentSB가 맞으면 상관없지만, 
	 * 틀리다면, 먼전 가능한 band의 가능한 sb를 plot해주고, 사용자가 쉽게 찾을 수 있도록 I/F를 제공해야 한다. 
	 */
	//pdev = getIcsDev(CurrentDev);
	//printf("CurrentDev = %d, %d\n", CurrentDev);  // 22-10-14
	//if (!pdev->supported) {
  	//	printf("Current Band[%d] is not installed\n",CurrentDev);
  	//	printf("The availabe Bands and SBs are listed below\n");
  	//	for (band=0;band<MAX_BAND;band++) {
  	//		for (sb=0;sb<MAX_SB;sb++) {
	//			pdev = getIcsDev(band);
	//			if (pdev->supported) {
  	//				printf("- %s,%s,%s[band=%d,sb=%d]\n", pdev->bd_name,pdev->sb_name,pdev->band_info.desc,pdev->bd_id,pdev->sb_id);
  	//			}
  	//		}
  	//	}
	//    return(0);
  	//}
  	
	/* 1. 해당 SB가 동작하기 위해서는 NCO를 우선 설정해 주어야 한다. 
	 * 
	 */
	else if (strcmp(arg1, "init") == 0) {
		init_ics();
	}
    else if (strcmp(arg1, "reg") == 0) {
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_reg_base[DL],pdev->ics_reg_size);
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_reg_base[UL],pdev->ics_reg_size);
		printf("\n");
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_pwr_base[DL],pdev->ics_pwr_size);
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_pwr_base[UL],pdev->ics_pwr_size);
	}
	else if (strcmp(arg1, "on") == 0) {
    	setIcsOnOff(pdev,DL,1);
    	setIcsOnOff(pdev,UL,1);
    }
	else if (strcmp(arg1, "off") == 0) {
    	setIcsOnOff(pdev,DL,0);
    	setIcsOnOff(pdev,UL,0);
    }
	else if (strcmp(arg1, "coe") == 0) {
		showLmsCoef(pdev,path);
	}
	else if (strcmp(arg1, "fir") == 0) {
		if (strcmp(arg2, "set") == 0) {
			setIcsFilter(pdev,val);
		}
		getIcsFilter(pdev,val);

		printf("getIcsFilter : %d\n", val);
	}
	else if (strcmp(arg1, "mno") == 0) {
	}
	else if (strcmp(arg1, "earfcn") == 0) {
		static int		earfcn;
		static int		step=1;
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
			setLocalOsc(CurrentDev, rf_lo*100);
			setIcsMno(pdev,mno);
			setIcsNco(pdev,bb_of);
		}
		else if (strcmp(arg2, "step") == 0) {
			step = val;
			printf("EARFCN Step : %d\n", step);
			return 0;
		}
		else if (strcmp(arg2, "+") == 0) {
			earfcn += step;
			if (getB3FromEarfcn(earfcn,&rf_fc,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				printf("Out of Range : %d\n", earfcn);
				return 0;
			}
			setLocalOsc(CurrentDev, rf_lo*100);
			setIcsMno(pdev,mno);
			setIcsNco(pdev,bb_of);
		}
		else if (strcmp(arg2, "-") == 0) {
			earfcn -= step;
			if (getB3FromEarfcn(earfcn,&rf_fc,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				printf("Out of Range : %d\n", earfcn);
				return 0;
			}
			setLocalOsc(CurrentDev, rf_lo*100);
			setIcsMno(pdev,mno);
			setIcsNco(pdev,bb_of);
		}
		else {
			if (getB3FromEarfcn(earfcn,&rf_fc,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				return 0;
			}
		}

		printf("%10s%10s%10s%10s%10s%10s","EARFCN|","RF Fc|","RF Lo|","BB Fc|","BB Offset|","MNO|");
		printf("\n");
		printf("%9d|",earfcn);
		printf("%7d.%d|",rf_fc/10, abs(rf_fc)%10);
		printf("%7d.%d|",rf_lo/10, abs(rf_lo)%10);
		printf("%7d.%d|",bb_fc/10, abs(bb_fc)%10);
		printf("%7d.%d|",bb_of/10, abs(bb_of)%10);
		printf("%9d|",mno);
		printf("\n");
	}
	else if (strcmp(arg1, "fc") == 0) {
		static int		rf_fc=0;
		static int		step=1;
		int				earfcn,rf_lo,bb_fc,bb_of;
		uchar	mno;
		extern int getB3FromRfFc(int rf_fc, int *earfcn, int *rf_lo, int *bb_fc, int *bb_of, unsigned char *mno);
		extern void setLocalOsc(uchar band, unsigned int lo);
		
		if (strcmp(arg2, "set") == 0) {
			rf_fc = val;
			if (getB3FromRfFc(rf_fc,&earfcn,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				printf("Out of Range : %d,%d\n", rf_fc/10, abs(rf_fc)%10);
				return 0;
			}
			setLocalOsc(CurrentDev, rf_lo*100);
			setIcsMno(pdev,mno);
			setIcsNco(pdev,bb_of);
		}
		else if (strcmp(arg2, "step") == 0) {
			step = val;
			printf("Fc Step : %d\n", step);
			return 0;
		}
		else if (strcmp(arg2, "+") == 0) {
			rf_fc += step;
			if (getB3FromRfFc(rf_fc,&earfcn,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				printf("Out of Range : %d,%d\n", rf_fc/10, abs(rf_fc)%10);
				return 0;
			}
			setLocalOsc(CurrentDev, rf_lo*100);
			setIcsMno(pdev,mno);
			setIcsNco(pdev,bb_of);
		}
		else if (strcmp(arg2, "-") == 0) {
			rf_fc -= step;
			if (getB3FromRfFc(rf_fc,&earfcn,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				printf("Out of Range : %d,%d\n", rf_fc/10, abs(rf_fc)%10);
				return 0;
			}
			setLocalOsc(CurrentDev, rf_lo*100);
			setIcsMno(pdev,mno);
			setIcsNco(pdev,bb_of);
		}
		else {
			if (getB3FromRfFc(rf_fc,&earfcn,&rf_lo,&bb_fc,&bb_of,&mno) == -1) {
				return 0;
			}
		}
		
		printf("%10s%10s%10s%10s%10s%10s","EARFCN|","RF Fc|","RF Lo|","BB Fc|","BB Offset|","MNO|");
		printf("\n");
		printf("%9d|",earfcn);
		printf("%7d.%d|",rf_fc/10, abs(rf_fc)%10);
		printf("%7d.%d|",rf_lo/10, abs(rf_lo)%10);
		printf("%7d.%d|",bb_fc/10, abs(bb_fc)%10);
		printf("%7d.%d|",bb_of/10, abs(bb_of)%10);
		printf("%9d|",mno);
		printf("\n");
	}
    else if (strcmp(arg1, "tget") == 0) {
		if (strcmp(arg2, "run") == 0) {
			pdev->lms_tget_run[DL] = val;
			pdev->lms_tget_run[UL] = val;
			setLmsTgetNow(pdev,  DL, pdev->lms_tget_run[DL]);   
			setLmsTgetNow(pdev,  UL, pdev->lms_tget_run[UL]);
		}
		else if (strcmp(arg2, "def") == 0) {
			pdev->lms_tget_def[DL] = val;
			pdev->lms_tget_def[UL] = val;
			setLmsTgetNow(pdev,DL, pdev->lms_tget_def[DL]);   
			setLmsTgetNow(pdev,UL, pdev->lms_tget_def[UL]);
		}
		showLteInfo(pdev);
	}
    else if (strcmp(arg1, "step") == 0) {
		if (strcmp(arg2, "run") == 0) {
			pdev->lms_step_run[DL] = val;
			pdev->lms_step_run[UL] = val;
			setLmsStepNow(pdev,DL,pdev->lms_step_run[DL]);   
			setLmsStepNow(pdev,UL,pdev->lms_step_run[UL]);
		}
		else if (strcmp(arg2, "def") == 0) {
			pdev->lms_step_def[DL] = val;
			pdev->lms_step_def[UL] = val;
			setLmsStepNow(pdev,DL,pdev->lms_step_def[DL]);   
			setLmsStepNow(pdev,UL,pdev->lms_step_def[UL]);
		}
		showLteInfo(pdev);
	}
	else if (strcmp(arg1, "dly") == 0) {
		showDlyInfo(pdev);
	}
	else if (strcmp(arg1, "dly1") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->delay1[path] = val;
			setIcsDelay1(pdev,path,pdev->delay1[path]);
		}
		else if (strcmp(arg2, "+") == 0) {
			pdev->delay1[path] += 1;
			setIcsDelay1(pdev,path,pdev->delay1[path]);
		}
		else if (strcmp(arg2, "-") == 0) {
			pdev->delay1[path] -= 1;
			setIcsDelay1(pdev,path,pdev->delay1[path]);
		}
		showDlyInfo(pdev);
  	}
	else if (strcmp(arg1, "dly2") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->delay2[path] = val;
			setIcsDelay2(pdev,path,pdev->delay2[path]);
		}
		else if (strcmp(arg2, "+") == 0) {
			pdev->delay2[path] += 1;
			setIcsDelay2(pdev,path,pdev->delay2[path]);
		}
		else if (strcmp(arg2, "-") == 0) {
			pdev->delay2[path] -= 1;
			setIcsDelay2(pdev,path,pdev->delay2[path]);
		}
		showDlyInfo(pdev);
  	}
    else if (strcmp(arg1, "nco") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->fc = val;
			setIcsNco(pdev,pdev->fc);
		}
		else if (strcmp(arg2, "dump") == 0) {
			showNcoInfo(0/*ics_reg_desc*/,pdev->nco_ram_base,pdev->nco_ram_size);
		}
	}
	else if (strcmp(arg1, "usr") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->usr_gain[path] = (short)val;
			pdev->init_gain[path] = (short)val;
			setIcsUsrGain(pdev,path,val);
		}
		printf("%10s","usr");
		ival = getIcsUsrGain(pdev,DL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		ival = getIcsUsrGain(pdev,UL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		printf("\n");
	}
	else if (strcmp(arg1, "att") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->att_gain[path] = (short)val;
			setIcsAttGain(pdev,path,val);
		}
		ival = getIcsAttGain(pdev,DL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		ival = getIcsAttGain(pdev,UL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		printf("\n");
	}
	else if (strcmp(arg1, "alc") == 0) {
		/* 'w'든 'l'든 ALC SETP는 각 Path별 한개만 존재 한다. */
		if (strcmp(arg2, "on") == 0) {
			pdev->alc_onoff[path] = 1;					
			setIcsAlcOnOff(pdev,path,pdev->alc_onoff[path]);		/* FA or Sleep 자동 동작 */
		}
		else if (strcmp(arg2, "off") == 0) {
			pdev->alc_onoff[path] = 0;					
			setIcsAlcOnOff(pdev,path,pdev->alc_onoff[path]);		/* FA or Sleep 수동 동작 */
		}
		else if (strcmp(arg2, "set") == 0) {
			pdev->alc_setp[path] = (short)val;
			setIcsAlcSetp(pdev,path, val);
		}
		else {
			printf("%10s","On/Off");
			ival = getIcsAlcOnOff(pdev,DL);
			printf("%12s |",ival ? "On" : "Off");
			ival = getIcsAlcOnOff(pdev,UL);
			printf("%12s |",ival ? "On" : "Off");
			printf("\n");
			printf("%10s","setp");
			ival = getIcsAlcSetp(pdev,DL);
			fval = abs(ival)%100;
			ival = ival/100;
			printf("%6d.%02d dB |",ival,fval);
			ival = getIcsAlcSetp(pdev,UL);
			fval = abs(ival)%100;
			ival = ival/100;
			printf("%6d.%02d dB |",ival,fval);
			printf("\n\n");
		}
	}
	else if (strcmp(arg1, "fa") == 0) {
		/* FA 동작을 확인하기 위함 
		 * FA Level 설정은 중계기 입력을 기준으로 하나, 여기서는 ICS 입력 기준으로 함. 
		 */
		if (strcmp(arg2, "on") == 0) {
			setIcsFaMode(pdev,DL,1);
		}
		else if (strcmp(arg2, "off") == 0) {
			setIcsFaMode(pdev,DL,0);
		}
		//else if (strcmp(arg2, "set") == 0) {
		//	setIcsFaAlloc(pdev, val);				/* FA 설정값 */
		//}
		else if (strcmp(arg2, "lvl") == 0) {
			setIcsFaThrLvl(pdev,DL,val);			/* 자동 동작시 FA 임계값 */
		}
		printf("%10s","On/Off");
		ival = getIcsFaMode(pdev,DL);
		printf("%12s |",ival ? "On" : "Off");
		printf("\n");
		printf("%10s","OvrPwrLvl");
		ival = getIcsFaThrLvl(pdev,DL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		printf("\n");
    }
  	else if (strcmp(arg1, "sleep") == 0) {
		if (strcmp(arg2, "on") == 0) {
			setIcsFaMode(pdev,UL, 1);
		}
		else if (strcmp(arg2, "off") == 0) {
			setIcsFaMode(pdev,UL, 0);
		}
		else if (strcmp(arg2, "lvl") == 0) {
    		setIcsFaThrLvl(pdev,UL,(int)val);
    	}
		//else if (strcmp(arg2, "set") == 0) {
  		//	setIcsSleepSetup(pdev,(int)val);
  		//}
		printf("%10s","On/Off");
		ival = getIcsFaMode(pdev,UL);
		printf("%12s |",ival ? "On" : "Off");
		printf("\n");
		printf("%10s","Sleep lvl");
		ival = getIcsFaThrLvl(pdev,UL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		printf("\n");
  	}
    else if (strcmp(arg1, "agc") == 0) {
		if (strcmp(arg2, "on") == 0) {
			pdev->agc_onoff[DL] = 1;					
			pdev->agc_onoff[UL] = 1;					
			setIcsAgcOnOff(pdev,DL,pdev->agc_onoff[DL]);
			setIcsAgcOnOff(pdev,UL,pdev->agc_onoff[UL]);
		}
		else if (strcmp(arg2, "off") == 0) {
			pdev->agc_onoff[DL] = 0;
			pdev->agc_onoff[UL] = 0;
			setIcsAgcOnOff(pdev,DL,pdev->agc_onoff[DL]);
			setIcsAgcOnOff(pdev,UL,pdev->agc_onoff[UL]);
		}
		else if (strcmp(arg2, "man") == 0) {
			pdev->agc_setp[0] = val;
			setIcsAgcSetp(pdev,DL,pdev->agc_setp[0]);
		}
		else if (strcmp(arg2, "ref") == 0) {
			pdev->agc_setp[1] = val;
			setIcsAgcSetp(pdev,UL,pdev->agc_setp[1]);
		}
		else if (strcmp(arg2, "all") == 0) {
			pdev->agc_setp[DL] = val;
			pdev->agc_setp[UL] = val;
			setIcsAgcSetp(pdev,DL,pdev->agc_setp[DL]);
			setIcsAgcSetp(pdev,UL,pdev->agc_setp[UL]);
		}
		printf("%10s","On/Off");
		ival = getIcsAgcOnOff(pdev,DL);
		printf("%12s |",ival ? "On" : "Off");
		ival = getIcsAgcOnOff(pdev,UL);
		printf("%12s |",ival ? "On" : "Off");
		printf("\n");
		printf("%10s","agc setp");
		ival = getIcsAgcSetp(pdev,DL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		ival = getIcsAgcSetp(pdev,UL);
		fval = abs(ival)%100;
		ival = ival/100;
		printf("%6d.%02d dB |",ival,fval);
		printf("\n\n");
	}
    else if (strcmp(arg1, "dac") == 0) {
		if (strcmp(arg2, "on") == 0) {
			pdev->ics_onoff[path] = 1;	
			setIcsDacOnOff(pdev,path,1);
		}
		else if (strcmp(arg2, "off") == 0) {
			pdev->ics_onoff[path] = 0;	
			setIcsDacOnOff(pdev,path,0);
		}
	}
//	else if  (strcmp(arg1, "pwr") == 0){
//		printf("%10s","LMS Total : ");
//		printf("  %s dB |",showfloat(buf,dl_rssi));
//		printf("  %s dB \n",showfloat(buf,ul_rssi));	
//	}
	else if (strcmp(arg1, "def") == 0) {
		saveIcsInfoDef(pdev);
	}

	else if (strcmp(arg1, "save") == 0) {
		saveIcsInfo(pdev);
	}
	else if (strcmp(arg1, "load") == 0) {
		loadIcsInfo(pdev);
//		printf("Band Fc   : %d.%d\n",pdev->fc/10,pdev->fc%10);   // 22-10-14
		printf("agc_setp   : %d.%d\n",pdev->agc_setp[0],pdev->agc_setp[1]);   // 22-10-14
	}
    else if (strcmp(arg1, "info") == 0) {
		pdev = getIcsDev(CurrentDev);
		printf("**** %s,%s\n", pdev->bd_name,pdev->band_info.desc);
		printf("Band Type : %c\n",pdev->band_info.type);   // 22-10-14
		printf("Band Fc   : %d.%d\n",pdev->fc/10,pdev->fc%10);   // 22-10-14

		/* LTE는 DL/UL을 같이 보여준다. */
		showPwrInfo(pdev,0);
		printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
		showIsoInfo(pdev,0);
		showGainInfo(pdev,0);
	}
    return(rval);
}

/**********************************************************************************
 * Pre related functions are defined here below. 
 ( 
 **********************************************************************************/
/* startCal()
 */
void startCal(struct ics_struct *pdev, int hn[2][16][2], int verbose)
{
	struct cal_file 	*pbuf;
	int					i,j,k;
	int					path;
	int					ddc_pwr[2],lms_pwr[2];
	char 				buf[32];
	int 				numb_tap;
	int 				unit_tap,offset;
	int 				real,imag;
	unsigned int 		ioaddr;
	unsigned int 		iodata;

	/* Initiate Calibration */
	printf("Checking input power....\n");
	for (i=0;i<10;i++) {
		for (path=0;path<2;path++) {
			ddc_pwr[path] = getIcsDdcPwr(pdev,path);
		}
		if ((-2500 < ddc_pwr[DL] && ddc_pwr[DL] < -2000) && (-2500 < ddc_pwr[UL] && ddc_pwr[UL] < -2000)) {
			break;
		}
		printf("%10s","ddc |");
		printf("%6s dBm  ",showfloat(buf,ddc_pwr[DL]));	
		printf("%6s dBm |",showfloat(buf,ddc_pwr[UL]));
		printf("\n");

		usleep(1000000);
	}
	if (i==10) {
		printf("Input Power is too low..\n");
		return;
	}
	else {
		printf("OK\n");
		printf("%10s","ddc |");
		printf("%6s dBm  ",showfloat(buf,ddc_pwr[DL]));	
		printf("%6s dBm |",showfloat(buf,ddc_pwr[UL]));
		printf("\n");
	}
	
	ddc_pwr[DL] = 0;
	ddc_pwr[UL] = 0;
	
	do {
		usleep(1000000);
		lms_pwr[DL] = getPreLmsPwr(pdev,DL);
		lms_pwr[UL] = getPreLmsPwr(pdev,UL);
		
		printf("%10s","ddc |");
		printf("%6s dBm  ",showfloat(buf,lms_pwr[DL]));	
		printf("%6s dBm |",showfloat(buf,lms_pwr[UL]));
		printf("\n");
		
		if (lms_pwr[DL] < ddc_pwr[DL]) {
			ddc_pwr[DL] = lms_pwr[DL];
			continue;
		}
		else {
			break;
		}
	} while (1); 

	do {
		usleep(1000000);
		lms_pwr[DL] = getPreLmsPwr(pdev,DL);
		lms_pwr[UL] = getPreLmsPwr(pdev,UL);
		
		printf("%10s","ddc |");
		printf("%6s dBm  ",showfloat(buf,lms_pwr[DL]));	
		printf("%6s dBm |",showfloat(buf,lms_pwr[UL]));
		printf("\n");
		
		if (lms_pwr[UL] < ddc_pwr[UL]) {
			ddc_pwr[UL] = lms_pwr[UL];
			continue;
		}
		else {
			break;
		}
	} while (1); 

	offset   = getIcsOffset(pdev,DL) + 1;
	numb_tap = getIcsNumbTap(pdev,DL) + 1;
	unit_tap = getIcsUnitTap(pdev,DL) + 1;
	
	printf("OFFSET   : %2d\n",offset);
	printf("NUMB_TAP : %2d\n",numb_tap);
	printf("UNIT_TAP : %2d\n",unit_tap);
	
    if (hn) {
    	for (i=0;i<numb_tap;i++) {
    	    for (j=0;j<unit_tap;j++) {  
    	    	for (k=0;k<2;k++) {  
					ioaddr = pdev->pre_reg_base[k];
    	        	iodata = FPGA_inl(ioaddr | ((i*offset+j)<<2));
					real = (iodata & 0xffff);
					real = (real & 0x8000) ? (real | 0xffff0000) : (real & 0xffff);
					imag = (iodata >> 16) & 0xffff;
					imag = (imag & 0x8000) ? (imag | 0xffff0000) : (imag & 0xffff);
					hn[k][j][0] = real;
					hn[k][j][1] = imag;
				}
    	    }   
    	}
    }

//	getCalCoe(pdev,pbuf);
//    
//	if (verbose) {
//		showCalCoe(pdev);
//	}
//
//	sprintf(buf,"%s",fname[path]);
//////	mon_tfsadd(buf, NULL, NULL, (int *)pbuf, sizeof(struct cal_file));
//	free(pbuf);
}

static void showPrePwrInfo(struct ics_struct *pdev,int path)
{
	unsigned int i;
	char buf[32];

	printf("\n%10s","Path |");
	printf("%17s        |%17s        |\n","DownLink","UpLink");
	printf("%10s%13s%13s%13s%13s\n","Power |","Peak ","RMS |","Peak ","RMS |");
	printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");

	
	printf("%10s","adc |");
	for (path=0;path<2;path++) {
		printf("%20s dBm |",showfloat(buf,getIcsAdcPwr(path)));	
	}
	printf("\n");
	
	//printf("%10s","out |");
	//for (path=0;path<2;path++) {
	//	printf("%20s dBm |",showfloat(buf,pdev->outband_pwr[path]));	
	//}
	//printf("\n");
	
	printf("%10s","ddc |");
	for (path=0;path<2;path++) {
		printf("%6s dBm  ",showfloat(buf,getIcsDdcPkk(pdev,path)));	
		printf("%6s dBm |",showfloat(buf,getIcsDdcPwr(pdev,path)));
	}
	printf("\n");
    
	printf("%10s","ref |");
	for (path=0;path<2;path++) {
		printf("%6s dBm  ",showfloat(buf,getPreRefPkk(pdev,path)));
		printf("%6s dBm |",showfloat(buf,getPreRefPwr(pdev,path)));
	}
	printf("\n");

	printf("%10s","man |");
	for (path=0;path<2;path++) {
		printf("%6s dBm  ",showfloat(buf,getPreManPkk(pdev,path)));	
		printf("%6s dBm |",showfloat(buf,getPreManPwr(pdev,path)));
	}
	printf("\n");
    
	printf("%10s","lms |");
	for (path=0;path<2;path++) {
		printf("%6s dBm  ",showfloat(buf,getPreLmsPkk(pdev,path)));
		printf("%6s dBm |",showfloat(buf,getPreLmsPwr(pdev,path)));
	}
	printf("\n");

	printf("%10s","usr |");
	for (path=0;path<2;path++) {
		printf("%6s dBm  ",showfloat(buf,getIcsUsrPkk(pdev,path)));
		printf("%6s dBm |",showfloat(buf,getIcsUsrPwr(pdev,path)));
	}
	printf("\n");
}

static void showPreGainInfo(struct ics_struct *pdev, int path)
{
	int i,Bool=0;
	char buf[32];

	printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
	printf("%10s","Ics On |");
	Bool = getIcsOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
	printf("%10s","Alc On |");
	Bool = getIcsAlcOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsAlcOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","alc |");
	for (i=0;i<2;i++) {
		printf(" %7s dB |",showfloat(buf,getIcsAlcSetp(pdev,i)));
		printf(" %7s dB |",showfloat(buf,getIcsAlcGain(pdev,i)));
	}
	printf("\n");
	
	printf("%10s","Lim On |");
	Bool = getIcsAlcOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsAlcOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","lim |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsLimGain(pdev,i)));
	}
	printf("\n");

	printf("%10s","Agc On |");
	Bool = getIcsAgcOnOff(pdev,DL);
	printf(" %23s |",onoff[Bool]);
	Bool = getIcsAgcOnOff(pdev,UL);
	printf(" %23s |",onoff[Bool]);
	printf("\n");
	printf("%10s","man |");
	for (i=0;i<2;i++) {
		printf(" %7s dB |",showfloat(buf,getPreAgcSetp(pdev,DL)));
		printf(" %7s dB |",showfloat(buf,getPreManGain(pdev,i)));
	}
	printf("\n");
	
	printf("%10s","ref |");
	for (i=0;i<2;i++) {
		printf(" %7s dB |",showfloat(buf,getPreAgcSetp(pdev,UL)));
		printf(" %7s dB |",showfloat(buf,getPreRefGain(pdev,i)));
	}
	printf("\n");
	
	printf("%10s","usr |");
	for (i=0;i<2;i++) {
		printf(" %20s dB |",showfloat(buf,getIcsUsrGain(pdev,i)));
	}
	printf("\n");
}

static void showPreCoef(struct ics_struct *pdev, int path)
{
	unsigned int ioaddr;
	unsigned int iodata;
	int i,j,k;
	int numb_tap;
	int unit_tap,offset;
	int real,imag;

	offset   = getIcsOffset(pdev,DL) + 1;
	numb_tap = getIcsNumbTap(pdev,DL) + 1;
	unit_tap = getIcsUnitTap(pdev,DL) + 1;
	
	printf("OFFSET   : %2d\n",offset);
	printf("NUMB_TAP : %2d\n",numb_tap);
	printf("UNIT_TAP : %2d\n",unit_tap);
	
	printf("\n%5s"," ");
	printf("%s\n","|<--Downlink->|<---Uplink-->|\n");
    
    for (i=0;i<numb_tap;i++) {
        for (j=0;j<unit_tap;j++) {  
        	printf("%4d |",i*unit_tap+j);
        	for (k=0;k<2;k++) {  
				ioaddr = pdev->pre_reg_base[k];
    
            	iodata = FPGA_inl(ioaddr | ((i*offset+j)<<2));
				real = (iodata & 0xffff);
				real = (real & 0x8000) ? (real | 0xffff0000) : (real & 0xffff);
				imag = (iodata >> 16) & 0xffff;
				imag = (imag & 0x8000) ? (imag | 0xffff0000) : (imag & 0xffff);
				if (real) {
					printf("%6d ",real);
				}
				else {
					printf("%6s ","0");
				}
				if (imag) {
					printf("%6d|",imag);
				}
				else {
					printf("%6s|","0");
				}
				//if (!(j%1))
				//	printf("%d\t0x%x\t\t%d\t%d\r\n",i*unit_tap+j,ioaddr+((i*unit_tap+j)<<2),real,imag);
				//}
			}
			printf("\n");
        }   
    }
	printf("\n");
}   

char *PreHelp[] = {
    "Pre",
    "-[c:] -[v:] -[p:]",
    "",
    "Options:",
    "-c channel  specifiy fa 1 ~ 4",
    "-v value    specifiy value",
    "-p path     fwd or rvs path",
    "",
    "Operations:",
    "dev, on, off, fa, coe, atn, alc, info, reg",
    "ics -v 8 dly1 set",
    "ics -v 71 dly2 set",
    "ics -v -3400 agc man",
    "ics -v -2400 agc ref",
    "",
    "NOTE: none",
    0
};

extern void loadICSTbl(struct ics_info *pinfo);
extern void saveCalTbl(struct tbl_cal_info_struct *tinfo);
extern void loadCalTbl(struct tbl_cal_info_struct *tinfo);
extern short hi_Def[32];
extern short hq_Def[32];

static int hn[2][16][2];

int 
Pre(int argc, char *argv[])
{
	char 				*arg1,*arg2;
	int 				opt;
	int 				rval=0;
	int 				i,j,k;
	int 				band,sb,path=0,chan=0,val=0;
	struct ics_struct 	*pdev;
	char 				buf[32];

	int ival,fval;
	
	if (argc < 2) {
	    printf("No arguments, exiting... use help\n");
	    return(0);
	}

	while((opt=getopt(argc,argv,"duc:v:")) != -1) {
		switch (opt) {
		case 'd':
		    path = DL;
		    break;
		case 'u':
		    path = UL;
		    break;
		case 'c':
		    chan = (int)strtoul(optarg,0,0);
		    break;
		case 'v':
			val = (int)strtoul(optarg,0,0);
		    break;
		default:
		    return(0);
		}
	}
    
	arg1 = argv[optind];
	arg2 = argv[optind+1];

	pdev = getIcsDev(CurrentDev);
    
	if (strcmp(arg1, "band") == 0 || strcmp(arg1, "dev") == 0) {
        int tmpdev;
        if (argc == 3) {
            tmpdev = atoi(argv[2]);
            if (tmpdev < MAX_BAND) {
               	CurrentDev = tmpdev;
           		printf("Subsequent ics ops apply to band %d..\n",CurrentDev);
			}
        }
        else {
            printf("Current ics band: %d..\n",CurrentDev);
        }
	    return(0);
	}
	else if ((strcmp(arg1, "version") == 0) || (strcmp(arg1, "ver") == 0)) {
		val = getIcsVersion();
    	ival = (val >> 8) & 0xff;
    	fval = (val >> 4) & 0x0f;
    	printf("ICS Version : %d.%d\n",ival,fval);
	    return(0);
	}
    else if (strcmp(arg1, "reg") == 0) {
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_reg_base[DL],pdev->ics_reg_size);
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_reg_base[UL],pdev->ics_reg_size);
		printf("\n");
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_pwr_base[DL],pdev->ics_pwr_size);
		showRegInfo(0/*ics_reg_desc*/,pdev->ics_pwr_base[UL],pdev->ics_pwr_size);
	}
	else if (strcmp(arg1, "phase") == 0) {
    	unsigned int ioaddr;
		int 	hn_i, hn_q;
		static int	phase = 0;
		
		//double 	degrees = 60.0;
    	//double 	radians = degrees * (M_PI / 180.0); // Convert degrees to radians
    	//double 	result = cos(radians);
		if (strcmp(arg2, "set") == 0) {
			phase = val;
			hn_i = (int)(cos((double)phase * (M_PI / 180.0)) * 0x3fffffff);
			hn_q = (int)(sin((double)phase * (M_PI / 180.0)) * 0x3fffffff);
    		ioaddr = pdev->ics_reg_base[path] + 0x40;
    		FPGA_outl(ioaddr,hn_i);
    		ioaddr = pdev->ics_reg_base[path] + 0x44;
    		FPGA_outl(ioaddr,hn_q);
		}
		else if (strcmp(arg2, "+") == 0) {
			phase += 1;
			hn_i = (int)(cos((double)phase * (M_PI / 180.0)) * 0x3fffffff);
			hn_q = (int)(sin((double)phase * (M_PI / 180.0)) * 0x3fffffff);
    		ioaddr = pdev->ics_reg_base[path] + 0x40;
    		FPGA_outl(ioaddr,hn_i);
    		ioaddr = pdev->ics_reg_base[path] + 0x44;
    		FPGA_outl(ioaddr,hn_q);
		}
		else if (strcmp(arg2, "-") == 0) {
			phase -= 1;
			hn_i = (int)(cos((double)phase * (M_PI / 180.0)) * 0x3fffffff);
			hn_q = (int)(sin((double)phase * (M_PI / 180.0)) * 0x3fffffff);
    		ioaddr = pdev->ics_reg_base[path] + 0x40;
    		FPGA_outl(ioaddr,hn_i);
    		ioaddr = pdev->ics_reg_base[path] + 0x44;
    		FPGA_outl(ioaddr,hn_q);
		}
		printf("%10s","phase");
		printf("%10d |",phase);
		//printf("%10d |",getIcsDelay1(pdev,UL));
		printf("\n");
		showPreCoef(pdev,path);
	}
	else if (strcmp(arg1, "on") == 0) {
    	setIcsOnOff(pdev,DL,1);
    	setIcsOnOff(pdev,UL,1);
  		
  		startCal(pdev, hn, 0);
    	for(i=0;i<1;i++) {
    		for(j=0;j<16;j++) {
    			for(k=0;k<2;k++) {
    				printf("%6d, %6d", hn[k][j][0], hn[k][j][1]);
    			}
    			printf("\n");
			}
		}    			
		printf("\n");  		
    }
	else if (strcmp(arg1, "off") == 0) {
    	setIcsOnOff(pdev,DL,0);
    	setIcsOnOff(pdev,UL,0);
    }
	else if (strcmp(arg1, "coe") == 0) {
		showPreCoef(pdev,path);
	}
    else if (strcmp(arg1, "tget") == 0) {
		if (strcmp(arg2, "run") == 0) {
			pdev->lms_tget_run[DL] = val;
			pdev->lms_tget_run[UL] = val;
			setLmsTgetNow(pdev,  DL, pdev->lms_tget_run[DL]);   
			setLmsTgetNow(pdev,  UL, pdev->lms_tget_run[UL]);
		}
		else if (strcmp(arg2, "def") == 0) {
			pdev->lms_tget_def[DL] = val;
			pdev->lms_tget_def[UL] = val;
			setLmsTgetNow(pdev,DL, pdev->lms_tget_def[DL]);   
			setLmsTgetNow(pdev,UL, pdev->lms_tget_def[UL]);
		}
		showLteInfo(pdev);
	}
    else if (strcmp(arg1, "step") == 0) {
		if (strcmp(arg2, "run") == 0) {
			pdev->lms_step_run[DL] = val;
			pdev->lms_step_run[UL] = val;
			setLmsStepNow(pdev,DL,pdev->lms_step_run[DL]);   
			setLmsStepNow(pdev,UL,pdev->lms_step_run[UL]);
		}
		else if (strcmp(arg2, "def") == 0) {
			pdev->lms_step_def[DL] = val;
			pdev->lms_step_def[UL] = val;
			setLmsStepNow(pdev,DL,pdev->lms_step_def[DL]);   
			setLmsStepNow(pdev,UL,pdev->lms_step_def[UL]);
		}
		showLteInfo(pdev);
	}
	else if (strcmp(arg1, "dly") == 0) {
		showDlyInfo(pdev);
	}
	else if (strcmp(arg1, "dly1") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->delay1[path] = val;
			setIcsDelay1(pdev,path,pdev->delay1[path]);
		}
		else if (strcmp(arg2, "+") == 0) {
			pdev->delay1[path] += 1;
			setIcsDelay1(pdev,path,pdev->delay1[path]);
		}
		else if (strcmp(arg2, "-") == 0) {
			pdev->delay1[path] -= 1;
			setIcsDelay1(pdev,path,pdev->delay1[path]);
		}
		showDlyInfo(pdev);
  	}
	else if (strcmp(arg1, "dly2") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->delay2[path] = val;
			setIcsDelay2(pdev,path,pdev->delay2[path]);
		}
		else if (strcmp(arg2, "+") == 0) {
			pdev->delay2[path] += 1;
			setIcsDelay2(pdev,path,pdev->delay2[path]);
		}
		else if (strcmp(arg2, "-") == 0) {
			pdev->delay2[path] -= 1;
			setIcsDelay2(pdev,path,pdev->delay2[path]);
		}
		showDlyInfo(pdev);
  	}
	else if (strcmp(arg1, "dly3") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->delay3[path] = val;
			setIcsDelay3(pdev,path,pdev->delay3[path]);
		}
		else if (strcmp(arg2, "+") == 0) {
			pdev->delay3[path] += 1;
			setIcsDelay3(pdev,path,pdev->delay3[path]);
		}
		else if (strcmp(arg2, "-") == 0) {
			pdev->delay3[path] -= 1;
			setIcsDelay3(pdev,path,pdev->delay3[path]);
		}
		showDlyInfo(pdev);
  	}
	else if (strcmp(arg1, "alc") == 0) {
		if (strcmp(arg2, "on") == 0) {
			pdev->alc_onoff[path] = 1;					
			setIcsAlcOnOff(pdev,path,pdev->alc_onoff[path]);		/* FA or Sleep 자동 동작 */
		}
		else if (strcmp(arg2, "off") == 0) {
			pdev->alc_onoff[path] = 0;					
			setIcsAlcOnOff(pdev,path,pdev->alc_onoff[path]);		/* FA or Sleep 수동 동작 */
		}
		else {
			printf("%10s","On/Off");
			ival = getIcsAlcOnOff(pdev,DL);
			printf("%12s |",ival ? "On" : "Off");
			ival = getIcsAlcOnOff(pdev,UL);
			printf("%12s |",ival ? "On" : "Off");
			printf("\n");
			printf("%10s","setp");
			ival = getIcsAlcSetp(pdev,DL);
			fval = abs(ival)%100;
			ival = ival/100;
			printf("%6d.%02d dB |",ival,fval);
			ival = getIcsAlcSetp(pdev,UL);
			fval = abs(ival)%100;
			ival = ival/100;
			printf("%6d.%02d dB |",ival,fval);
			printf("\n\n");
		}
	}
	else if (strcmp(arg1, "def") == 0) {
		loadCalCoeDef(pdev);
	}
	else if (strcmp(arg1, "force") == 0) {
//			loadCalCoeForce(pdev,UL,0,0);
		if(val == 0)
			loadCalInfo(0); /// 0 Default Value
		else 
			loadCalInfo(1); /// 0 Default Value

	}
	else if (strcmp(arg1, "save") == 0) {
		int 			num;
		struct cal_file coe;
		
		for(int i=0; i<16; i++){
			coe.hi[0][i]  = hn[DL][i][0];
			coe.hq[0][i]  = hn[DL][i][1];

			cinfo.tbl_ul_cal_struct_B28.hi[0][i]  = hn[DL][i][0];
			cinfo.tbl_ul_cal_struct_B28.hq[0][i]  = hn[DL][i][1];
		}
		for(int i=0; i<16; i++){
			coe.hi[1][i]  = hn[UL][i][0];
			coe.hq[1][i]  = hn[UL][i][1];
			cinfo.tbl_ul_cal_struct_B28.hi[1][i]  = hn[UL][i][0];
			cinfo.tbl_ul_cal_struct_B28.hq[1][i]  = hn[UL][i][1];
		}

		//loadCalCoeForce(pdev,UL,coe.hi,coe.hq);
		saveCalTbl(&cinfo);
	}
	else if (strcmp(arg1, "load") == 0) {
		loadCalTbl(&cinfo);
		
		printf("**** %s,%s\n", pdev->bd_name,pdev->band_info.desc);
		for(int i=0; i<16; i++){
			printf("%2d : %6d, %6d\n",i, cinfo.tbl_ul_cal_struct_B28.hi[1][i], cinfo.tbl_ul_cal_struct_B28.hq[1][i]);
		}

		
		//loadCalCoeForce(pdev,UL,cinfo.tbl_ul_cal_struct_B28.hi,cinfo.tbl_ul_cal_struct_B28.hq);

//		printf("Cal Fc   : %d,%d\n",cinfo.tbl_ul_cal_struct_B28.hi[0][0],cinfo.tbl_ul_cal_struct_B28.hi[0][1]);   // 22-10-14

//		loadIcsInfo(pdev);
//		printf("Band Fc   : %d.%d\n",pdev->fc/10,pdev->fc%10);   // 22-10-14
	}
    else if (strcmp(arg1, "info") == 0) {
		pdev = getIcsDev(CurrentDev);
		printf("**** %s,%s\n", pdev->bd_name,pdev->band_info.desc);
		printf("Band Type : %c\n",pdev->band_info.type);   // 22-10-14
		printf("Band Fc   : %d.%d\n",pdev->fc/10,pdev->fc%10);   // 22-10-14

		showPrePwrInfo(pdev,0);
		printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
		showPreGainInfo(pdev,0);
	}
    else if (strcmp(arg1, "cal") == 0) {
		extern void saveCalTbl(struct tbl_cal_info_struct *tinfo);


 		static short hi[32] = {  -46, 	212, 3597, -331,  190,  -37,    0,   19,  -34,    0,    3,   -9,    7,    0,   -2,    1,
 1119,-3882, 6917, -146,-1042, 1476,-1092,  499,    0, -272,  293, -158,    0,   87,  -94,   50};

		static short hq[32] = { 65,  -133, -8938,   636,  -145,     0,    10,   -13,     0,    20,   -22,     0,    10,   -13,     7,     0,
 -37, -1329,  9470, -3177,  3604, -2472,  1052,     0,  -527,   598,  -424,   180,     0,   -69,    65,   -29};
 
		int 			num;
		struct cal_file coe;
//		num = getCalCoe(pdev,&coe);
		
		for(int i=0; i<16; i++){
			coe.hi[0][i]  = hi[i];
			coe.hq[0][i]  = hq[i];
		}
		for(int i=16; i<32; i++){
			coe.hi[1][i-16]  = hi[16+i];
			coe.hq[1][i-16]  = hq[16+i];
		}

		loadCalCoeForce(pdev,UL,coe.hi,coe.hq);
		saveCalTbl(&cinfo);
//		loadCalCoeForce(pdev,UL,hi,hq);
    }
	else if (strcmp(arg1, "ship") == 0) {
//		saveCalInfoDef();
		saveCalInfo();
	}

#if 0
	else if (strcmp(arg1, "cal") == 0) {
//		extern void saveCalTbl(struct tbl_cal_info_struct *tinfo);
//		extern void loadCalTbl(struct tbl_cal_info_struct *tinfo);
	//	int 			num;
	//	struct cal_file coe;

		loadCalTbl(&cinfo);
	}
#endif


	
    return(rval);
}

/**********************************************************************************
 **********************************************************************************/
char *IlcHelp[] = {
    "Ilc",
    "-[c:] -[v:] -[p:]",
    "",
    "Options:",
    "-v value    specifiy value",
    "",
    "Operations:",
    "on, off, xth, tget, coe, info",
    "ilc -v 1550 xth set",
    "ilc -v -3000 tget set xth set",
    "",
    "NOTE: none",
    0
};

int 
Ilc(int argc, char *argv[])
{
    char *arg1,*arg2;
    int opt;
    int rval=0;
	int val=0;
	struct ics_struct *pdev;

	int ival,fval,band;
	
    if (argc < 2) {
        printf("No arguments, exiting... use help\n");
        return(0);
    }

	pdev = getIcsDev(CurrentDev);

    while((opt=getopt(argc,argv,"v:")) != -1) {
        switch (opt) {
            case 'v':
            	val = (int)strtoul(optarg,0,0);
                break;
            default:
                return(0);
        }
    }
    
    arg1 = argv[optind];
    arg2 = argv[optind+1];
    
	/* ILC 동작 설정. 
	 * 1. TGET을 맞춘다. ILC_TGET은 정격 입력일 경우 ILC_TATT이 0dB가 되도록 설정한다. 
	 *    입력이 정격 이하인 경우 ILC_TATT이 증가하여 ATT(MAN)과 FBK(REF)을 ILC_TATT만큼 이득을 증가시킨다. 특히 무신호시 ILC 동작을 빠르게 한다.
	 * 	  무신호시 갑작스런 입력을 넣으면 출력이 퍼지지 않게 하기 위하여, ilc_agc의 수렴속도는 lte_alc의 수렴속도와 같게 한다. 매우 빠르게 동작시켜야 한다. 
	 * 2. Xth를 설정한다. 
	 * 3. Xth와 System ALC는 같이 연동하는 것이 좋다. 
	 *    System ALC가 동작하는 조건은 입력이 정격 이상인 경우이다. 이 경우에는 Xth를 15dB이상 가져갈 필요가 없다. 
	 *    ICS 성능은 항상 정격 이하인 경우, 즉 최대 이득 조건에서 유효하다. 정격 이상인 경우는 그 증가분만큼 Xth가 줄어들 필요가 있다. 
	 *    정격 이상인 경우 동작하는 것이 바로 ALC이기에 ALC가 1dB 동작하면, Xth도 1dB 감소시킬 필요가 있다. 
	 */
	if (strcmp(arg1, "on") == 0) {
    	setIcsIlcOnOff(pdev,DL,1);
    	setIcsIlcOnOff(pdev,UL,1);
    }
	else if (strcmp(arg1, "off") == 0) {
    	setIcsIlcOnOff(pdev,DL,0);
    	setIcsIlcOnOff(pdev,UL,0);
    }
	else if (strcmp(arg1, "xth") == 0) {
		if (strcmp(arg2, "set") == 0) {
			pdev->ilc_xth = (short)val;
			setIcsIlcXth(pdev,val);
		}
	}
	else if (strcmp(arg1, "tget") == 0) {
		if (strcmp(arg2, "set") == 0) {
			/* 정격일때 TATT이 0dB가 되도록 설정 */
			pdev->ilc_tget = val;
			setIcsIlcTget(pdev,pdev->ilc_tget);
		}
	}
	else if (strcmp(arg1, "info") == 0) {
		showIsoInfo(pdev,0);
	}
	showIsoInfo(pdev,0);

    return(rval);
}

static void showLpcCoef(struct ics_struct *pdev)
{
	unsigned int ioaddr;
	unsigned int iodata;
	int i,j,k;
	int numb_tap;
	int unit_tap,offset;
	int real,imag;
	
	offset = getIcsOffset(pdev,UL);
	numb_tap = getIcsNumbTap(pdev,UL);
	unit_tap = getIcsUnitTap(pdev,UL);
	
	printf("OFFSET   : %d\n",offset);
	printf("NUMB_TAP : %d\n",numb_tap);
	printf("UNIT_TAP : %d\n",unit_tap);
	
	printf("\n%5s"," ");
	printf("%s\n","|<--Downlink->|<--Uplink--->|\n");
    for (i=0;i<numb_tap;i++) {
        for (j=0;j<unit_tap;j++) {  
        	printf("%4d |",i*unit_tap+j);
        	for (k=0;k<2;k++) {  
				ioaddr = pdev->lpc_reg_base[k];

            	iodata = FPGA_inl(ioaddr | ((i*offset+j)<<2));
				real = (iodata & 0xffff);
				real = (real & 0x8000) ? (real | 0xffff0000) : (real & 0xffff);
				imag = (iodata >> 16) & 0xffff;
				imag = (imag & 0x8000) ? (imag | 0xffff0000) : (imag & 0xffff);
				if (real) {
					printf("%6d ",real);
				}
				else {
					printf("%6s ","-");
				}
				if (imag) {
					printf("%6d|",imag);
				}
				else {
					printf("%6s|","-");
				}
				//if (!(j%1))
				//	printf("%d\t0x%x\t\t%d\t%d\r\n",i*unit_tap+j,ioaddr+((i*unit_tap+j)<<2),real,imag);
				//}
			}
			printf("\n");
        }   
    }
	printf("\n");
}   

char *LpcHelp[] = {
    "Lpc",
    "-[c:] -[v:] -[p:]",
    "",
    "Options:",
    "-v value    specifiy value",
    "",
    "Operations:",
    "dev, on, off, fa, coe, atn, alc, info, reg",
    "",
    "NOTE: none",
    0
};

int 
Lpc(int argc, char *argv[])
{
    char *arg1,*arg2;
    int opt;
    int rval=0;
	int val=0;
	struct ics_struct *pdev;

	int ival,fval,band;
	
    if (argc < 2) {
        printf("No arguments, exiting... use help\n");
        return(0);
    }

	pdev = getIcsDev(CurrentDev);

    while((opt=getopt(argc,argv,"v:")) != -1) {
        switch (opt) {
            case 'v':
            	val = (int)strtoul(optarg,0,0);
                break;
            default:
                return(0);
        }
    }
    
    arg1 = argv[optind];
    arg2 = argv[optind+1];
    
	if (strcmp(arg1, "on") == 0) {
    	setLpcOnOff(pdev,DL,1);
    	setLpcOnOff(pdev,UL,1);
    }
	else if (strcmp(arg1, "off") == 0) {
    	setLpcOnOff(pdev,DL,0);
    	setLpcOnOff(pdev,UL,0);
    }
	else if (strcmp(arg1, "coe") == 0) {
		showLpcCoef(pdev);
    }
    else if (strcmp(arg1, "step") == 0) {
		if (strcmp(arg2, "run") == 0) {
			pdev->lpc_step_run[DL] = val;
			pdev->lpc_step_run[UL] = val;
			setLpcStepNow(pdev,DL, pdev->lpc_step_run[DL]);   
			setLpcStepNow(pdev,UL, pdev->lpc_step_run[UL]);
		}
		else if (strcmp(arg2, "def") == 0) {
			pdev->lpc_step_def[DL] = val; 
			pdev->lpc_step_def[UL] = val;
			setLpcStepNow(pdev,DL, pdev->lpc_step_def[DL]);   
			setLpcStepNow(pdev,UL, pdev->lpc_step_def[UL]);
		}
		showLteInfo(pdev);
		//printf("%10s","lpc step");
		//printf("%6d, %6d \n",getLpcStepNow(pdev,DL),getLpcStepNow(pdev,UL));
	}
	else if (strcmp(arg1, "dly") == 0) {
		ival = getLpcMdly(pdev);
		printf("M-Delay:%4d\n",ival);
		ival = getLpcRdly(pdev);
		printf("R-Delay:%4d\n",ival);
	}
	else if (strcmp(arg1, "mdly") == 0) {
		static int mdly=0;
		if (strcmp(arg2, "set") == 0) {
			mdly = val;
			setLpcMdly(pdev,mdly);
		}
		else if (strcmp(arg2, "+") == 0) {
			mdly++;
			setLpcMdly(pdev,mdly);
		}
		else if (strcmp(arg2, "-") == 0) {
			mdly--;
			setLpcMdly(pdev,mdly);
		}
		ival = getLpcMdly(pdev);
		printf("M-Delay:%4d\n",ival);
		ival = getLpcRdly(pdev);
		printf("R-Delay:%4d\n",ival);
    }
	else if (strcmp(arg1, "rdly") == 0) {
		static int rdly=0;
		if (strcmp(arg2, "set") == 0) {
			rdly = val;
			setLpcRdly(pdev,rdly);
		}
		else if (strcmp(arg2, "+") == 0) {
			rdly++;
			setLpcRdly(pdev,rdly);
		}
		else if (strcmp(arg2, "-") == 0) {
			rdly--;
			setLpcRdly(pdev,rdly);
		}
		ival = getLpcMdly(pdev);
		printf("M-Delay:%4d\n",ival);
		ival = getLpcRdly(pdev);
		printf("R-Delay:%4d\n",ival);
    }

    return(rval);
}

#define MASK_SYN	0x0001
#define MASK_LNA	0x0002
#define MASK_PAM	0x0004
#define MASK_RSW	0x0008

static void showLatencyInfo(struct ics_struct *pdev, int verbose)
{
	int		path;
	
	printf("%10s%10s%10s\n","|","-------------------------|","-------------------------|");
	printf("%10s%13s%13s%13s%13s\n","Latency |","Setup ","Hold |","Setup ","Hold |");

	if (verbose & MASK_SYN) {
		printf("%10s","SYN |");
		for (path=0;path<2;path++) {
			printf("%12d ",getTddSynSetp(pdev,path));	
			printf("%11d |",getTddSynHold(pdev,path));	
		}
		printf("\n");
	}
	if (verbose & MASK_LNA) {
		printf("%10s","LNA |");
		for (path=0;path<2;path++) {
			printf("%12d ",getTddLnaSetp(pdev,path));	
			printf("%11d |",getTddLnaHold(pdev,path));	
		}
		printf("\n");
	}
	if (verbose & MASK_PAM) {
		printf("%10s","PAM |");
		for (path=0;path<2;path++) {
			printf("%12d ",getTddPamSetp(pdev,path));	
			printf("%11d |",getTddPamHold(pdev,path));	
		}
		printf("\n");
	}
	if (verbose & MASK_RSW) {
		printf("%10s","RSW |");
		for (path=0;path<2;path++) {
			printf("%12d ",getTddRswSetp(pdev,path));	
			printf("%11d |",getTddRswHold(pdev,path));	
		}
		printf("\n");
	}
}

char *PwmHelp[] = {
    "Pwm Dac",
    "-[c:] -[v:] ",
    "",
    "Options:",
    "-v value    specifiy value",
	"-c chanel   0: TR;  1: IR_BR1; 2: IR_BR2; 3: IR_BR3; 4: IR_BR4",
	"pwm save",
	"pwm load",
    "NOTE: none",
    0
};

int Pwm(int argc, char *argv[]) {
	char *arg1;
	int opt;
	int rval = 0;
	int val = 0;
	int channel = -1;
	int i;
	uint16_t dac_value;
	uint32_t voltage_mV;
	struct repeater_struct *pdev;
	pdev = getRptDev();

	while ((opt = getopt(argc, argv, "c:v:")) != -1) {

		switch (opt) {

		case 'c':
			channel = (int)strtoul(optarg, 0, 0);

			if (channel < 0 || channel > 4) {
				printf("Invalid channel: %d\n", channel);
				printf("Channel: 0: TR; 1: IR_BR1; 2: IR_BR2; 3: IR_BR3; 4: IR_BR4\n");
				return 0;
			}
			break;

		case 'v':
			val = (int)strtoul(optarg, 0, 0);

			if (val < 0 || val > 3300) {
				printf("Invalid voltage: %d mV\n", val);
				printf("Voltage range: 0 ~ 3300 mV\n");
				return 0;
			}
			break;

		default:
			return 0;
		}
	}

	arg1 = argv[optind];

	if (arg1 == NULL) {
		printf("Usage:\n");
		printf(" pwm -c <channel> -v <value> set\n");
		printf(" pwm -c <channel> +\n");
		printf(" pwm -c <channel> -\n");
		printf(" pwm save\n");
		printf(" pwm load\n");
		return 0;
	}

	if (strcmp(arg1, "save") == 0) {
		printf("PWM save\n");
		saveRepeaterInfo(pdev);
		return 0;
	}

	if (strcmp(arg1, "load") == 0) {

		printf("PWM load\n");

		loadRepeaterInfo(pdev);

		setPwmDac(0, (uint16_t)pdev->tr_pwm);

		voltage_mV =
			((uint32_t)(uint16_t)pdev->tr_pwm * 3300U) / 65535U;

		printf("PWM TR Voltage = %d mV\n",(int)voltage_mV);

		for (i = 0; i < 4; i++) {

		    setPwmDac(i + 1, (uint16_t)pdev->ir_pwm_br[i]);

		    voltage_mV =
		        ((uint32_t)(uint16_t)pdev->ir_pwm_br[i] * 3300U) / 65535U;

		    printf("PWM IR_BR%d Voltage = %d mV\n", i + 1,(int)voltage_mV);
		}

		return 0;
	}

	if (channel < 0 || channel > 4) {
		printf("Channel is required\n");
		printf("0: TR; 1: IR_BR1; 2: IR_BR2; 3: IR_BR3; 4: IR_BR4\n");
		return 0;
	}

	if (strcmp(arg1, "set") == 0) {

		dac_value =
			(uint16_t)(((uint32_t)val * 65535U) / 3300U);

		if (channel == 0)
			pdev->tr_pwm = (int16_t)dac_value;
		else
			pdev->ir_pwm_br[channel - 1] = (int16_t)dac_value;

		setPwmDac(channel, dac_value);

		voltage_mV =
			((uint32_t)dac_value * 3300U) / 65535U;

		if (channel == 0) {
			printf("PWM TR Voltage = %ld mV\n", (int)voltage_mV);
		} else {
			printf("PWM IR_BR%d Voltage = %d mV\n",  channel, (int)voltage_mV);
		}
	}

	else if (strcmp(arg1, "+") == 0) {

		if (channel == 0)
			dac_value = (uint16_t)pdev->tr_pwm;
		else
			dac_value = (uint16_t)pdev->ir_pwm_br[channel - 1];

		if (dac_value <= 0xEFFF)
			dac_value += 0x1000;
		else
			dac_value = 0xFFFF;

		if (channel == 0)
			pdev->tr_pwm = (int16_t)dac_value;
		else
			pdev->ir_pwm_br[channel - 1] = (int16_t)dac_value;

		setPwmDac(channel, dac_value);

		voltage_mV = ((uint32_t)dac_value * 3300U) / 65535U;

		if (channel == 0) {
			printf("PWM TR Voltage = %ld mV\n", (int)voltage_mV);
		} else {
			printf("PWM IR_BR%d Voltage = %d mV\n",  channel, (int)voltage_mV);
		}
	}


	else if (strcmp(arg1, "-") == 0) {

		if (channel == 0)
			dac_value = (uint16_t)pdev->tr_pwm;
		else
			dac_value = (uint16_t)pdev->ir_pwm_br[channel - 1];

		if (dac_value >= 0x1000)
			dac_value -= 0x1000;
		else
			dac_value = 0x0000;

		if (channel == 0)
			pdev->tr_pwm = (int16_t)dac_value;
		else
			pdev->ir_pwm_br[channel - 1] = (int16_t)dac_value;

		setPwmDac(channel, dac_value);

		voltage_mV = ((uint32_t)dac_value * 3300U) / 65535U;

		if (channel == 0) {
			printf("PWM TR Voltage = %ld mV\n", (int)voltage_mV);
		} else {
			printf("PWM IR_BR%d Voltage = %d mV\n",  channel, (int)voltage_mV);
		}
	}

	else {
		printf("Invalid command: %s\n", arg1);
		return 0;
	}

	return rval;
}

char *UartHelp[] = {
    "UART SEL",
    "-i [0-4] -t/-r",
    "",
    "Options:",
    "-i uart index",
    "-t tx mode",
    "-r rx mode",
    "",
    "",
    "NOTE: 0=TR, 1=IR_BR1, 2=IR_BR2, 3=IR_BR3, 4=IR_BR4",
    0
};

void __setTrIrSel(int idx, int mode)
{
    unsigned int iodata;
    unsigned int bit;
	
//    struct psa_struct *ppsa;
    struct psa_struct *pdev = getPsaDev();

    if (pdev == NULL)
        return;

    if (idx < 0 || idx > 4) {
        printf("Invalid UART index: %d\r\n", idx);
        printf("UART index range: 0 ~ 4\r\n");
        printf("0=TR, 1=IR_BR1, 2=IR_BR2, 3=IR_BR3, 4=IR_BR4\r\n");
        return;
    }

    bit = 4 + idx;

    iodata = FPGA_inl(pdev->psa_reg_base + 0x00);

    if (mode == 1)
    {

        iodata |= (1U << bit);

        FPGA_outl(pdev->psa_reg_base + 0x00, iodata);

//        if (idx == 0)
//            printf("TR SEL = TX\r\n");
//        else
//            printf("IR BR%d SEL = TX\r\n", idx);
    }
    else
    {

        iodata &= ~(1U << bit);

        FPGA_outl(pdev->psa_reg_base + 0x00, iodata);

 //       if (idx == 0)
 //           printf("TR SEL = RX\r\n");
 //       else
 //           printf("IR BR%d SEL = RX\r\n", idx);
    }
}


void setTrIrSel(struct psa_struct *ppsa, int idx, int mode)
{
    unsigned int iodata;
    unsigned int bit;

    if (ppsa == NULL)
        return;

    if (idx < 0 || idx > 4) {
        printf("Invalid UART index: %d\r\n", idx);
        printf("UART index range: 0 ~ 4\r\n");
        printf("0=TR, 1=IR_BR1, 2=IR_BR2, 3=IR_BR3, 4=IR_BR4\r\n");
        return;
    }

    bit = 4 + idx;

    iodata = FPGA_inl(ppsa->psa_reg_base + 0x00);

    if (mode == 1)
    {

        iodata |= (1U << bit);

        FPGA_outl(ppsa->psa_reg_base + 0x00, iodata);

        if (idx == 0)
            printf("TR SEL = TX\r\n");
        else
            printf("IR BR%d SEL = TX\r\n", idx);
    }
    else
    {

        iodata &= ~(1U << bit);

        FPGA_outl(ppsa->psa_reg_base + 0x00, iodata);

        if (idx == 0)
            printf("TR SEL = RX\r\n");
        else
            printf("IR BR%d SEL = RX\r\n", idx);
    }
}



extern unsigned char Uart_Test;

int Uart(int argc, char *argv[])
{
    struct psa_struct *ppsa;
	struct repeater_struct *prpt;

    int opt;
    int rval = 0;

    int idx = -1;
    int mode = -1;

    ppsa = getPsaDev();


	prpt = getRptDev();

    if (ppsa == NULL)
        return 0;

    while ((opt = getopt(argc, argv, "i:tr")) != -1)
    {
        switch (opt)
        {
        case 'i':
            idx = (int)strtoul(optarg, 0, 0);

            if (idx < 0 || idx > 4) {
                printf("Invalid UART index: %d\r\n", idx);
                printf("UART index range: 0 ~ 4\r\n");
                printf("0=TR, 1=IR_BR1, 2=IR_BR2, 3=IR_BR3, 4=IR_BR4\r\n");
                return 0;
            }
            break;

        case 't':
            mode = 1;
			Uart_Test = 1;

		#if 0
			prpt->tr_pwm = 0xffff;
			prpt->ir_pwm_br[0] = 0xffff;
			prpt->ir_pwm_br[1] = 0xffff;
			prpt->ir_pwm_br[2] = 0xffff;
			prpt->ir_pwm_br[3] = 0xffff;

		    for (int i = 0; i < 4; i++) {
		        setPwmDac(i + 1, (uint16_t)prpt->ir_pwm_br[i]);
		    }
		#endif	
            break;

        case 'r':
            mode = 0;
			Uart_Test = 0;

	#if 0		
			prpt->tr_pwm = 0x07c1;
			prpt->ir_pwm_br[0] = 0x07c1;
			prpt->ir_pwm_br[1] = 0x07c1;
			prpt->ir_pwm_br[2] = 0x07c1;
			prpt->ir_pwm_br[3] = 0x07c1;

			
			for (int i = 0; i < 4; i++) {
				setPwmDac(i + 1, (uint16_t)prpt->ir_pwm_br[i]);
			}
	#endif
			break;

        default:
            return 0;
        }
    }

    if (idx < 0 || idx > 4)
    {
        printf("UART index is required\r\n");
        printf("Usage: uart -i <0-4> -t/-r\r\n");
        printf("0=TR, 1=IR_BR1, 2=IR_BR2, 3=IR_BR3, 4=IR_BR4\r\n");
        return 0;
    }

    if (mode < 0)
    {
        printf("Mode is required\r\n");
        printf("Usage: uart -i <0-4> -t/-r\r\n");
        return 0;
    }

//    setTrIrSel(ppsa, idx, mode);
	__setTrIrSel(idx, mode);

    return rval;
}

void BrPowerSet(int branch, int onoff)
{
    u32 gpio;
    int value;

    if (branch < 1 || branch > BR_COUNT)
    {
        xil_printf("Invalid branch: %d\r\n", branch);
        return;
    }

    gpio = br_pwr_enable_gpio[branch - 1];

    XGpioPs_WritePin(&Gpio, gpio, onoff);

    value = XGpioPs_ReadPin(&Gpio, gpio);

    xil_printf("BR%d POWER %s, GPIO%d READBACK=%d\r\n",
               branch,
               onoff ? "ON" : "OFF",
               gpio,
               value);
}

int BrPowerMonGet(int branch)
{
    if (branch < 1 || branch > BR_COUNT)
        return -1;

    return XGpioPs_ReadPin(&Gpio,
                           br_pwr_mon_gpio[branch - 1]);
}

void BrInfo(void)
{
    int i;
    int enable;
    int mon;

    xil_printf("\r\n");
    xil_printf("Branch Power Status\r\n");
    xil_printf("--------------------\r\n");

    for (i = 0; i < BR_COUNT; i++)
    {
        enable = XGpioPs_ReadPin(&Gpio,
                                 br_pwr_enable_gpio[i]);

        mon = XGpioPs_ReadPin(&Gpio,
                              br_pwr_mon_gpio[i]);

        xil_printf("BR%d : ENABLE=%s   PWR_MON=%s\r\n",
                   i + 1,
                   enable ? "ON " : "OFF",
                   mon ? "ON " : "OFF");
    }

    xil_printf("\r\n");
}

char *BrHelp[] = {
    "BR",
    "-c <1-4> power <on|off>",
    "info",
    "Options:",
    "-c branch number",
    "power on    Enable branch power",
    "power off   Disable branch power",
    "info        Display branch power monitor status",
    "",
    "NOTE: Branch 1~4",
    0
};

int Br(int argc, char *argv[])
{
    int opt;
    int branch = -1;

    if (argc >= 2 && strcmp(argv[1], "info") == 0)
    {
        BrInfo();
        return 0;
    }

    while ((opt = getopt(argc, argv, "c:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            branch = (int)strtoul(optarg, NULL, 0);

            if (branch < 1 || branch > BR_COUNT)
            {
                xil_printf("Invalid branch: %d\r\n", branch);
                xil_printf("Branch range: 1 ~ 4\r\n");
                return 0;
            }
            break;

        default:
            return 0;
        }
    }

    if (branch < 1 || branch > BR_COUNT)
    {
        xil_printf("Usage:\r\n");
        xil_printf("  br -c <1-4> power on\r\n");
        xil_printf("  br -c <1-4> power off\r\n");
        xil_printf("  br info\r\n");
        return 0;
    }

    if (optind + 1 >= argc)
    {
        xil_printf("Usage: br -c <1-4> power <on|off>\r\n");
        return 0;
    }

    if (strcmp(argv[optind], "power") != 0)
    {
        xil_printf("Unknown command: %s\r\n", argv[optind]);
        return 0;
    }

    if (strcmp(argv[optind + 1], "on") == 0)
    {
        BrPowerSet(branch, 1);
    }
    else if (strcmp(argv[optind + 1], "off") == 0)
    {
        BrPowerSet(branch, 0);
    }
    else
    {
        xil_printf("Invalid power state: %s\r\n",
                   argv[optind + 1]);
        xil_printf("Use: on / off\r\n");
    }

    return 0;
}

int InitBr(void)
{
    int i;

    for (i = 1; i <= BR_COUNT; i++)
    {
        BrPowerSet(i, 1);
    }

    return XST_SUCCESS;
}
