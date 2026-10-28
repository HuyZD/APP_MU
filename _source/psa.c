/*	psa.c
 */
//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "psa.h"
#include "xil_io.h"
#include "xil_cache.h"
#include "xparameters.h"


//#define	BAND0						0		//	Band0
//#define	BAND1						1		//	Band1
//#define	BAND2						2		//	Band2

static int CurrentChan = 0;		/* FWD_IN=0, FWD_OUT=1, RVS_IN=2, RVS_OUT=3*/

//#if INCLUDE_ICS
//
//extern char fs_buff[];
//
static ushort 	Ch =   0;
static ushort 	Xs =   0;
////static ushort 	Xe = 799;
static ushort 	Xe = 1023;
////static ushort 	Fs= 6144;				/* Sampling Rate * 100*/
//
////0xaa 10101010 : 7 48
////0x6a 01101010 : 7 42
////0x5a 01011010 : 6 36
////0x56 01010110 : 5 30
////0x55 01010101 : 4 24
////0x15 00010101 : 3 18
////0x05 00000101 : 2 12
//
//extern char *showfloat(char *buf, short val);

struct psa_struct psa_dev;

struct psa_struct *getPsaDev(void)
{
	return (&psa_dev);
}

///* psaChpwr()
// * 1024의 샘플중에 800샘플만 취한다. 
// */
short psaChpwr(ushort Xs, ushort Xe)
{
	unsigned short 	Xstart,Xend;
	unsigned int 	ioaddr,iodata;
	short			chpwr;
	short			i;
	
//	Xstart = Xs + 112;
//	Xend = Xe + 112;
	Xstart = Xs;
	Xend = Xe;
	
	ioaddr = psa_dev.psa_ram_base;
	for (i=Xstart,iodata=0;i<Xend;i++) {
		iodata += FPGA_inl(ioaddr + i*4);
	}
	chpwr = cal_power(iodata);
	chpwr += psa_dev.offset[CurrentChan];
	return chpwr;
}

short psaPapr(void)
{
	unsigned int	ioaddr;
	int 			mean,peak;
	
	//ioaddr = psa_dev.psa_reg_base + PEAK_PWR;
	//peak = FPGA_inl(ioaddr);
	//ioaddr = psa_dev.psa_reg_base + MEAN_PWR;
	//mean = FPGA_inl(ioaddr);
	return (cal_papr(peak,mean));
}

void setAntSwitch(int	onoff)
{
    struct psa_struct 	*pdev;
	unsigned int	ioaddr,ctrl;

	pdev = getPsaDev();
	
	ioaddr = pdev->psa_reg_base + 0x00;
	ctrl = FPGA_inl(ioaddr);
//	printf("Ctrl1:%08X\n",ctrl);
	
	if(onoff)
		ctrl |= 0x80000000;
	else
		ctrl &= ~0x80000000;

//	printf("Ctrl2[%08X]:%08X[%d]\n",ioaddr,ctrl,onoff);
    FPGA_outl(ioaddr,ctrl);
}

//int getAntSwitch(void)
//{
//	unsigned int	ioaddr;
//	short 			iodata;
//	
//	ioaddr = psa_dev.psa_reg_base + 0x01;
//	iodata = FPGA_inw(ioaddr);
//	
//	return (iodata & 0x0001);
//}
//
//
///* 
// * loadPsaInfo()
// * Filesystem으로 부터 중계기 정보를 읽어온다. 
// */
//int loadPsaInfo(struct psa_struct *pdev)
//{
//	int						i,fd;
//	struct psa_info 		pinfo;
//
//	fd = mon_tfsopen("psa", TFS_RDONLY, NULL);
//	if (fd < 0) {
//#if 0
//		printf("%s:%s\n",(char *)mon_tfsctrl(TFS_ERRMSG,fd,0),"psa"); 
//#endif		
//		return (-1);
//	}
//	mon_tfsread(fd, (char *)(&pinfo), sizeof(struct psa_info));
//	mon_tfsclose(fd,NULL);
//
//    for(i=0;i<12;i++) {
//		pdev->offset[i]		= pinfo.offset[i];
//    }
//    return (1);
//}
//
///* 
// * savePsaInfo()
// * Filesystem으로 부터 중계기 정보를 저장한다.
// */
//int savePsaInfo(struct psa_struct *pdev)
//{
//	int						i,fd;
//	struct psa_info 		pinfo;
//
//    for(i=0;i<12;i++) {
//    	pinfo.offset[i]			= pdev->offset[i];
//    }
//                                                                
//    /* Store To File System */
//	fd = mon_tfsopen("psa", TFS_CREATE, (char *)fs_buff);
//	if (fd < 0) {
//#if 0
//		printf("%s\n",(char *)mon_tfsctrl(TFS_ERRMSG,fd,0)); 
//#endif		
//		mon_tfsunlink("psa");
//		fd = mon_tfsopen("psa", TFS_CREATE, (char *)fs_buff);
//	}
//	mon_tfswrite(fd, (char *)&pinfo, sizeof(struct psa_info));
//	mon_tfsclose(fd,NULL);
//	return (1);
//}
//
///* 
// * savePsaInfoDef()
// * Filesystem에 파일이 존재하지 않을 경우 중계기 설정의 기본값을 저장한다 .
// */
//int savePsaInfoDef(struct psa_struct *pdev)
//{
//	int i;
//    for(i=0;i<12;i++) {
//    	pdev->offset[i]			= 0;
//    }
//	return (savePsaInfo(pdev));
//}
char syslock[4];

int init_psa(void)
{
    struct psa_struct 	*pdev;

	pdev = getPsaDev();
	pdev->psa_reg_base = XPAR_EMIF_BASEADDR + 0xF0000;
	pdev->psa_reg_size = 32;
	pdev->psa_ram_base = XPAR_EMIF_BASEADDR + 0xF8000;
	pdev->psa_ram_size = 1024;

	printf("Initialize PSA!!\n");
	////////////////// 파일에서 읽어들여야 하는 것들 //////////////////
	//if (loadPsaInfo(&psa_dev) < 0) {
	//	savePsaInfoDef(&psa_dev);
	//}
	
	//setAntSwitch(1);

	return 0;
}

///* confPsaChpwr()
// * Volatile 정보를 설정한다. 
// * 1. Path     	: Tx/Rx
// * 2. Channel	: In/Out
// * 3. Xstart	: 채널 파워 구간의 시작점
// * 4. Xend		: 채널 파워 구간의 끝점
// */
void confPsaChpwr(unsigned char Channel, short Xstart, short Xend)
{
	unsigned int ioaddr,iodata;
	
	//if (Channel < 0 || Channel > MAX_PSA_CHAN)
	//	return;
	//if (Xstart < 0 || Xstart > 799)
	//	return;
	//if (Xend < 0 || Xend > 799 || Xend < Xstart)
	//	return;
	//	
	//Xs = Xstart;
	//Xe = Xend;
	//CurrentChan = Channel%8;
	//ioaddr = psa_dev.psa_reg_base;
	//iodata = FPGA_inl(ioaddr);
	//iodata &= ~0xf;
	//iodata |= CurrentChan;
	//FPGA_outl(ioaddr, iodata);

	if(Channel == 4) Channel = 0;
	else if(Channel == 5) Channel = 1;
	else if(Channel == 6) Channel = 2;
	else if(Channel == 7) Channel = 3;

	ioaddr = psa_dev.psa_reg_base + PSA_CFG0;
	iodata = FPGA_inl(ioaddr);
	iodata &= ~0xf;
	iodata |= Channel;
	FPGA_outl(ioaddr,iodata);
	//psa_dev.psa_ram_base = 0xF0000 + (Channel/4)*4096;

	//return 0;
}

///* save_psa()
// * Nonvolatile 정보를 설정한다. 
// * 1. Path     	: Tx/Rx
// * 2. Channel	: In/Out
// * 3. Offset	: 채널 파워 옵셋
// */
//void confPsaOffst(char channel, short offset)
//{
//	if(channel < 0 || channel > 3)
//		return;
//
//	psa_dev.offset[channel] = offset;
//
//	savePsaInfo(&psa_dev);
//}

int getPsa(char *buf)
{
	int				len=0;
	int				sa;
	int				i;
	unsigned short	swap;
	unsigned int 	ioaddr,iodata;
//	Ch = 0x12;

//	memcpy((void *)&buf[len], &Ch, 1); len+=1;					/* Channel             	*/
//	memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;		/// error 

	//swap = swap2(Fs);
	//memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;		/* Fs : Sampling Rate	*/

	//swap = swap2(Xs);
	//memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;		/* Xs					*/
    //
	//swap = swap2(Xe);
	//memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;		/* Xe					*/
	
//	swap = swap2(psa_dev.offset[CurrentChan]);
	swap = (psa_dev.offset[CurrentChan]);
//	swap = 0x7788;
//	printf("1 offset[%d]\n",psa_dev.offset[CurrentChan]);
	memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;		/* Offset				*/
    
//	swap = swap2(psaPapr());
	swap = (psaPapr());
//	swap = 0x3344;
//	printf("2 psaPapr[%d]\n",psaPapr());
 	memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;		/* Papr					*/
    
//	swap = swap2(psaChpwr(Xs,Xe));
	swap = (psaChpwr(Xs,Xe));
//	swap = 0x5566;
//	printf("3 psaChpwr[%d]\n",psaChpwr(Xs,Xe));
	memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;		/* Channel Power		*/
//	printf("0 getPsa[%d]\n",len);

	ioaddr = psa_dev.psa_ram_base;
	for (i=0;i<1024;i++) {
		iodata = FPGA_inl(ioaddr + i*4);		
		sa = cal_power(iodata);
		sa += psa_dev.offset[CurrentChan];
		swap = swap2(sa);
		memcpy((void *)&buf[len], (void *)&swap, 2); len+=2;	/* SA Data				*/
	}
	
//	printf("1 getPsa[%d]\n",len);
	return len;
}

static void showPsaReg(unsigned int reg_base, unsigned int reg_size)
{
#if DEBUG_PRINT
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
#endif
}
static void showPsaRam(unsigned int ram_base, unsigned int ram_size)
{
    int i;
	
    for (i=0;i<ram_size;i++) {
    	if (i%8 == 0) {
    		printf("0x%08X: ", (ram_base+i*4));
    	}

	    printf("%08x ", FPGA_inl(ram_base+i*4));

    	if (i%8 == 7) {
    		printf("\n");
    	}
    }
}

char *PsaHelp[] = {
    "Psa",
    "-[c:] -[v:] -[p:]",
    "",
    "Options:",
    "-c channel  specifiy fa 1 ~ 4",
    "-v value    specifiy value",
    "",
    "Operations:",
    "init, on, off, fa, coe, atn, alc, info, reg",
    "",
    "NOTE: none",
    0
};

int 
Psa(int argc, char *argv[])
{
    char *arg1,*arg2;
    int opt;
    int rval=0;
	int ival,fval;
    int i;
	unsigned int ioaddr,iodata;
	struct psa_struct *pdev;
	int val=0,center,span;

    while((opt=getopt(argc,argv,"v:c:s:")) != -1) {
        switch (opt) {
            case 'v':
                val = (int)strtoul(optarg,0,0);
                break;
            case 'c':
                center = (int)strtoul(optarg,0,0);
                break;
            case 's':
                span = (int)strtoul(optarg,0,0);
                break;
            default:
                return(0);
        }
    }
    
    arg1 = argv[optind];
    arg2 = argv[optind+1];

	pdev = getPsaDev();
    
	if (strcmp(arg1, "init") == 0) {
	}
    else if (strcmp(arg1, "reg") == 0) {
		showPsaReg(pdev->psa_reg_base,pdev->psa_reg_size);
	}
    else if (strcmp(arg1, "ram") == 0) {
		showPsaRam(pdev->psa_ram_base,pdev->psa_ram_size);
	}
    else if (strcmp(arg1, "res") == 0) {
		ioaddr = pdev->psa_reg_base + 0x40 + CurrentChan*4;
    	iodata = FPGA_inl(ioaddr);
    	iodata &= ~0xff0000;
    	iodata |= (val << 16);
    	FPGA_outl(ioaddr,iodata);
    }
    else if (strcmp(arg1, "fc") == 0) {
    	double 	fc;
    	int		start;
    	
		if (strcmp(arg2, "set") == 0) {
    		start = center - span/2;
    		fc = (double)(start)/10.0;
    		start = (int)((fc/307.2)*65536.0);
    		start &= 0xffff;
    		printf("0x%04x\n",start);
    		
			ioaddr = pdev->psa_reg_base + 0x40 + CurrentChan*4;
    		iodata = FPGA_inl(ioaddr);
    		iodata &= ~0xffff;
    		iodata |= start;
    		FPGA_outl(ioaddr,iodata);
    	}
    }
    else if (strcmp(arg1, "chan") == 0) {
        int tmpchan;
        if (argc == 3) {
            tmpchan = atoi(arg2);
            if (tmpchan < 16) {
                CurrentChan = tmpchan;
				ioaddr = pdev->psa_reg_base + PSA_CFG0;
				iodata = FPGA_inl(ioaddr);
				iodata &= ~0xf;
				iodata |= CurrentChan;
				FPGA_outl(ioaddr,iodata);
				//pdev->psa_ram_base = 0xF0000 + (CurrentChan/4)*4096;

			}
            printf("Subsequent psa ops apply to channel #%d[0x%08X]\n",CurrentChan,pdev->psa_ram_base);
        }
        else {
            printf("Current psa channel: %d\n",CurrentChan);
        }
	}
    //else if (strcmp(arg1, "papr") == 0) {
	//	int mean,peak;
	//	
	//	ioaddr = pdev->psa_reg_base + PEAK_PWR;
	//	peak = FPGA_inl(ioaddr);
	//	ioaddr = pdev->psa_reg_base + MEAN_PWR;
	//	mean = FPGA_inl(ioaddr);
	//	ival = cal_power(peak);
	//	fval = abs(ival)%100;
	//	ival = ival/100;
	//	printf("Peak : %4d.%02d[dBm]\n",ival,fval);
	//	ival = cal_power(mean);
	//	fval = abs(ival)%100;
	//	ival = ival/100;
	//	printf("Mean : %4d.%02d[dBm]\n",ival,fval);
	//	ival = cal_papr(peak,mean);
	//	fval = abs(ival)%100;
	//	ival = ival/100;
	//	printf("PAPR : %4d.%02d[dB]\n",ival,fval);
    //}
    else if (strcmp(arg1, "plot") == 0) {
		ioaddr = pdev->psa_ram_base;
		for (i=0;i<1024;i++) {
			iodata = FPGA_inl(ioaddr + i*4);
			ival = cal_power(iodata);// - 5419 + offset;		/*20log(2^9)=54.185*/
			//ival = -5419;		/*20log(2^9)=54.185*/
			//ival = (int)(20*log10(512)*100);		//=54.185*/
			fval = abs(ival)%100;
			ival = ival/100;
			printf("%4d.%02d ",ival,fval);
		}
		printf("\n");
	}
    else if (strcmp(arg1, "ant") == 0) {
		if (strcmp(arg2, "ext") == 0) {
			setAntSwitch(0);
		}
		else if (strcmp(arg2, "int") == 0) {
			setAntSwitch(1);
		}
	}
    //else if (strcmp(arg1, "scale") == 0) {
	//	ioaddr = pdev->psa_reg_base + 0x2;
	//	iodata = FPGA_inw(ioaddr);
	//	iodata &= ~0x3FF;
	//	iodata |= val;
    //	FPGA_outw(ioaddr, iodata);
    //}
    else if (strcmp(arg1, "info") == 0) {
		char buf[32];
    
    	//printf("Channel     : %d\n",CurrentChan);
    	//printf("PSA Xs      : %d\n",Xs);
    	//printf("PSA Xe      : %d\n",Xe);
    	//printf("PSA Chpwr   : %sdBm\n",showfloat(buf, psaChpwr(Xs,Xe)));
    	//printf("Ch0 Offset  : %sdBm\n",showfloat(buf, pdev->offset[0]));
    	//printf("Ch1 Offset  : %sdBm\n",showfloat(buf, pdev->offset[1]));
    	//printf("Ch2 Offset  : %sdBm\n",showfloat(buf, pdev->offset[2]));
    	//printf("Ch3 Offset  : %sdBm\n",showfloat(buf, pdev->offset[3]));
    }
    
    return (rval);
}
//#else
//int getPsa(char *buf)
//{
//	return (0);
//}
//int confPsaChpwr(char Channel, short Xstart, short Xend)
//{
//	return (1);
//}
//int savePsaInfo(struct psa_struct *pdev)
//{
//	return (1);
//}
//#endif
