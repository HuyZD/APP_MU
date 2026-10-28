#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "genlib.h"
#include "stddefs.h"
#include "psa.h"
#include "rf.h"
#include "tbl.h"
#include "repeater.h"
#include "flash.h"

struct hpa_info_struct		hinfo;

//	Shift Register Bit Field
static unsigned int pam_mask[2][2] = {{0x00000040,0x00000080},{0x00004000,0x00008000}};

extern int	CurrentBand;
extern char *OnOff[2];
char *DlUl[2] = {"DL","UL"};


void setHpaOnOff(uchar band, uchar path, uchar onoff)
{
	unsigned int		ioaddr,iodata;
    struct psa_struct 	*ppsa;
	char 				idx = 0;
	unsigned int 		ctrl;

	ppsa = getPsaDev();

	ioaddr = ppsa->psa_reg_base+0x0C;

    ctrl = FPGA_inl(ioaddr);

	idx = band*2 + path%2;
	if(idx ==1)
	{
		iodata = (onoff == 0) ? (ctrl | (0x1 << idx)) : (ctrl & ~(0x1 << idx));
	}
	else
	{
		iodata = (onoff == 1) ? (ctrl | (0x1 << idx)) : (ctrl & ~(0x1 << idx));
	}
	
//	printf("setHpaOnOff[%d][%d]\n",idx,iodata);
	
  //  iodata = FPGA_inl(ioaddr);
  //  iodata = (onoff) ? (iodata | pam_mask[band][path]) : (iodata & ~pam_mask[band][path]);
    FPGA_outl(ioaddr,iodata);
}

void confFwdHpaOn(int band, int val)
{
	struct ics_struct		*idev;   
	idev = getIcsDev(band); 

	if(val)
	{
		setHpaOnOff(band, 0, 1); //Active high(on)
		setIcsDacOnOff(idev,DL,1);
	}
	else
	{
		setHpaOnOff(band, 0, 0); //off
		setIcsDacOnOff(idev,DL,0);
	}
}

void confRvsHpaOn(int band, int val)
{
	struct ics_struct		*idev;   
	idev = getIcsDev(band); 

	if(val)
	{
		setHpaOnOff(band, 1, 1); //Active high(on)
		setIcsDacOnOff(idev,UL,1);
	}
	else
	{
		setHpaOnOff(band, 1, 0); //off
		setIcsDacOnOff(idev,UL,0);
	}
}

void init_hpa(void)
{
	/*	CPU GPIO�� ����� GPIO �ʱ�ȭ ��
		FPGA I/O�� �����ϱ� ������ �ʿ����.
	*/

	setHpaOnOff(0,0,_On);
	setHpaOnOff(0,1,_On);  /// UL AMP ON 1
	setHpaOnOff(1,0,_On);
	setHpaOnOff(1,1,_On);

	printf("setHpaOnOff\n");
}
 

extern void saveHpaTbl(struct hpa_info_struct *tinfo);
extern void loadHpaTbl(struct hpa_info_struct *tinfo);


int loadAmpInfo(struct repeater_struct *pdev)
{
	
}

int SaveAmpInfo(struct repeater_struct *pdev)
{
	
}


char *HpaHelp[] = {
    "Hpa",
    "-[frdu]",
    "",
    "Options:",
    "-f or -d forward/downlink path",
    "-r or -u reverse/uplink path",
    "",
    "Operations:",
    "hpa -f on/off, hpa -d on/off, hpa on",
    "hpa -r on/off, hpa -u on/off, hpa off",
    "NOTE: none",
    0
};

int
Hpa(int argc, char *argv[])
{
    char 					*arg1;
    int 					opt;
	int						path=0;

	struct repeater_struct *prpt;
	prpt = getRptDev();

    if (argc < 2) {
        printf("No arguments, exiting... use help\n");
        return(0);
    }

    while((opt=getopt(argc,argv,"du")) != -1) {
        switch (opt) {
            case 'd':					/* Forward 	*/
				path = 0;
                break;
            case 'u':					/* Reverse 	*/
				path = 1;
                break;
            default:
                return(0);
        }
    }

    arg1 = argv[optind];

    if (strcmp(arg1, "band") == 0) {
        int tmpband;
        if (argc == 3) {
            tmpband = atoi(argv[2]);
            if (tmpband < 4) {
                CurrentBand = tmpband;
            	printf("Subsequent Hpa ops apply to Band %s(%d)\n",band_name[CurrentBand],CurrentBand);
			}
        }
        else {
           	printf("Current Band %s(%d)\n",band_name[CurrentBand],CurrentBand);

        }
	}
	else if (strcmp(arg1, "on") == 0) {
		setHpaOnOff(CurrentBand,path,_On);
		printf("%s %s Amp %s\n",band_name[CurrentBand],DlUl[path],OnOff[1]);
//		printf("AmpOn [%d][%d]\n",CurrentBand,path);

	}
	else if (strcmp(arg1, "off") == 0) {
		setHpaOnOff(CurrentBand,path,_Off);
		printf("%s %s Amp %s\n",band_name[CurrentBand],DlUl[path],OnOff[0]);
	}
	else if (strcmp(arg1, "save") == 0) {
		printf("Hpa Save \n");
//		SaveAmpInfo(prpt);
 	}
	else if (strcmp(arg1, "load") == 0) {
		printf("Hpa load \n");
//		SaveAmpInfo(prpt);
	}
 	
	return 0;
}



