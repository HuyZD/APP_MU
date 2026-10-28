#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "ics_private.h"
#include "psa.h"

#define PI   3.1415926535897932

extern void milliseconds_sleep(unsigned long mSec);
/* isIcsBandAlive()
 * SB가 활성화 되어 있는지 확인한다. 
 */
uchar isIcsBandAlive(struct ics_struct *pdev)
{
	unsigned int 		ioaddr;
	unsigned int 		iodata;
	
	ioaddr = pdev->ics_reg_base[DL] + ICS_VERSION;
	iodata = FPGA_inl(ioaddr);
	iodata &= 0xffff;

	return ((iodata == 0xdead || iodata == 0x0) ? 0:1);
}

/* getIcsVersion()
 * Version은 모든 Band의 모든 SB가 동일하다. 
 * 아래 Loop는 맨 마지막 유효한 band의 유효한 sb에 대한 버전을 반환한다. 
 */
unsigned int getIcsVersion(void)
{
	struct ics_struct 	*pdev;
    unsigned int 		band,sb;
    unsigned int 		ioaddr;
    unsigned int 		iodata;

  	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (isIcsBandAlive(pdev)) {
    		ioaddr = pdev->ics_reg_base[DL] + ICS_VERSION;
    		iodata = FPGA_inl(ioaddr);
    		break;
    	}
	}
    return (iodata);
}

static struct band_info band_info[51] = {
	{ 1,	"700M",	 'l', "   LTE05M", 0x0001, 0, 0,"700M_L05M"},      		//	1
	{ 2,	"700M",	 'l', "   LTE10M", 0x0003, 0,10,"700M_L10M"},     		//	2
	{ 3,	"700M",	 'l', "   LTE15M", 0x0007, 0,11,"700M_L15M"},     		//	3
	{ 4,	"700M",	 'l', "   LTE20M", 0x000F, 0,12,"700M_L20M"},     		//	4
	{ 5,	"700M",	 'e', "   PRE05M", 0x0001, 1, 0,"700M_L05M"},     		//	5
	{11,	"800M",	 'l', "   LTE05M", 0x0001, 0, 0,"800M_L05M"},      		//	5
	{12,	"800M",	 'l', "   LTE10M", 0x0003, 0,10,"800M_L10M"},     		//	6
	{13,	"800M",	 'l', "   LTE15M", 0x0007, 0,11,"800M_L15M"},     		//	7
	{14,	"800M",	 'l', "   LTE20M", 0x000F, 0,12,"800M_L20M"},     		//	8
	{21,	"900M",	 'l', "   LTE05M", 0x0001, 0, 0,"900M_L05M"},      		//	9
	{22,	"900M",	 'l', "   LTE10M", 0x0003, 0,10,"900M_L10M"},     		//	10
	{23,	"900M",	 'l', "   LTE15M", 0x0007, 0,11,"900M_L15M"},     		//	11
	{24,	"900M",	 'l', "   LTE20M", 0x000F, 0,12,"900M_L20M"},     		//	12
	{25,	"900M",	 'w', " WCDMA1FA", 0x0008, 1, 0,"900M_W1FA"},      		//	13 /*SBM & Left:W RightL*/  
	{26,	"900M",	 'w', " WCDMA2FA", 0x000C, 2, 0,"900M_W2FA"},      		//	14 /*SBM & Left:W RightL*/  
	{27,	"900M",	 'w', " WCDMA3FA", 0x0007, 3, 0,"900M_W3FA"},      		//	15 /*SBM & Left:W RightL*/  
	{28,	"900M",	 'w', " WCDMA4FA", 0x000F, 4, 0,"900M_W4FA"},      		//	16 /*SBM & Left:W RightL*/  
	{31,	"1.8G",	 'l', "   LTE05M", 0x0001, 0, 0,"1.8G_L05M"},      		//	17
	{32,	"1.8G",	 'l', "   LTE10M", 0x0003, 0,10,"1.8G_L10M"},     		//	18
	{33,	"1.8G",	 'l', "   LTE15M", 0x0007, 0,11,"1.8G_L15M"},     		//	19
	{34,	"1.8G",	 'l', "   LTE20M", 0x000F, 0,12,"1.8G_L20M"},     		//	20
	{39,	"1.8G",	 'l', "   FULL75", 0x000F, 0,12,"2.1G_F75M"},     		//	25
	{41,	"2.1G",	 'l', "   LTE05M", 0x0001, 0, 0,"2.1G_L05M"},      		//	21
	{42,	"2.1G",	 'l', "   LTE10M", 0x0003, 0,10,"2.1G_L10M"},     		//	22
	{43,	"2.1G",	 'l', "   LTE15M", 0x0007, 0,11,"2.1G_L15M"},     		//	23
	{44,	"2.1G",	 'l', "   LTE20M", 0x000F, 0,12,"2.1G_L20M"},     		//	24
	{45,	"2.1G",	 'l', "   LTE25M", 0x000F, 0,12,"2.1G_L25M"},     		//	25
	{49,	"2.1G",	 'l', "   FULL60", 0x000F, 0,12,"2.1G_F60M"},     		//	25
	{51,	"1.9G",	 'l', "   LTE05M", 0x0001, 0, 0,"1.9G_L05M"},      		//	29
	{52,	"1.9G",	 'l', "   LTE10M", 0x0003, 0, 0,"1.9G_L10M"},     		//	30
	{53,	"1.9G",	 'l', "   LTE15M", 0x0007, 0, 0,"1.9G_L15M"},     		//	31
	{54,	"1.9G",	 'l', "   LTE20M", 0x000F, 0, 0,"1.9G_L20M"},     		//	32
	{55,	"1.9G",	 'p', "   PCS05M", 0x0007, 0, 0,"1.9G_P05M"},      		// 	33
	{56,	"1.9G",	 'p', "   PCS10M", 0x007F, 0, 0,"1.9G_P10M"},     		//	34
	{57,	"1.9G",	 'p', "   PCS15M", 0x07FF, 0, 0,"1.9G_P15M"},     		//	35
	{58,	"1.9G",	 'p', "   PCS20M", 0x7FFF, 0, 0,"1.9G_P20M"},     		//	36
	{61,	"AWS21", 'l', "   LTE05M", 0x0001, 0, 0,"AWS21_L05"},      		//	37
	{62,	"AWS21", 'l', "   LTE10M", 0x0003, 0,10,"AWS21_L10"},     		//	38
	{63,	"AWS21", 'l', "   LTE15M", 0x0007, 0,11,"AWS21_L15"},     		//	39
	{64,	"AWS21", 'l', "   LTE20M", 0x000F, 0,12,"AWS21_L20"},     		//	40
	{71,	"AWS70", 'l', "   LTE05M", 0x0001, 0, 0,"AWS70_L05"},      		//	41
	{72,	"AWS70", 'l', "   LTE10M", 0x0003, 0,10,"AWS70_L10"},     		//	42
	{73,	"AWS70", 'l', "   LTE15M", 0x0007, 0,11,"AWS70_L15"},     		//	43
	{74,	"AWS70", 'l', "   LTE20M", 0x000F, 0,12,"AWS70_L20"},     		//	44
	{81,	"TDD35", 'n', "   NR100M", 0x0001, 0, 0,"3.5G_N100"},      		//	45
	{82,	"TDD35", 'n', "   NR_90M", 0x0001, 0, 0,"3.5G_N_90"},      		//	46
	{83,	"TDD35", 'n', "   NR_80M", 0x0001, 0, 0,"3.5G_N_80"},      		//	47
	{84,	"TDD35", 'n', "   NR_60M", 0x0001, 0, 0,"3.5G_N_60"},      		//	48
	{85,	"TDD35", 'n', "   NR_50M", 0x0001, 0, 0,"3.5G_N_50"},      		//	49
	{86,	"TDD35", 'n', "   NR_40M", 0x0001, 0, 0,"3.5G_N_40"},      		//	50
};

#define	N	sizeof(band_info)/sizeof(struct band_info)

uchar getIcsBandType(struct ics_struct *pdev, int *index, char **band, char *type, char **desc, ushort *firsel, uchar *fa, uchar *nco, char **name)
{
	unsigned int 	ioaddr;
	unsigned int 	iodata;
	int 			x;
	
	ioaddr = pdev->ics_reg_base[UL] + ICS_VERSION;
	iodata = FPGA_inl(ioaddr);
	iodata &= 0xff;
	
	for (x=0;x<N;x++) {
		if (iodata == band_info[x].index) {
			*index = band_info[x].index;
			break;
		}
	}
	if (x == N) {
		//*name = band_info[N-1].rname;		
		//printf("name:%s\n",*name);	
		return (0);
	}
	if (band) {
		*band = band_info[x].band;
	}
	if (type) {
		*type = band_info[x].type;
	}
	if (desc) {
		*desc = band_info[x].desc;
	}
	if (firsel) {
		*firsel = band_info[x].firsel;
	}
	if (fa) {
		*fa = band_info[x].fa;
	}
	if (nco) {
		*nco = band_info[x].nco;
	}
	if(name) {
		*name = band_info[x].rname;	
	}	
	return (1);
}

/**********************************************************************************
 * ICS related APIs. 
 * 
 * 1. setIcsOnOff(),getIcsOnOff()
 * 2. setLpcOnOff(),getLpcOnOff()
 * 3. getIcsOffset(), getIcsUnitTap(), getIcsNumbTap()
 * 4. setIcsDelay1(), getIcsDelay1(), setIcsDelay2(), getIcsDelay2()
 **********************************************************************************/
void setIcsOnOff(struct ics_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~ICS_ON) : (ctrl | ICS_ON);
    
    FPGA_outl(ioaddr,ctrl);
}
int getIcsOnOff(struct ics_struct *pdev, int path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & ICS_ON) ? 1:0;
}
void setLpcOnOff(struct ics_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~LPC_ON) : (ctrl | LPC_ON);
    
    FPGA_outl(ioaddr,ctrl);
}
int getLpcOnOff(struct ics_struct *pdev, int path)
{
    unsigned short ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & LPC_ON) ? 1:0;
}
/* getLteNumbTap() 
 * A_REG3[ 4: 0] : UNIT_TAP
 * A_REG3[ 9: 5] : NUMB_TAP
 * A_REG3[15:10] : OFFSET
 */
unsigned int getIcsOffset(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata=0;
    
	if (pdev->band_info.type == 'l' || pdev->band_info.type == 'e') {
    	ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG1;
    	iodata = FPGA_inl(ioaddr);
    	iodata = ((iodata >> 27) & 0x1f);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG1;
    	iodata = FPGA_inl(ioaddr);
    	iodata = ((iodata >> 27) & 0x1f);
    }
    else {
		printf("Error[Band Type] for %s\n", pdev->bd_name);
		iodata = 0;
    }
    return (iodata);
}
unsigned int getIcsUnitTap(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
	if (pdev->band_info.type == 'l' || pdev->band_info.type == 'e') {
    	ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG1;
    	iodata = FPGA_inl(ioaddr);
    	iodata = ((iodata >> 16) & 0x1f);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG1;
    	iodata = FPGA_inl(ioaddr);
    	iodata = ((iodata >> 16) & 0x1f);
    }
    else {
		printf("Error[Band Type] for %s\n", pdev->bd_name);
		iodata = 0;
    }
    return (iodata);
}
unsigned int getIcsNumbTap(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
	if (pdev->band_info.type == 'l' || pdev->band_info.type == 'e') {
    	ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG1;
    	iodata = FPGA_inl(ioaddr);
    	iodata = ((iodata >> 21) & 0x3f);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG1;
    	iodata = FPGA_inl(ioaddr);
    	iodata = ((iodata >> 21) & 0x3f);
    }
    else {
		printf("Error[Band Type] for %s\n", pdev->bd_name);
		iodata = 0;
    }
    return (iodata);
}
ushort getLmsStepDef(struct ics_struct *pdev, int path)					/* 실험에 의해 결정된 LMS_STEP 디폴트 값을 일거온다. ISO 확보시 Step 값 		*/
{          																/* ISO 확보시에는 Step 값을 키워서 Coef를 거의 0에 가깝게 바꿔놔야 EVM이 좋다.	*/
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_STEP;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xff;
    return iodata;
}          
ushort getLmsStepRun(struct ics_struct *pdev, int path)					/* 실험에 의해 결정된 LMS_STEP RUN 값을 일거온다. G=ISO+15 Step 값 		*/
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_STEP;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xff;
    return iodata;
}          
ushort getLpcStepDef(struct ics_struct *pdev, int path)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_STEP;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  8) & 0xff;
    return iodata;
}          
ushort getLpcStepRun(struct ics_struct *pdev, int path)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_STEP;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 24) & 0xff;
    return iodata;
}          
ushort getLmsTgetRun(struct ics_struct *pdev, int path)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_TGET;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    return iodata;
}          
ushort getLmsTgetDef(struct ics_struct *pdev, int path)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_TGET;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    return iodata;
}          
void setLmsTgetNow(struct ics_struct *pdev, int path, short set)		/* 현재 적용 중인 LMS_STEP 값을 읽어 온다. */
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0000ffff;
    iodata |= (set <<  0);
    FPGA_outl(ioaddr, iodata);
}          
ushort getLmsTgetNow(struct ics_struct *pdev, int path)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    return iodata;
}          
void setLmsStepNow(struct ics_struct *pdev, int path, uchar set)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x00ff0000;
    iodata |= (set << 16);
    FPGA_outl(ioaddr, iodata);
}          
ushort getLmsStepNow(struct ics_struct *pdev, int path)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xff;
    return iodata;
}          
void setLpcStepNow(struct ics_struct *pdev,int path, short set)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0xff000000;
    iodata |= (set << 24);
    FPGA_outl(ioaddr, iodata);
}          
uchar getLpcStepNow(struct ics_struct *pdev, int path)
{          
    unsigned int ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LMS_NOW;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 24) & 0xff;
    return iodata;
}  
void setIcsDelay1(struct ics_struct *pdev, int path, unsigned int delay)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + LTE_DELAY1;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x00000fff;
    iodata |= (delay << 0);
    FPGA_outl(ioaddr,iodata);
}
unsigned int getIcsDelay1(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + LTE_DELAY1;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 0) & 0xfff;

    return (iodata);
}
void setIcsDelay2(struct ics_struct *pdev, int path, unsigned int delay)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + LTE_DELAY2;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0fff0000;
    iodata |= (delay << 16);
    FPGA_outl(ioaddr,iodata);
}
unsigned int getIcsDelay2(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + LTE_DELAY2;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xfff;

    return (iodata);
}
void setIcsDelay3(struct ics_struct *pdev, int path, unsigned int delay)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + LTE_DELAY3;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0000f000;
    iodata |= (delay << 12);
    FPGA_outl(ioaddr,iodata);
}
unsigned int getIcsDelay3(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + LTE_DELAY3;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 12) & 0xf;

    return (iodata);
}
void setLpcMdly(struct ics_struct *pdev, unsigned int delay)
{
	unsigned int ioaddr;
	unsigned int iodata;
    
	ioaddr = pdev->ics_reg_base[DL] + LTE_DELAY1;
	iodata = FPGA_inl(ioaddr);
	iodata &= ~0x0000f000;
	iodata |= (delay << 12);
	FPGA_outl(ioaddr,iodata);
}
void setLpcRdly(struct ics_struct *pdev, unsigned int delay)
{
	unsigned int ioaddr;
	unsigned int iodata;
    
	ioaddr = pdev->ics_reg_base[DL] + LTE_DELAY2;
	iodata = FPGA_inl(ioaddr);
	iodata &= ~0xf0000000;
	iodata |= (delay << 28);
	FPGA_outl(ioaddr,iodata);
}
int getLpcMdly(struct ics_struct *pdev)
{
    unsigned int 	ioaddr;
    unsigned int 	iodata;
    
    ioaddr = pdev->ics_reg_base[DL] + LTE_DELAY1;
    iodata = FPGA_inl(ioaddr);
    iodata = ((iodata >> 12) & 0xf);
    return (iodata);
}
int getLpcRdly(struct ics_struct *pdev)
{
	unsigned int 	ioaddr;
	unsigned int 	iodata;
    
	ioaddr = pdev->ics_reg_base[DL] + LTE_DELAY2;
	iodata = FPGA_inl(ioaddr);
	iodata = ((iodata >> 28) & 0xf);
	return (iodata);
}
/**********************************************************************************
 * ILC 관련 APIs
 * 
 * 1. setIcsIlcOnOff(),getIcsIlcOnOff()
 * 2. getIcsIlcGain(),getLpcOnOff()
 * 3. setIcsIlcXth(),getIcsIlcXth()
 * 4. getIcsIlcTatt(), setIcsIlcTget(), getIcsIlcTget(), getIcsIlcSetp()
 **********************************************************************************/
void setIcsIlcOnOff(struct ics_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~ILC_ON) : (ctrl | ILC_ON);
    
    FPGA_outl(ioaddr,ctrl);
}
int getIcsIlcOnOff(struct ics_struct *pdev, int path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;

    ctrl = FPGA_inl(ioaddr);

	return (ctrl & ILC_ON) ? 1:0;
}
int getIcsIlcGain(struct ics_struct *pdev, int path)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ILC_GAIN;
    	
    	iodata = FPGA_inl(ioaddr);
    	iodata = (iodata >> 16) & 0xffff;
    	if (!iodata) {
    		gain = 0.0001;
    	}
    	else {
    		gain = (double)iodata;
    		
    		//gain = gain / (2047.0);			//AGC 30dB Gain
    		//gain = gain / (4095.0);		//AGC 24dB Gain
    		//gain = gain / (8191.0);		//AGC 18dB Gain
    		//gain = gain / (16383.0);		//AGC 12dB Gain
    		//gain = gain / (32767.0);		//AGC 6dB Gain
    		gain = gain / (65535.0);		//AGC 0dB Gain    
		}
	}
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ILC_GAIN;
    	
    	iodata = FPGA_inl(ioaddr);
    	iodata = (iodata >> 16) & 0xffff;
    	if (!iodata) {
    		gain = 0.0001;
    	}
    	else {
    		gain = (double)iodata;
    		
    		//gain = gain / (2047.0);			//AGC 30dB Gain
    		//gain = gain / (4095.0);		//AGC 24dB Gain
    		//gain = gain / (8191.0);		//AGC 18dB Gain
    		//gain = gain / (16383.0);		//AGC 12dB Gain
    		//gain = gain / (32767.0);		//AGC 6dB Gain
    		gain = gain / (65535.0);		//AGC 0dB Gain    
		}
	}
    gain = 20*log10(gain);

    return (int)(gain * 100);
}
/* 	setIcsIlcXth()
 */
void setIcsIlcXth(struct ics_struct *pdev, short gain)
{
    double set_gain;
    unsigned short init_gain;
    unsigned int ioaddr,iodata;

	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[UL] + LTE_ILC_X_TH;
 		iodata = FPGA_inl(ioaddr);
 		iodata &= ~0x0000ffff;
    	set_gain = ((double)gain/2000.0);
    	init_gain = (unsigned int)(pow(10.0,set_gain) * 4095);
    	iodata |= (init_gain <<  0);
    	FPGA_outl(ioaddr, iodata);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[UL] + LTE_ILC_X_TH;
 		iodata = FPGA_inl(ioaddr);
 		iodata &= ~0x0000ffff;
    	set_gain = ((double)gain/2000.0);
    	init_gain = (unsigned int)(pow(10.0,set_gain) * 4095);
    	iodata |= (init_gain <<  0);
    	FPGA_outl(ioaddr, iodata);
    }
}
int getIcsIlcXth(struct ics_struct *pdev)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[UL] + LTE_ILC_X_TH;
    	
    	iodata = FPGA_inl(ioaddr);
    	iodata &= 0x0000ffff;
    	if (!iodata) {
    		gain = 0.0001;
    	}
    	else {
    		gain = (double)iodata;
    		
    		//gain = gain / (2047.0);			//AGC 30dB Gain
    		gain = gain / (4095.0);		//AGC 24dB Gain
    		//gain = gain / (8191.0);		//AGC 18dB Gain
    		//gain = gain / (16383.0);		//AGC 12dB Gain
    		//gain = gain / (32767.0);		//AGC 6dB Gain
    		//gain = gain / (65535.0);		//AGC 0dB Gain    
		}
    	gain = 20*log10(gain);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[UL] + LTE_ILC_X_TH;
    	
    	iodata = FPGA_inl(ioaddr);
    	iodata &= 0x0000ffff;
    	if (!iodata) {
    		gain = 0.0001;
    	}
    	else {
    		gain = (double)iodata;
    		
    		//gain = gain / (2047.0);			//AGC 30dB Gain
    		gain = gain / (4095.0);		//AGC 24dB Gain
    		//gain = gain / (8191.0);		//AGC 18dB Gain
    		//gain = gain / (16383.0);		//AGC 12dB Gain
    		//gain = gain / (32767.0);		//AGC 6dB Gain
    		//gain = gain / (65535.0);		//AGC 0dB Gain    
		}
    	gain = 20*log10(gain);
    }
    return (int)(gain * 100);
}
int getIcsIlcTatt(struct ics_struct *pdev, int path)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ILC_TATT;
    	
    	iodata = FPGA_inl(ioaddr);
    	iodata &= 0x0000ffff;
    	if (!iodata) {
    		gain = 0.0001;
    	}
    	else {
    		gain = (double)iodata;
    		
    		gain = gain / (2047.0);			//AGC 30dB Gain
    		//gain = gain / (4095.0);		//AGC 24dB Gain
    		//gain = gain / (8191.0);		//AGC 18dB Gain
    		//gain = gain / (16383.0);		//AGC 12dB Gain
    		//gain = gain / (32767.0);		//AGC 6dB Gain
    		//gain = gain / (65535.0);		//AGC 0dB Gain    
		}
    	
    	gain = 20*log10(gain);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ILC_TATT;
    	
    	iodata = FPGA_inl(ioaddr);
    	iodata &= 0x0000ffff;
    	if (!iodata) {
    		gain = 0.0001;
    	}
    	else {
    		gain = (double)iodata;
    		
    		gain = gain / (2047.0);			//AGC 30dB Gain
    		//gain = gain / (4095.0);		//AGC 24dB Gain
    		//gain = gain / (8191.0);		//AGC 18dB Gain
    		//gain = gain / (16383.0);		//AGC 12dB Gain
    		//gain = gain / (32767.0);		//AGC 6dB Gain
    		//gain = gain / (65535.0);		//AGC 0dB Gain    
		}
    	
    	gain = 20*log10(gain);
    }
    return (int)(gain * 100);
}
void setIcsIlcTget(struct ics_struct *pdev, short set)
{          
    double set_point;
    unsigned int set_word;
    unsigned int ioaddr;
           
    ioaddr = pdev->ics_reg_base[DL] + LTE_ILC_TGET;
		   
    set_point = ((double)set/2000.0);
    set_word = (unsigned int)(pow(10.0,set_point) * 32767);
           
    FPGA_outl(ioaddr, set_word*set_word);
}          
int getIcsIlcTget(struct ics_struct *pdev)
{        
    double target = 0.0f;
    unsigned int ioaddr;
         
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[DL] + LTE_ILC_TGET;
    	     
    	target = (double)FPGA_inl(ioaddr);
    	target = sqrt(target);
    	target = target / 32767.0;
    	     
    	target = 20*log10(target);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[DL] + LTE_ILC_TGET;
    	     
    	target = (double)FPGA_inl(ioaddr);
    	target = sqrt(target);
    	target = target / 32767.0;
    	     
    	target = 20*log10(target);
    }
    return (int)(target * 100);
}        
int getIcsIlcSetp(struct ics_struct *pdev, int path)
{        
    double target = 0.0f;
    unsigned int ioaddr;
         
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ILC_SETP;
    	     
    	target = (double)FPGA_inl(ioaddr);
    	target = sqrt(target);
    	target = target / 32767.0;
    	     
    	target = 20*log10(target);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ILC_SETP;
    	     
    	target = (double)FPGA_inl(ioaddr);
    	target = sqrt(target);
    	target = target / 32767.0;
    	     
    	target = 20*log10(target);
    }
    return (int)(target * 100);
}        
/**********************************************************************************
 * DAC On/Off related APIs. 
 * 
 * 1. setIcsDacOnOff()
 * 2. getIcsDacOnOff()
 **********************************************************************************/
void setIcsDacOnOff(struct ics_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;

    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~DAC_ON) : (ctrl | DAC_ON);
    FPGA_outl(ioaddr,ctrl);
}
int getIcsDacOnOff(struct ics_struct *pdev, int path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;

    ctrl = FPGA_inl(ioaddr);

	return (ctrl & DAC_ON) ? 1:0;
}

/**********************************************************************************
 * ALC Attn related APIs. 
 * 
 * 1. setIcsAttGain()
 * 2. getIcsAttGain()
 **********************************************************************************/
/* 	setIcsAttGain()
 *	각 FA의 DSP ATTN 값을 설정한다. 
 */
void setIcsAttGain(struct ics_struct *pdev, int path, short gain)
{
    double set_gain;
    unsigned short init_gain;
    unsigned int ioaddr,iodata=0;

    ioaddr = pdev->ics_reg_base[path] + LTE_ATT_GAIN;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0xffff0000;

    set_gain = ((double)gain/2000.0);

    //init_gain = (unsigned int)(pow(10.0,set_gain) * 65535);	//  0dB
	//init_gain = (unsigned int)(pow(10.0,set_gain) * 32767);	//  6dB
    //init_gain = (unsigned int)(pow(10.0,set_gain) * 16383);	// 12dB
	init_gain = (unsigned int)(pow(10.0,set_gain) * 8191);	// 18dB
	//init_gain = (unsigned int)(pow(10.0,set_gain) * 4095);		// 24dB
    iodata |= (init_gain << 16);
   	
    FPGA_outl(ioaddr, iodata);
}

/* 	getIcsAttGain()
 *	ATT의 최대 이득은 18dB이다.
 */
int getIcsAttGain(struct ics_struct *pdev, int path)
{          
    double gain;
    unsigned int ioaddr;
    unsigned int iodata=0;
           
    ioaddr = pdev->ics_reg_base[path] + LTE_ATT_GAIN;

    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;

		//gain = gain / (2047.0);		//ATC 30dB Gain
		//gain = gain / (4095.0);		//ATC 24dB Gain
		gain = gain / (8191.0);			//ATC 18dB Gain
		//gain = gain / (16383.0);		//ATC 12dB Gain
		//gain = gain / (32767.0);		//ATC 6dB Gain
		//gain = gain / (65535.0);		//ATC 0dB Gain
    }
    gain = 20*log10(gain);
    return (int)(gain * 100);
}

/**********************************************************************************
 * User Attn related APIs. 
 * 
 * 1. setIcsUsrGain()
 * 2. getIcsUsrGain()
 **********************************************************************************/
void setIcsUsrGain(struct ics_struct *pdev, int path, short gain)
{
    double set_gain;
    unsigned short init_gain;
    unsigned int ioaddr,iodata=0;

//    printf("ics reg base : 0x%x\n", pdev->ics_reg_base[path]);
    /* 'l'과 'w' 모두 레지스터 주소가 같다. */
    ioaddr = pdev->ics_reg_base[path] + ICS_USR_GAIN;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0000ffff;

    set_gain = ((double)gain/2000.0);
    init_gain = (unsigned int)(pow(10.0,set_gain) * 8191);

    iodata |= (init_gain <<  0);
    FPGA_outl(ioaddr, iodata);
}

/* 	getIcsUsrGain()
 *	ATT의 최대 이득은 6dB이다.
 */
int getIcsUsrGain(struct ics_struct *pdev, int path)
{          
    double 				gain;
    unsigned int 		ioaddr;
    unsigned int 		iodata=0;
           
    /* 'l'과 'w' 모두 레지스터 주소가 같다. */
    ioaddr 	= pdev->ics_reg_base[path] + ICS_USR_GAIN;

    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;
		
		//gain = gain / (2047.0);		//ATC 30dB Gain
		//gain = gain / (4095.0);			//ATC 24dB Gain
		gain = gain / (8191.0);		//ATC 18dB Gain
		//gain = gain / (16383.0);		//ATC 12dB Gain
		//gain = gain / (32768.0);		//ATC 6dB Gain
		//gain = gain / (65535.0);		//ATC 0dB Gain
    }
    
    gain = 20*log10(gain);
    	
    return (int)(gain * 100);
}
/**********************************************************************************
 * ALC related APIs. 
 * 
 * 1. setIcsAlcOnOff()
 * 2. getIcsAlcOnOff()
 * 3. setIcsAlcSetp()
 * 4. getIcsAlcSetp()
 * 5. getIcsAlcGain()
 **********************************************************************************/
void setIcsAlcOnOff(struct ics_struct *pdev, int path, uchar onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;

    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~ALC_ON) : (ctrl | ALC_ON);
    FPGA_outl(ioaddr,ctrl);
}
int getIcsAlcOnOff(struct ics_struct *pdev, short path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & ALC_ON) ? 1:0;
}
void setIcsLimOnOff(struct ics_struct *pdev, int path, uchar onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;

    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~LIM_ON) : (ctrl | LIM_ON);
    FPGA_outl(ioaddr,ctrl);
}
int getIcsLimOnOff(struct ics_struct *pdev, short path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & LIM_ON) ? 1:0;
}
void setIcsAlcSetp(struct ics_struct *pdev, int path, int set)
{          
    double set_point;
    unsigned int set_word;
    unsigned int ioaddr;
           
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ALC_SETP;
    	
    	set_point = ((double)set/2000.0);
    	set_word = (unsigned int)(pow(10.0,set_point) * 32767);   
    	       
    	FPGA_outl(ioaddr, set_word*set_word);
	}
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_ALC_SETP;
    	
    	set_point = ((double)set/2000.0);
    	set_word = (unsigned int)(pow(10.0,set_point) * 32767);   
    	       
    	FPGA_outl(ioaddr, set_word*set_word);
	}
}          
int getIcsAlcSetp(struct ics_struct *pdev, int path)
{          
    double target=0.0f;
    unsigned int ioaddr;
           
    ioaddr = pdev->ics_reg_base[path] + LTE_ALC_SETP;

    target = (double)FPGA_inl(ioaddr);
    target = sqrt(target);
    target = target / 32767.0;

    target = 20*log10(target);
    return (int)(target * 100);
}          
/* 	getIcsAlcGain()
 *	현재 각 FA의 DSP ALC의 이득값을 구한다. 
 *	ALC의 최대 이득은 0dB이다.
 */
int getIcsAlcGain(struct ics_struct *pdev, int path)
{          
    double gain=0.0f;
    unsigned int ioaddr;
    unsigned int iodata=0;
           
    ioaddr = pdev->ics_reg_base[path] + LTE_ALC_GAIN;

    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    if (!iodata) {
    	gain = 0xffff;
		//gain = gain / (4095.0);			//ALC 24dB Gain
		gain = gain / (8191.0);				//ALC 18dB Gain
    }
    else {
    	gain = (double)iodata;

		//gain = gain / (2047.0);		//ALC 30dB Gain
		gain = gain / (4095.0);			//ALC 24dB Gain
		//gain = gain / (8191.0);			//ALC 18dB Gain
		//gain = gain / (16383.0);		//ALC 12dB Gain
		//gain = gain / (32767.0);		//ALC 6dB Gain
		//gain = gain / (65535.0);		//ALC 0dB Gain
    }
    gain = 20*log10(gain);
    return (int)(gain * 100);
}

int getIcsLimGain(struct ics_struct *pdev, int path)
{          
    double gain=0.0f;
    unsigned int ioaddr;
    unsigned int iodata=0;
           
    ioaddr = pdev->ics_reg_base[path] + LTE_LIM_GAIN;

    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    if (!iodata) {
    	gain = 0xffff;
		//gain = gain / (4095.0);			//Lim 0dB Gain
		gain = gain / (65535.0);			//Lim 0dB Gain
    }
    else {
    	gain = (double)iodata;

		//gain = gain / (2047.0);		//Lim 30dB Gain
		//gain = gain / (4095.0);			//Lim 24dB Gain
		//gain = gain / (8191.0);		//Lim 18dB Gain
		//gain = gain / (16383.0);		//Lim 12dB Gain
		//gain = gain / (32767.0);		//Lim 6dB Gain
		gain = gain / (65535.0);		//Lim 0dB Gain
    }
    gain = 20*log10(gain);
    return (int)(gain * 100);
}

/**********************************************************************************
 * AGC related APIs. 
 * 
 * 1. setIcsAgcOnOff()
 * 2. getIcsAgcOnOff()
 * 3. setIcsAgcSetp()
 * 4. getIcsAgcSetp()
 * 5. getIcsAgcGain()
 **********************************************************************************/
void setIcsAgcOnOff(struct ics_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~AGC_ON) : (ctrl | AGC_ON);
    FPGA_outl(ioaddr,ctrl);
}
int getIcsAgcOnOff(struct ics_struct *pdev, int path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & AGC_ON) ? 1:0;
}
void setIcsAgcSetp(struct ics_struct *pdev, int path, int set)
{          
    double set_point;
    unsigned int set_word;
    unsigned int ioaddr;
           
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_AGC_SETP;
    	
    	set_point = ((double)set/2000.0);
    	set_word = (unsigned int)(pow(10.0,set_point) * 32767);   
    	       
    	FPGA_outl(ioaddr, set_word*set_word);
	}
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_AGC_SETP;
    	
    	set_point = ((double)set/2000.0);
    	set_word = (unsigned int)(pow(10.0,set_point) * 32767);   
    	       
    	FPGA_outl(ioaddr, set_word*set_word);
	}
}          
int getIcsAgcSetp(struct ics_struct *pdev, int path)
{        
    double target=0.0f;
    unsigned int ioaddr;
         
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_AGC_SETP;
    	       
    	target = (double)FPGA_inl(ioaddr);
    	target = sqrt(target);
    	target = target / 32767.0;
    	       
    	target = 20*log10(target);
	}
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[path] + LTE_AGC_SETP;
    	       
    	target = (double)FPGA_inl(ioaddr);
    	target = sqrt(target);
    	target = target / 32767.0;
    	       
    	target = 20*log10(target);
	}
    return (int)(target * 100);
}        
int getIcsManGain(struct ics_struct *pdev, short path)
{          
    double gain=0.0f;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
    ioaddr = pdev->ics_reg_base[path] + LTE_MAN_GAIN;

    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;
    	
    	//gain = gain / (2047.0);		//AGC 30dB Gain
    	gain = gain / (4095.0);			//AGC 24dB Gain
    	//gain = gain / (8191.0);		//AGC 18dB Gain
    	//gain = gain / (16383.0);		//AGC 12dB Gain
    	//gain = gain / (32767.0);		//AGC 6dB Gain
    	//gain = gain / (65535.0);		//AGC 0dB Gain
	}
    gain = 20*log10(gain);
    return (int)(gain * 100);
}
/* getIcsRefGain()
 * Ref(=AGC) Gain
 */
int getIcsRefGain(struct ics_struct *pdev, short path)
{          
    double gain=0.0f;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
    ioaddr = pdev->ics_reg_base[path] + LTE_REF_GAIN;

    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;
    	
    	//gain = gain / (2047.0);		//AGC 30dB Gain
    	gain = gain / (4095.0);			//AGC 24dB Gain
    	//gain = gain / (8191.0);		//AGC 18dB Gain
    	//gain = gain / (16383.0);		//AGC 12dB Gain
    	//gain = gain / (32767.0);		//AGC 6dB Gain
    	//gain = gain / (65535.0);		//AGC 0dB Gain
	}
    gain = 20*log10(gain);
    return (int)(gain * 100);
}

//////////////////////////////////////////////////////////////////////////////////////
//	Pre-Block
//////////////////////////////////////////////////////////////////////////////////////
int getPreRefPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_LMS_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getPreRefPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_LMS_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getPreManPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_FBK_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getPreManPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_FBK_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getPreLmsPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_ATT_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getPreLmsPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;

    ioaddr = pdev->ics_pwr_base[path] + LTE_ATT_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);

    return (pwr);
}
int getPreAgcSetp(struct ics_struct *pdev, int path)
{        
    double target=0.0f;
    unsigned int ioaddr;
         
    ioaddr = pdev->ics_reg_base[path] + LTE_AGC_SETP;
           
    target = (double)FPGA_inl(ioaddr);
    target = sqrt(target);
    target = target / 32767.0;
           
    target = 20*log10(target);

    return (int)(target * 100);
}        
int getPreManGain(struct ics_struct *pdev, short path)
{          
    double gain=0.0f;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
    ioaddr = pdev->ics_reg_base[path] + LTE_MAN_GAIN;
    
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;
    	
    	//gain = gain / (2047.0);		//AGC 30dB Gain
    	//gain = gain / (4095.0);			//AGC 24dB Gain
    	//gain = gain / (8191.0);		//AGC 18dB Gain
    	//gain = gain / (16383.0);		//AGC 12dB Gain
    	//gain = gain / (32767.0);		//AGC 6dB Gain
    	gain = gain / (65535.0);		//AGC 0dB Gain    
	}
    gain = 20*log10(gain);
    return (int)(gain * 100);
}
/* getIcsRefGain()
 * Ref(=AGC) Gain
 */
int getPreRefGain(struct ics_struct *pdev, short path)
{          
    double gain=0.0f;
    unsigned int ioaddr;
    unsigned int iodata=0;
       
    ioaddr = pdev->ics_reg_base[path] + LTE_REF_GAIN;
    
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    if (!iodata) {
    	gain = 0.0001;
    }
    else {
    	gain = (double)iodata;
    	
    	//gain = gain / (2047.0);		//AGC 30dB Gain
    	//gain = gain / (4095.0);			//AGC 24dB Gain
    	//gain = gain / (8191.0);		//AGC 18dB Gain
    	//gain = gain / (16383.0);		//AGC 12dB Gain
    	//gain = gain / (32767.0);		//AGC 6dB Gain
    	gain = gain / (65535.0);		//AGC 0dB Gain    
	}
    gain = 20*log10(gain);
    return (int)(gain * 100);
}

//////////////////////////////////////////////////////////////////////////////////////
//	set 값은 0.1Mhz 단위이다. 따라서 +10.1M를 명시하고자 한다면, set값은 101이 된다. 
//	0.1/30.72 = 10/3072 = 5/1536
#define TAPS 			1536
#define fs 				(double)(30.72f)
#define SECOND_MUL 		(double)360/fs
#define PI   			3.1415926535897932

void setIcsNco(struct ics_struct *pdev, int set)
{
	double 			ctime=0;                 
	float			fc;
	double 			coef_i,coef_q;
	int 			coe_i,coe_q;
	int				i;
	unsigned int	iq;

	fc = (float)set/10.0;

	ctime = (double)(fc * SECOND_MUL);

	for (i=0;i<TAPS;i++) {
		coef_i = cos((ctime * i)*PI/180) * 32767;
		coef_q = -sin((ctime * i)*PI/180) * 32767;
		
		coe_i = (int)coef_i;
		coe_q = (int)coef_q;
		coe_i &= 0xffff;
		coe_q &= 0xffff;
		iq = (coe_q << 16) | coe_i;
		//printf("%08x >> %08x\n",pdev->nco_ram_base+i*4, iq);
    	FPGA_outl(pdev->nco_ram_base+i*4, iq);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//
//	FA Mode Control and Sleep
//
//////////////////////////////////////////////////////////////////////////////////////
void setIcsFaMode(struct ics_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;

    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~FA_MODE) : (ctrl | FA_MODE);
    FPGA_outl(ioaddr,ctrl);
}
int getIcsFaMode(struct ics_struct *pdev, int path)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & FA_MODE) ? 1:0;
}
int getIcsFaAlloc(struct ics_struct *pdev)
{
    unsigned short ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[DL] + ICS_CONFIG1;
    ctrl = FPGA_inl(ioaddr);
    ctrl = (ctrl >> 4) & 0xf;
    return (ctrl);
}

void setIcsFaAlloc(struct ics_struct *pdev, unsigned short set)
{
    unsigned short mask;
    unsigned short ctrl;
    unsigned int ioaddr;
    
	mask = set & 0xf;
	
    ioaddr = pdev->ics_reg_base[DL] + ICS_CONFIG1;
    ctrl = FPGA_inl(ioaddr);
    ctrl &= ~0xf0;
    ctrl |= (mask << 4);
    FPGA_outl(ioaddr,ctrl);

    ioaddr = pdev->ics_reg_base[UL] + ICS_CONFIG1;
    ctrl = FPGA_inl(ioaddr);
    ctrl &= ~0xf0;
    ctrl |= (mask << 4);
    FPGA_outl(ioaddr,ctrl);

}
/* FA Threshold Level 설정 */
void setIcsFaThrLvl(struct ics_struct *pdev, int path, int target)
{          
    double set_point;
    unsigned int ioaddr;
    unsigned int set_word;
    unsigned int set_long;
           
    /* 'w','l' 모두 FA_LVL 레지스터 주소는 같다.  */
    ioaddr = pdev->ics_reg_base[path] + FA_THR_LVL;
	set_point = ((double)target/2000.0);	
	set_word = (unsigned int)(pow(10.0,set_point) * 32767);
	set_long = (set_word*set_word);
	FPGA_outl(ioaddr, set_long); 
	//mon_printf("%08x\n", set_long);
}           
int getIcsFaThrLvl(struct ics_struct *pdev, int path)
{          
    unsigned int ioaddr;
    int		lvll;
    double	lvlf;
    int 	lvli;
           
    ioaddr = pdev->ics_reg_base[path] + FA_THR_LVL;
	lvll = FPGA_inl(ioaddr);
	lvlf = (float)(sqrt((double)lvll)/32767.0);

    lvlf = 20*log10(lvlf);
    lvli = (int)(lvlf * 100);

    return (lvli);
}          


/* setIcsFir()
 *	FDD(5종류) : Off(0), 5M(1), 10M(2), 15M(3), 20M(4)
 */
void setIcsFilter(struct ics_struct *pdev, unsigned short filter)
{
    unsigned int mask;
    unsigned int ctrl;
    unsigned int ioaddr;
    
	mask = filter & 0xf;
	
    ioaddr = pdev->ics_reg_base[DL] + ICS_CONFIG1;
    ctrl = FPGA_inl(ioaddr);
    ctrl &= ~0x00000F00;
    ctrl |= (mask << 8);
    FPGA_outl(ioaddr,ctrl);
}
int getIcsFilter(struct ics_struct *pdev)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[DL] + ICS_CONFIG1;
    ctrl = FPGA_inl(ioaddr);
    ctrl = (ctrl >> 8) & 0xf;

    return (ctrl);
}

/* setIcsMno()
 * For 1.7G 
 * 		0 -> KDDI
 * 		1 -> Rakuten(C1)
 * 		2 -> SoftBank
 * 		3 -> Rakuten(C2) or Docomo
 */
void setIcsMno(struct ics_struct *pdev, uchar MNO)
{
    unsigned int mask;
    unsigned int ctrl;
    unsigned int ioaddr;
    
	mask = MNO & 0x3;
	
    ioaddr = pdev->ics_reg_base[UL] + ICS_CONFIG1;
    ctrl = FPGA_inl(ioaddr);
    ctrl &= ~0x00000F00;
    ctrl |= (mask << 8);
    FPGA_outl(ioaddr,ctrl);
}

int getIcsMno(struct ics_struct *pdev)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[UL] + ICS_CONFIG1;
    ctrl = FPGA_inl(ioaddr);
    ctrl = (ctrl >> 8) & 0xf;

    return (ctrl);
}

char *showfloat(char *buf, short val)
{
	int 	ival,fval;
	char 	isNeg;
	char	sign[2]={'+','-'};
	char 	*str;
	
	str = buf;
	
	isNeg = (val&0x8000) ? 1 : 0;
	
	ival = (int)val;												/* -0.0~-0.9까지 Display하는데 문제가 있었음. */
	fval = abs(ival)%100;
	ival = abs(ival)/100;
	sprintf(buf, " %c%2d.%02d",sign[isNeg],ival,fval);
	
	return (str);
}

//////////////////////////////////////////////////////////////////////////////////////
//
//	Power Meter, Iso Calculation
//
//////////////////////////////////////////////////////////////////////////////////////
int cal_power(int val)
{
    double	pwr1, pwr2;
    int 	pwr;

	pwr1 = (double)(sqrt((double)val)/32767.0);
    if (pwr1 < 0.000016)
        pwr1 = 0.000016;
    
    pwr2 = 20*log10(pwr1);
    pwr = (int)(pwr2 * 100);
    
    return (pwr);
}

/* cal_iso()
 * isollation을 계산한다. 
 */
short cal_iso(int val1,int val2)
{
    double 	iso1;
    short 	iso;

    if (val2) {
    	iso1 = ((double)val1/(double)val2);
    }
    else {
    	iso1 = 0.0001;
    }
    

    if (iso1 < 0.0001)
        iso1 = 0.0001;
    
    iso1 = 10*log10(iso1);
    iso = (short)(iso1 * 100);
	
    return (iso);
}

short cal_papr(int peak,int mean)
{
    double	pwrf,peakf,meanf;
    short 	pwrw;
    
    peakf = (double)(sqrt((double)peak)/32767.0);
    meanf = (double)(sqrt((double)mean)/32767.0);
    
    pwrf = 20*log10(peakf/meanf);
    pwrw = (short)(pwrf * 100);
    
    return (pwrw);
}

int getIcsPapr(struct ics_struct *pdev, short path)
{
    unsigned int ioaddr;
    unsigned int peak,mean;
    int papr;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_LMS_PKK;
    peak = FPGA_inl(ioaddr);
    ioaddr = pdev->ics_pwr_base[path] + LTE_LMS_PWR;
    mean = FPGA_inl(ioaddr);
    papr = cal_papr(peak, mean);
    return (papr);
}

int getIcsIso(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr;
    unsigned int man,lms;
    int iso=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_FBK_PWR;  ///// LTE_DDC_PWR
    man = FPGA_inl(ioaddr);
    ioaddr = pdev->ics_pwr_base[path] + LTE_LMS_PWR;
    lms = FPGA_inl(ioaddr);
    iso = cal_iso(man, lms);
    return (iso);
}

/* NR RSRP 파워 .*/
int getSsbPwr(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
	if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[DL] + 0x3C;
    	iodata = FPGA_inl(ioaddr);
    	pwr = cal_power(iodata);
    }
    return (pwr);
}

int getSsbPkk(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
	if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[UL] + 0x3C;
    	iodata = FPGA_inl(ioaddr);
    	pwr = cal_power(iodata);
    }
    return (pwr);
}

int getCsiPkk(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
	if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_reg_base[DL] + 0x5C;
    	iodata = FPGA_inl(ioaddr);
    	pwr = cal_power(iodata);
    }
    return (pwr);
}

int getIcsDdcPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_DDC_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
/* LTE에만 사용됨. Peak Power를 구해야만 하는 경우.*/
int getIcsDdcPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_DDC_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getIcsLmsPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_LMS_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
/* LTE에만 사용됨. Peak Power를 구해야만 하는 경우.*/
int getIcsLmsPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_LMS_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getIcsFbkPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_FBK_PWR;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}
int getIcsFbkPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_FBK_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}

int getIcsAttPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr = 0;
    
	if (pdev->band_info.type == 'l') {
    	ioaddr = pdev->ics_pwr_base[path] + LTE_ATT_PWR;
    	iodata = FPGA_inl(ioaddr);
    	pwr = cal_power(iodata);
    }
	else if (pdev->band_info.type == 'n') {
    	ioaddr = pdev->ics_pwr_base[path] + LTE_ATT_PWR;
    	iodata = FPGA_inl(ioaddr);
    	pwr = cal_power(iodata);
    }
    return (pwr);
}

void setPwrClrOnOff(struct ics_struct *pdev, int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[path] + ICS_CONFIG0;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~0x08000000) : (ctrl | 0x08000000);
    
    FPGA_outl(ioaddr,ctrl);
}

int getIcsAttPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;

    ioaddr = pdev->ics_pwr_base[path] + LTE_ATT_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);

    return (pwr);
}

int getIcsUsrPwr(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
   	ioaddr = pdev->ics_pwr_base[path] + LTE_USR_PWR;
   	iodata = FPGA_inl(ioaddr);
   	pwr = cal_power(iodata);
    return (pwr);
}
int getIcsUsrPkk(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    
    ioaddr = pdev->ics_pwr_base[path] + LTE_USR_PKK;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    return (pwr);
}

#if 0
int getIcsAdcPwr(short path)
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    struct psa_struct 	*ppsa;

	ppsa = getPsaDev();
  
  	ioaddr = ppsa->psa_reg_base + LTE_ADC_PWR + path*4;
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    
    return (pwr);
}
#endif 

int getIcsAdcPwr(short path, int band)		// 22-10-05
{
    unsigned int ioaddr, iodata;
    int pwr=0;
    struct psa_struct 	*ppsa;

	ppsa = getPsaDev();
  
  	ioaddr = ppsa->psa_reg_base + LTE_ADC_PWR + path*4;   // Band별 offset 추가 필요; 22-10-05
    iodata = FPGA_inl(ioaddr);
    pwr = cal_power(iodata);
    
    return (pwr);
}

void setTddMode(struct ics_struct *pdev, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_1;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~TDD_EXT) : (ctrl | TDD_EXT);

#if DEBUG_PRINT
	printf("setTddMode[%d] > [%x]\n", onoff,ctrl);
#endif
    FPGA_outl(ioaddr,ctrl);
}

void setMonTsyncOut(struct ics_struct *pdev, int en)
{
    unsigned int ctrl;
    unsigned int ioaddr;
    
    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_1;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (en == 1) ? (ctrl & ~0x40000000) : (ctrl | 0x40000000);
#if DEBUG_PRINT
	printf("setMonTsyncOut[%d] > [%x]\n", en,ctrl);
#endif
    FPGA_outl(ioaddr,ctrl);
}


void setTddConfig(struct ics_struct *pdev, int *TransmissionPeriodicity, int *nrofDLSlot, int *nrofDLSymbol, int *nrofULSlot, int *nrofULSymbol)
{
    unsigned int ioaddr, iodata;

    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_0;
    iodata = FPGA_inl(ioaddr);
    *nrofDLSlot   = (iodata >> 16) & 0xff;
    *nrofDLSymbol = (iodata >> 24) & 0xff;

    ioaddr = pdev->ics_reg_base[UL] + TDD_CFG_0;
    iodata = FPGA_inl(ioaddr);
    *nrofULSlot   = (iodata >> 16) & 0xff;
    *nrofULSymbol = (iodata >> 24) & 0xff;
}

void getTddConfig(struct ics_struct *pdev, int *TransmissionPeriodicity, int *nrofDLSlot, int *nrofDLSymbol, int *nrofULSlot, int *nrofULSymbol)
{
    unsigned int ioaddr, iodata;

    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_0;
    iodata = FPGA_inl(ioaddr);
    *nrofDLSlot   = (iodata >> 16) & 0xff;
    *nrofDLSymbol = (iodata >> 24) & 0xff;

    ioaddr = pdev->ics_reg_base[UL] + TDD_CFG_0;
    iodata = FPGA_inl(ioaddr);
    *nrofULSlot   = (iodata >> 16) & 0xff;
    *nrofULSymbol = (iodata >> 24) & 0xff;
}
//////////////////////////////////////////////////////////////////////////////////////
int setTddSynSetp(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_4;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0000ffff;
    iodata |= (set << 0);
    FPGA_outl(ioaddr,iodata);
}
void setTddSynHold(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;

    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_4;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0xffff0000;
    iodata |= (set << 16);
    FPGA_outl(ioaddr,iodata);
}
int getTddSynSetp(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_4;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 0) & 0xffff;
    return (iodata);
}
int getTddSynHold(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_4;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    return (iodata);
}
//////////////////////////////////////////////////////////////////////////////////////
int setTddLnaSetp(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x000000ff;
    iodata |= (set << 0);
    FPGA_outl(ioaddr,iodata);
}
void setTddLnaHold(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;

    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0000ff00;
    iodata |= (set << 8);
    FPGA_outl(ioaddr,iodata);
}
int getTddLnaSetp(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 0) & 0xff;
    return (iodata);
}
int getTddLnaHold(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 8) & 0xff;
    return (iodata);
}
//////////////////////////////////////////////////////////////////////////////////////
int setTddPamSetp(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x00ff0000;
    iodata |= (set << 16);
    FPGA_outl(ioaddr,iodata);
}
void setTddPamHold(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;

    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0xff000000;
    iodata |= (set << 24);
    FPGA_outl(ioaddr,iodata);
}
int getTddPamSetp(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xff;
    return (iodata);
}
int getTddPamHold(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_2;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 24) & 0xff;
    return (iodata);
}
//////////////////////////////////////////////////////////////////////////////////////
int setTddRswSetp(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_3;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x000000ff;
    iodata |= (set << 0);
    FPGA_outl(ioaddr,iodata);
}
void setTddRswHold(struct ics_struct *pdev, int path, int set)
{
    unsigned int ioaddr, iodata;

    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_3;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0x0000ff00;
    iodata |= (set << 8);
    FPGA_outl(ioaddr,iodata);
}
int getTddRswSetp(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_3;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 0) & 0xff;
    return (iodata);
}
int getTddRswHold(struct ics_struct *pdev, int path)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[path] + TDD_CFG_3;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 8) & 0xff;
    return (iodata);
}
unsigned int getTddNegTiming1(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_5;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    return (iodata);
}
unsigned int getTddNegTiming2(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_5;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    return (iodata);
}
unsigned int getTddNegTiming3(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_6;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    return (iodata);
}
unsigned int getTddPosTiming1(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[UL] + TDD_CFG_5;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    return (iodata);
}
unsigned int getTddPosTiming2(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[UL] + TDD_CFG_5;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    return (iodata);
}
unsigned int getTddPosTiming3(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[UL] + TDD_CFG_6;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >>  0) & 0xffff;
    return (iodata);
}
unsigned int getTddCenterPos(struct ics_struct *pdev)
{
    unsigned int ioaddr, iodata;
    
    ioaddr = pdev->ics_reg_base[UL] + TDD_CFG_7;
    iodata = FPGA_inl(ioaddr);
    iodata = (iodata >> 16) & 0xffff;
    return (iodata);
}
void setIcsSleepPeriod(struct ics_struct *pdev, short set)
{          
    unsigned int 	ioaddr;
    unsigned int	iodata;	
           
    ioaddr = pdev->ics_reg_base[UL] + SLEEP_PERIOD;
    iodata = FPGA_inl(ioaddr);
    iodata &= ~0xffff0000;
    iodata |= (set << 16);
    FPGA_outl(ioaddr, iodata);
} 

void setAntDonorSel(int val)
{
  	unsigned int ctrl;
  	unsigned int ioaddr;
    struct psa_struct 	*ppsa;

	ppsa = getPsaDev();
  
  	ioaddr = ppsa->psa_reg_base + PSA_CFG0;

  	ctrl = FPGA_inl(ioaddr);
  	ctrl = (val == 0) ? (ctrl & ~ANT_DONOR_SEL) : (ctrl | ANT_DONOR_SEL);
	
//	printf("setAntDonorSel[%08X]:%08X[%d]\n",ioaddr,ctrl,val);
  	FPGA_outl(ioaddr,ctrl);
}

int getAntDonorSel()
{
    unsigned int ctrl;
    unsigned int ioaddr;
    struct psa_struct 	*ppsa;

	ppsa = getPsaDev();    
    ioaddr = ppsa->psa_reg_base + PSA_CFG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & ANT_DONOR_SEL) ? 1:0;
}


int getAntAutoOnOff()
{
    unsigned int ctrl;
    unsigned int ioaddr;
    struct psa_struct 	*ppsa;

	ppsa = getPsaDev();     
    ioaddr = ppsa->psa_reg_base + PSA_CFG0;
    ctrl = FPGA_inl(ioaddr);

	return (ctrl & ANT_AUTO_ONOFF) ? 1:0;
}

char *_onoff[2] = {"Off","On"};
char *_path[2] = {"DL","UL"};

void setDlTddMode(struct ics_struct *pdev,int path, int onoff)
{
    unsigned int ctrl;
    unsigned int ioaddr;
//	struct ics_struct *pdev	;
	
#if 0    
    ioaddr = pdev->ics_reg_base[DL] + TDD_CFG_1;
    ctrl = FPGA_inl(ioaddr);
	ctrl = (onoff == 0) ? (ctrl & ~TDD_EXT) : (ctrl | TDD_EXT);
    
    FPGA_outl(ioaddr,ctrl);
#endif
	#if 1
	{
		unsigned int ctrl;
		unsigned int ioaddr;
		char *str;

		if (onoff == 0) {
			ioaddr = pdev->ics_reg_base[path] + TDD_CFG_1;
			ctrl = FPGA_inl(ioaddr);
			ctrl = (ctrl | 0x01000000);
			FPGA_outl(ioaddr,ctrl);
		}
		else 
		{
			ioaddr = pdev->ics_reg_base[path] + TDD_CFG_1;
			ctrl = FPGA_inl(ioaddr);
			ctrl = (ctrl & ~0x01000000);
			FPGA_outl(ioaddr,ctrl);
		}

#if DEBUG_PRINT
		printf("Force[%d] %d > [%x]\n", path,onoff,ctrl);
#endif
	}
	#endif
}

void setPwmDac(int channel, unsigned short dac)
{
    struct psa_struct *pdev;
    unsigned int iodata;
    unsigned int offset;
    unsigned int mask;

    pdev = getPsaDev();

    switch (channel) {

    case 0:
        offset = 0x70;
        mask   = 0xFFFF0000;
        break;

    case 1:
        offset = 0x74;
        mask   = 0x0000FFFF;
        break;

    case 2:
        offset = 0x74;
        mask   = 0xFFFF0000;
        break;

    case 3:
        offset = 0x78;
        mask   = 0x0000FFFF;
        break;

    case 4:
        offset = 0x78;
        mask   = 0xFFFF0000;
        break;

    default:
        printf("Invalid PWM channel: %d\n", channel);
        return;
    }

    iodata = FPGA_inl(pdev->psa_reg_base + offset);

    iodata &= ~mask;

    if (mask == 0xFFFF0000)
        iodata |= ((unsigned int)dac << 16);
    else
        iodata |= (unsigned int)dac;
    FPGA_outl(pdev->psa_reg_base + offset, iodata);

}

int getPwmDac(int channel)
{
    struct psa_struct *pdev;
    unsigned int iodata;
    unsigned int offset;
    unsigned int shift;

    pdev = getPsaDev();

    switch (channel) {

    case 0:
        offset = 0x70;
        shift = 16;
        break;

    case 1:
        offset = 0x74;
        shift = 0;
        break;

    case 2:
        offset = 0x74;
        shift = 16;
        break;

    case 3:
        offset = 0x78;
        shift = 0;
        break;

    case 4:
        offset = 0x78;
        shift = 16;
        break;

    default:
        printf("Invalid PWM channel: %d\n", channel);
        return -1;
    }

    iodata = FPGA_inl(pdev->psa_reg_base + offset);

    return (iodata >> shift) & 0xFFFF;
}
/* 
 * loadIcsInfo()
 * Filesystem으로 부터 ICS 정보를 읽어온다. 
 */
struct ics_info 				iinfo;
extern void saveICSTbl(struct ics_info *pinfo);
extern void loadICSTbl(struct ics_info *pinfo);

int loadIcsInfo(struct ics_struct *pdev)
{
	int						fd,sb,path,band;
//	struct ics_info 		pinfo;
	char					fname[256];
	
//	memset((void *)fname, 0, sizeof(fname));
//	sprintf(fname, "/data/app/%s_%s",pdev->bd_name, pdev->sb_name);
//	fd=open(fname,O_RDONLY);
//	if(0 < fd) {
//		read(fd,(char *)&pinfo,sizeof(struct ics_info));
//		close(fd);
//    }
//    else {
//		printf("[load] failed to open %s_%s file ics\n",pdev->bd_name, pdev->sb_name);
//		return -1;
//    }
	
	/* 저장할 때는 살아 있는 SB에 대해서만 업데이트를 한다. */
#if DEBUG_PRINT
//	printf("loadIcsInfo:%s_%s\n",pdev->bd_name, pdev->sb_name);
	printf("loadIcsInfo\n");

#endif
	loadICSTbl(&iinfo);


	for(path=0;path<2;path++){
		/* Load from File System */
		pdev->ics_onoff[path] 	= iinfo.ics_onoff[path];
		pdev->alc_onoff[path] 	= iinfo.alc_onoff[path];
		pdev->agc_onoff[path] 	= iinfo.agc_onoff[path];
		pdev->att_gain[path] 	= iinfo.att_gain[path];
		pdev->alc_setp[path] 	= iinfo.alc_setp[path];
		pdev->agc_setp[path] 	= iinfo.agc_setp[path];
		pdev->usr_gain[path] 	= iinfo.usr_gain[path];
		pdev->init_gain[path] 	= iinfo.init_gain[path];			
		pdev->fa_lvl[path]		= iinfo.fa_lvl[path];
		pdev->delay1[path]		= iinfo.delay1[path];
		pdev->delay2[path]		= iinfo.delay2[path];
	}
	pdev->fc				= iinfo.fc;

  	return (1);
}

/* 
 * saveIcsInfo()
 * Filesystem으로 부터 중계기 정보를 저장한다.
 */
 
void saveIcsInfo(struct ics_struct *pdev)
{
	int						fd,err,sb,path,band;
//	struct ics_info 		pinfo;

	char					fname[128];

   /* Store To File System */
//	memset((void *)fname, 0, sizeof(fname));
//	sprintf(fname, "/data/app/%s_%s",pdev->bd_name, pdev->sb_name);
	
	/* 저장할 때는 살아 있는 SB에 대해서만 업데이트를 한다. */
#if DEBUG_PRINT
//	printf("saveIcsInfo:%s_%s\n",pdev->bd_name, pdev->sb_name);
	printf("saveIcsInfo\n");
#endif

	for(path=0;path<2;path++){
		/* Load from File System */
		iinfo.ics_onoff[path] 	= pdev->ics_onoff[path];
		iinfo.alc_onoff[path] 	= pdev->alc_onoff[path];
		iinfo.agc_onoff[path] 	= pdev->agc_onoff[path];
		iinfo.att_gain[path] 	= pdev->att_gain[path];
		iinfo.alc_setp[path] 	= pdev->alc_setp[path];
		iinfo.agc_setp[path] 	= pdev->agc_setp[path];
		iinfo.usr_gain[path] 	= pdev->usr_gain[path];	
		iinfo.init_gain[path] 	= pdev->init_gain[path];	
		iinfo.fa_lvl[path]		= pdev->fa_lvl[path];
		iinfo.delay1[path]		= pdev->delay1[path];
		iinfo.delay2[path]		= pdev->delay2[path];
	}
	iinfo.fc					= pdev->fc;

	saveICSTbl(&iinfo);
}

/* 
 * saveIcsInfoDef()
 * Filesystem에 파일이 존재하지 않을 경우 중계기 설정의 기본값을 저장한다 .
 */


void saveIcsInfoDef(struct ics_struct *pdev)
{
	int 	sb,path,band;
	
	for(path=0;path<2;path++){
		pdev->alc_onoff[path] 	= 1;
		pdev->agc_onoff[path]  	= 1;	    		
		pdev->att_gain[path]  	= 0;
	
		pdev->agc_setp[path]  	= -2000;
		pdev->usr_gain[path]  	=  0; 
		pdev->init_gain[path] 	= 0;
	
		pdev->alc_setp[path]  	= -1800;  		
	}
	/* fa_lvl[DL]은 다운링크 최대 파워 레벨을 명시함. 이 파워 이상 나가면, 다운링크 출력 바로 죽음.
	 * fa_lvl[UL]은 업링크 슬립 레벨을 명시함. 이 파워 이하이면, Sleep Mode로 들어감. .
	 */
	pdev->fa_lvl[DL]			= -1500; 
	pdev->fa_lvl[UL]			= -9000; 	

	pdev->agc_setp[0] 	= -3400; // '0' : DL
	pdev->agc_setp[1] 	= -2400; // '1' : UL
		 
	//if (strcmp(pdev->ics_name, "BAND_1.8G") == 0){
	//	pdev->fc[SB0]			= -275;
	//	pdev->fc[SB1]			= -75;
	//	pdev->fc[SB2]			= +125;
	//}
	//
	//if (strcmp(pdev->ics_name, "BAND_2.1G") == 0){
	//	pdev->fc[SB0]			= -275;
	//	pdev->fc[SB1]			= -75;
	//	pdev->fc[SB2]			= +125;
	//}
	saveIcsInfo(pdev);
}
