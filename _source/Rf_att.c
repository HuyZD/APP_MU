#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "genlib.h"
#include "stddefs.h"
#include "psa.h"
#include "rf.h"

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
#define	ATT_CTRL	0x00
#define	ATT_DATA	0x0018

#define	ATT_INIT	0x0080
#define	ATT_DONE	0x0200
#define	ATT_SEL		0xF000

/*	setRfAttn()
 *  band : 0->700M, 1->900M, 2->1.7G, 3->1.8G
 *    id : 해당 Band의 Atten 식별자.
 * 			0->alc_d,
 * 			1->alc_u,
 * 			2->agc_u,	(HOME RF Repeater에는 없음)
 * 	 att : 0.01dB Reolution, 100배 곱한 값을 적용.
 */
void setRfAttn(uchar band, uchar id, short data)
{
	int 		i;
	char 		byte,val;

	val = data/50;
    if (val > 63)
       val = 63;
//	val &= 0x3f;

//	printf("init_att[%d][%d]\n",data,val);

	ATT_ClkWrite(0);
	if(band == 0)
		ATT1_EnWrite(id, 0);
	else 
		ATT2_EnWrite(id, 0);
		
	ATT_DataWrite(0);
      
//	byte = (char)data;   
	byte = (char)val;   
	for (i = 5; i >=0; i--) {
	    /* Clock low */      
		ATT_ClkWrite(0);
	    /* HMC624 Hittite 제품인 경우 데이터가 토클임.*/
	    //RF_ATT_DATA_En(pdev, ((byte >> i) & 0x1) ^ 0x1);
	    /* PE 4302 사용, 0인 경우 데이타 All High */
		ATT_DataWrite((byte >> i) & 0x1);
	    /* Clock high */     
		ATT_ClkWrite(1);
	}      
	ATT_ClkWrite(0);
	ATT_DataWrite(0);
	
	if(band == 0)
	{
		ATT1_EnWrite(id, 1);
		ATT1_EnWrite(id, 0);
	}
	else
	{
		ATT2_EnWrite(id, 1);
		ATT2_EnWrite(id, 0);
	}

#if 0
	int 				i,idx;
	unsigned int		ioaddr,ctrl,val;
    struct psa_struct 	*ppsa;

	val = data/50;
    if (val > 63)
        val = 63;

	val &= 0x3f;

	ppsa = getPsaDev();
	ctrl = FPGA_inl(ppsa->psa_reg_base + ATT_CTRL);
	ctrl &= ~ATT_INIT;
	FPGA_outl(ppsa->psa_reg_base + ATT_CTRL, ctrl);

    FPGA_outl(ppsa->psa_reg_base+ATT_DATA, val);
	ctrl |= ATT_INIT;
	ctrl &= ~ATT_SEL;
	ctrl |= ((band*2+id) << 12);
	FPGA_outl(ppsa->psa_reg_base + ATT_CTRL, ctrl);
	for (i=0;i<64;i++) {
		ctrl = FPGA_inl(ppsa->psa_reg_base + ATT_CTRL);
		if (ctrl & ATT_DONE) {
			break;
		}
	}
	ctrl &= ~ATT_INIT;
	FPGA_outl(ppsa->psa_reg_base + ATT_CTRL, ctrl);
	if (i == 64) {
		printf("PLL Init Failed\n");
		return;
	}
#endif	
}

void init_att(){
	setRfAttn(0,0,300);
	setRfAttn(0,1,300);
	setRfAttn(1,0,300);
	setRfAttn(1,1,300);
	
	printf("init_att\n");
}


char *AttHelp[] = {
    "Att",
    "-[v:i:du] ",
    "",
    "Options:",
    "-v value",
    "-i path id",
    "-d downlink",
    "-u uplink",
    "",
    "Operations:",
    "att -d -v 100 set -> DL DNC  1dB, aten",
    "att -u -v 100 set -> UL DNC  1dB, aten",
    "att -d -v 100 set -> UL UPC1 1dB, aten",
    "att -u -v 100 set -> UL UPC2 1dB, aten",
    "",
    "NOTE: none",
    0
};

int Att(int argc, char *argv[])
{
    char 			*arg1;
    int 			opt;
	int 			val=0, id=0;
	int 			path = 0;
	short			attn;
	struct ics_struct		*idev;

    if (argc < 2) {
#if DEBUG_PRINT
        printf("No arguments, exiting... use help\n");
#endif
        return(0);
    }
	while((opt=getopt(argc,argv,"v:i:du")) != -1) {
        switch (opt) {
            case 'd':			/* DL ALC */
				path = 0;
                break;
            case 'u':			/* UL ALC	*/
				path = 1;
                break;
            case 'i':			/* UL ALC	*/
				path = (int)strtoul(optarg, 0, 0);
                break;
            case 'v':
            	val = (int)strtoul(optarg, 0, 0);
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
            if (tmpband < 2) {
               	CurrentBand = tmpband;
           		printf("Subsequent Att ops apply to band %s(%d)\n",band_name[CurrentBand],CurrentBand);
			}
        }
        else {
           	printf("Current Band %s(%d)\n",band_name[CurrentBand],CurrentBand);
        }
	}
	else if (strcmp(arg1, "set") == 0) {
		setRfAttn(CurrentBand, path, val);
	}
	else if(strcmp(arg1, "on") == 0) {
		if(val == 0)
			ATT1_EnWrite(0, 1);
		else if(val == 1)
			ATT1_EnWrite(1, 1);
		else if(val == 2)
			ATT2_EnWrite(0, 1);
		else if(val == 3)
			ATT2_EnWrite(1, 1);
		else if(val == 4)
		{
			ATT1_EnWrite(0, 1);
			ATT1_EnWrite(1, 1);
			ATT2_EnWrite(0, 1);
			ATT2_EnWrite(1, 1);
		}
       	printf("Att On(%d)\n",val);

	}
	else if(strcmp(arg1, "off") == 0)
	{
		if(val == 0)
			ATT1_EnWrite(0, 0);
		else if(val == 1)
			ATT1_EnWrite(1, 0);
		else if(val == 2)
			ATT2_EnWrite(0, 0);
		else if(val == 3)
			ATT2_EnWrite(1, 0);
		else if(val == 4)
		{
			ATT1_EnWrite(0, 0);
			ATT1_EnWrite(1, 0);
			ATT2_EnWrite(0, 0);
			ATT2_EnWrite(1, 0);
		}
       	printf("Att Off(%d)\n",val);
	}


	return (0);
}

