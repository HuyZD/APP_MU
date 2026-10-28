//#include "config.h"
//#include "genlib.h"
//#include "stddefs.h"
//#include "cli.h"
//#include "fpga.h"
//#include "psa.h"
//#include "ads4246.h"
//
//#if 1
//#define	ADC_CTRL	0x0000
//#define	ADC_STAT	0x0004
//#define	ADC_DIN		0x0018
//#define	ADC_DOUT	0x001C
//
//#define	ADC_INIT	0x0010
//#define	ADC_DONE	0x0200
//#define	ADC_NWR		0x0400
//#define	ADC_SEL		0xF000
//
///* ADS4246 I/F
// */
//static int spiWriteAdcDev(ushort id, ushort addr, ushort data, int verbose)
//{
//    struct psa_struct 	*pdev;
//	unsigned int 		ctrl=0,i;
//
//    pdev = getPsaDev();
//
//	ctrl = ((addr << 8) | data);
//	FPGA_outl(pdev->psa_reg_base + ADC_DIN, ctrl);
//	ctrl = FPGA_inl(pdev->psa_reg_base + ADC_CTRL);
//	ctrl &= ~ADC_SEL;
//	ctrl |= (id << 12);
//	ctrl &= ~ADC_INIT;
//	FPGA_outl(pdev->psa_reg_base + ADC_CTRL, ctrl);
//	ctrl |= ADC_INIT;
//	ctrl &= ~ADC_NWR;
//	FPGA_outl(pdev->psa_reg_base + ADC_CTRL, ctrl);
//	for (i=0;i<64;i++) {										/* wait until ADC_DONE cleared */
//		ctrl = FPGA_inl(pdev->psa_reg_base + ADC_CTRL);
//		if (ctrl & ADC_DONE) {
//			break;
//		}
//	}
//	ctrl &= ~ADC_INIT;
//	FPGA_outl(pdev->psa_reg_base + ADC_CTRL, ctrl);
//	if (i == 64) {
//		printf("ADC Write Failed\n");
//		return (-1);
//	}
//
//	if (verbose) {
//		printf("ADC[%d] @0x%02x <- %02x\n",id, addr, data);
//	}
//	return (0);
//}
//static int spiReadAdcDev(ushort id, ushort addr)
//{
//	struct psa_struct 		*pdev;
//	unsigned int 			ctrl=0,i;
//	unsigned int 			data = 0;
//
//    pdev = getPsaDev();
//	spiWriteAdcDev(id, 0x00, 0x01,0);
//	ctrl = (addr << 8);
//	FPGA_outl(pdev->psa_reg_base + ADC_DIN, ctrl);
//	ctrl = FPGA_inl(pdev->psa_reg_base + ADC_CTRL);
//	ctrl &= ~ADC_SEL;
//	ctrl |= (id << 12);
//	ctrl &= ~ADC_INIT;
//	FPGA_outl(pdev->psa_reg_base + ADC_CTRL, ctrl);
//	ctrl |= ADC_INIT;
//	ctrl |= ADC_NWR;
//	FPGA_outl(pdev->psa_reg_base + ADC_CTRL, ctrl);
//	for (i=0;i<64;i++) {										/* wait until ADC_DONE cleared */
//		ctrl = FPGA_inl(pdev->psa_reg_base + ADC_CTRL);
//		if (ctrl & ADC_DONE) {
//			data = FPGA_inl(pdev->psa_reg_base + ADC_DOUT);
//			data = (data & 0xff);
//			break;
//		}
//	}
//	ctrl &= ~ADC_INIT;												//
//	FPGA_outl(pdev->psa_reg_base + ADC_CTRL, ctrl);
//	spiWriteAdcDev(id, 0x00, 0x00, 0);
//	if (i == 64) {
//		printf("ADC Read Failed\n");
//		return (-1);
//	}
//
//	return (int)data;
//}
//
///*	cal_adc()
// * 	Analog ADCs
// *	id		: 0,1,2 -> ADC 0,1,2
// *	clkdiv	: "000"->Pin Control, "001"->div by 2, "010"->div by 3, "011"->div by 4
// *	outmode	: "000"->LVDS 3mA (Default), "001"->LVDS 2mA, "100"->LVCMOS
// *	format	: "000"->2��s Complement (Default), "010"->Gray Code, "100"->Offset Binary
// *	dco		: '1'->invert,'0'->normal
// *	dcodly	: '1'->invert,'0'->normal
// */
//static void cal_adc(void)
//{
//}
//static void showRegInfo(void)
//{
//	int i,j,data;
//
//	printf("[ADS4246]\n");
//	printf("%7s"," ");
//	for (i=0;i<16;i++) {
//		printf("%02x ",i);
//	}
//	printf("\n");
//    for (i=0;i<16;i++) {
//   		printf("0x%04x:", i*16);
//   		for (j=0;j<16;j++) {
//   			//if ((i*16+j)==0) {
//   			//	printf("xx ");
//   			//}
//   			//else {
//   				data = spiReadAdcDev(0, i*16+j);
//   				if (data == -1) {
//   					return;
//   				}
//   				printf("%02x ", data);
//   			//}
//   		}
//		printf("\n");
//    }
//	printf("\n");
//}
//static void showAdcInfo(void)
//{
//}
//
//static struct ads4246_struct ads4246;
//
////static char gainA=ADC_GAIN_2_5;		//	B3
////static char gainB=ADC_GAIN_2_5;		//	B28
//static char gainA=ADC_GAIN_1_5;		//	B3
//static char gainB=ADC_GAIN_1_0;		//	B28
//
//
//static char freqA=1;
//static char freqB=1;
//static char format=0;
//static char dco=0;
//static char dcodly=16;
//
//void init_adc(int verbose)
//{
//	struct ads4246_struct	*pdev;
//
//	pdev = (struct ads4246_struct *)&ads4246;
//
//	pdev->REG00._bitval.READOUT					= 0;
//	pdev->REG00._bitval.RESET					= 1;
//	pdev->REG00._bitval.RSVD					= 0;
//	spiWriteAdcDev(0, 0x00, pdev->REG00._intval, verbose);
//
//	pdev->REG01._bitval.RSVD					= 0;
//	pdev->REG01._bitval.LVDS_SWING				= 0x0;		/* 0x14 = "010100" -> ±570 mV */
//	spiWriteAdcDev(0, 0x01, pdev->REG01._intval, verbose);
//
//	pdev->REG02._bitval.RSVD					= 0;
//	pdev->REG02._bitval.HIGH_PERF_MODE_3		= 1;
//	pdev->REG02._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0x02, pdev->REG02._intval, verbose);
//
//	pdev->REG03._bitval.HIGH_PERF_MODE_1		= 1;
//	pdev->REG03._bitval.HIGH_PERF_MODE_2		= 1;
//	pdev->REG03._bitval.RSVD					= 0;
//	spiWriteAdcDev(0, 0x03, pdev->REG03._intval, verbose);
//
//	pdev->REG25._bitval.CH_A_TEST_PATTERNS		= 3;		/* 0->Normal, 1->All '0', 2->All '1', 3->Toggle, 5->Custom Pattern*/
//	pdev->REG25._bitval.RSVD					= 0;
//	pdev->REG25._bitval.CH_A_GAIN				= gainA;	/* "0000"-> 0dB, "0001"->0.5dB, */
//	spiWriteAdcDev(0, 0x25, pdev->REG25._intval, verbose);
//
//	pdev->REG29._bitval.RSVD					= 0;
//	pdev->REG29._bitval.DATA_FORMAT				= format;	/* "00"->2's complement, "11"->Offset Binary */
//	pdev->REG29._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0x29, pdev->REG29._intval, verbose);
//
//	pdev->REG2B._bitval.CH_B_TEST_PATTERNS		= 3;
//	pdev->REG2B._bitval.RSVD					= 0;
//	pdev->REG2B._bitval.CH_B_GAIN				= gainB;	/* "0000"-> 0dB, "0001"->0.5dB, */
//	spiWriteAdcDev(0, 0x2B, pdev->REG2B._intval, verbose);
//
//	pdev->REG3D._bitval.RSVD					= 0;
//	pdev->REG3D._bitval.ENABLE_OFFSET_CORR		= 1;		/* This bit enables the offset correction. */
//	pdev->REG3D._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0x3D, pdev->REG3D._intval, verbose);
//
//	pdev->REG3F._bitval.CUSTOM_PATTERN_D		= 0x2A;
//	pdev->REG3F._bitval.RSVD					= 0;
//	spiWriteAdcDev(0, 0x3F, pdev->REG3F._intval, verbose);
//
//	pdev->REG40._bitval.CUSTOM_PATTERN_D		= 0xAA;
//	spiWriteAdcDev(0, 0x40, pdev->REG40._intval, verbose);
//
//	pdev->REG41._bitval.DIS_OBUF				= 0;
//	pdev->REG41._bitval.RSVD					= 0;
//	pdev->REG41._bitval.CMOS_CLKOUT_STRENGTH	= 0;
//	pdev->REG41._bitval.LVDS_CMOS				= 0x2;		/* "00","01","10"->LVDS, "11"->CMOS */
//	spiWriteAdcDev(0, 0x41, pdev->REG41._intval, verbose);
//
//	pdev->REG42._bitval.RSVD					= 0;
//	pdev->REG42._bitval.EN_DIGITAL				= 1;		/* 1 = All digital functions (such as test patterns, gain, and offset correction) enabled */
//	pdev->REG42._bitval.CLKOUT_RISE_POSN		= 0;
//	pdev->REG42._bitval.CLKOUT_FALL_POSN		= 0;
//	spiWriteAdcDev(0, 0x42, pdev->REG42._intval, verbose);
//
//	pdev->REG45._bitval.RSVD					= 0;
//	pdev->REG45._bitval.PDN_GLOBAL				= 0;
//	pdev->REG45._bitval.RSVD2					= 0;
//	pdev->REG45._bitval.LVDS_DATA_STRENGTH		= 1;
//	pdev->REG45._bitval.LVDS_CLKOUT_STRENGTH	= 1;
//	pdev->REG45._bitval.STBY					= 0;
//	spiWriteAdcDev(0, 0x45, pdev->REG45._intval, verbose);
//
//	pdev->REG4A._bitval.HIGH_FREQ_MODE_CH_B	= freqB;
//	pdev->REG4A._bitval.RSVD					= 0;
//	spiWriteAdcDev(0, 0x4A, pdev->REG4A._intval, verbose);
//
//	pdev->REG58._bitval.HIGH_FREQ_MODE_CH_A	= freqA;
//	pdev->REG58._bitval.RSVD					= 0;
//	spiWriteAdcDev(0, 0x58, pdev->REG58._intval, verbose);
//
//	pdev->REGBF._bitval.RSVD					= 0;
//	pdev->REGBF._bitval.CH_A_OFFSET_PEDESTAL	= 0;
//	spiWriteAdcDev(0, 0xBF, pdev->REGBF._intval, verbose);
//
//	pdev->REGC1._bitval.RSVD					= 0;
//	pdev->REGC1._bitval.CH_B_OFFSET_PEDESTAL	= 0;
//	spiWriteAdcDev(0, 0xC1, pdev->REGC1._intval, verbose);
//
//	pdev->REGCF._bitval.RSVD					= 0;
//	pdev->REGCF._bitval.OFFSET_CORR_TIME_CONSTANT	= 0;
//	pdev->REGCF._bitval.RSVD2					= 0;
//	pdev->REGCF._bitval.FREEZE_OFFSET_CORR	= 0;
//	spiWriteAdcDev(0, 0xCF, pdev->REGCF._intval, verbose);
//
//	pdev->REGEF._bitval.RSVD					= 0;
//	pdev->REGEF._bitval.EN_LOW_SPEED_MODE		= 0;
//	pdev->REGEF._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0xEF, pdev->REGEF._intval, verbose);
//
//	pdev->REGF1._bitval.EN_LVDS_SWING			= 0;
//	pdev->REGF1._bitval.RSVD					= 0;
//	spiWriteAdcDev(0, 0xF1, pdev->REGF1._intval, verbose);
//
//	pdev->REGF2._bitval.RSVD					= 0;
//	pdev->REGF2._bitval.LOW_SPEED_MODE_CH_A		= 0;
//	pdev->REGF2._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0xF2, pdev->REGF2._intval, verbose);
//
//	pdev->REGD5._bitval.RSVD					= 0;
//	pdev->REGD5._bitval.HIGH_PERF_MODE_5		= 1;
//	pdev->REGD5._bitval.HIGH_PERF_MODE_4		= 1;
//	pdev->REGD5._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0xD5, pdev->REGD5._intval, verbose);
//
//	pdev->REGD7._bitval.RSVD					= 0;
//	pdev->REGD7._bitval.HIGH_PERF_MODE_7		= 1;
//	pdev->REGD7._bitval.HIGH_PERF_MODE_6		= 1;
//	pdev->REGD7._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0xD7, pdev->REGD7._intval, verbose);
//
//	pdev->REGDB._bitval.LOW_SPEED_MODE_CH_B		= 0;
//	pdev->REGDB._bitval.RSVD					= 0;
//	pdev->REGDB._bitval.HIGH_PERF_MODE_8		= 1;
//	pdev->REGDB._bitval.RSVD2					= 0;
//	spiWriteAdcDev(0, 0xDB, pdev->REGDB._intval, verbose);
//
//	cal_adc();
//	showRegInfo();
//    showAdcInfo();
//}
//
//static void setAdcPwrDown(uchar pd)
//{
//	struct ads4246_struct	*pdev;
//
//	pdev = (struct ads4246_struct *)&ads4246;
//
//	pdev->REG45._bitval.RSVD					= 0;
//	pdev->REG45._bitval.PDN_GLOBAL				= pd;
//	pdev->REG45._bitval.RSVD2					= 0;
//	pdev->REG45._bitval.LVDS_DATA_STRENGTH		= 0;
//	pdev->REG45._bitval.LVDS_CLKOUT_STRENGTH	= 0;
//	pdev->REG45._bitval.STBY					= 0;
//	spiWriteAdcDev(0, 0x45, pdev->REG45._intval,0);
//}
//static void setAdcOutMode(uchar outmode)
//{
//	struct ads4246_struct	*pdev;
//
//	pdev = (struct ads4246_struct *)&ads4246;
//
//	pdev->REG25._bitval.CH_A_TEST_PATTERNS		= 0;			/* 0->Normal, 1->All '0', 2->All '1', 3->Toggle, 5->Custom Pattern*/
//	pdev->REG25._bitval.RSVD					= 0;
//	pdev->REG25._bitval.CH_A_GAIN				= gainA;		/* "0000"-> 0dB, "0001"->0.5dB, */
//	spiWriteAdcDev(0, 0x25, pdev->REG25._intval,0);
//
//	pdev->REG2B._bitval.CH_B_TEST_PATTERNS		= 0;
//	pdev->REG2B._bitval.RSVD					= 0;
//	pdev->REG2B._bitval.CH_B_GAIN				= gainB;		/* "0000"-> 0dB, "0001"->0.5dB, */
//	spiWriteAdcDev(0, 0x2B, pdev->REG2B._intval,0);
//}
//
//#define	DCB_ON		0x0400
//#define	PS_INIT		0x0001
//#define	PS_INC		0x0002
//#define	PS_DONE		0x0004
//
//static int			psc = 0;
//
//static int setClkPsc(uchar inc)
//{
//	struct psa_struct 	*pdev;
//    unsigned int 		ioaddr;
//    unsigned int 		i,iodata;
//
//	pdev = getPsaDev();
//	ioaddr = pdev->psa_reg_base + 0x4;
//
//	if (inc) {
//    	iodata = FPGA_inl(ioaddr);
//    	/* PS_INIT */
//    	iodata &= ~PS_INIT;
//    	FPGA_outl(ioaddr,iodata);
//    	iodata |= PS_INIT;
//    	iodata |= PS_INC;
//    	FPGA_outl(ioaddr,iodata);
//    	/* PS_DONE */
//    	for(i=0;i<16;i++) {
//    		iodata = FPGA_inl(ioaddr);
//    		if (iodata & PS_DONE) {
//    			break;
//    		}
//    	}
//    	if (i==16) {
//    		printf("PS_DONE Failed\n",i);
//    	}
//    	psc += 1;
//    }
//    else {
//    	iodata = FPGA_inl(ioaddr);
//    	iodata &= ~PS_INIT;
//    	FPGA_outl(ioaddr,iodata);
//    	iodata |= PS_INIT;
//    	iodata &= ~PS_INC;
//    	FPGA_outl(ioaddr,iodata);
//    	for(i=0;i<16;i++) {
//    		iodata = FPGA_inl(ioaddr);
//    		if (iodata & PS_DONE) {
//    			break;
//    		}
//    	}
//    	if (i==16) {
//    		printf("PS_DONE Failed\n",i);
//    	}
//    	psc -= 1;
//    }
//    return psc;
//}
///* 0 0 1 1 */
///* 1 0 0 1 */
///* 1 1 0 0 */
///* 0 1 1 0 */
//static ushort mask[2][2] = {
//	{0xaaa8,0x5554},
//	{0x5554,0xaaa8},
//};
//
//static void dcb_on(int onoff)
//{
//	struct psa_struct 	*pdev;
//	unsigned int 		ctrl;
//
//	pdev = getPsaDev();
//	ctrl = FPGA_inl(pdev->psa_reg_base+ADC_STAT);
//	ctrl = (onoff == 1) ? (ctrl | DCB_ON) : (ctrl & ~DCB_ON);
//	FPGA_outl(pdev->psa_reg_base + ADC_STAT, ctrl);
//}
//
///* isPatternMatched().
// */
//static int isPatternMatched(void)
//{
//	struct psa_struct 	*pdev;
//	unsigned int 		pattern[2];
//	int					i;
//
//	pdev = getPsaDev();
//	for (i=0;i<32;i++) {
//		pattern[0] = FPGA_inl(pdev->psa_reg_base+0x20);
//		pattern[1] = FPGA_inl(pdev->psa_reg_base+0x24);
//		if (memcmp((void *)&pattern[0], (void *)&mask[0][0], 4) != 0 &&
//			memcmp((void *)&pattern[0], (void *)&mask[1][0], 4) != 0) {
//			//printf("%08x %08x \n",pattern[0],pattern[1]);
//			return (0);
//		}
//	}
//	//printf("pattern >> %04x%04x\n", pattern[0],pattern[1]);
//
//	return (1);
//}
//
//int rxAdc(void)
//{
//	int		center=0;
//	int		idx=0;
//	int		start=0,end=0;
//	int		timeout;
//	int 	redge = 0, fedge=0;
//
//	printf("rxAdc()....\n");
//	if (isPatternMatched()) {
//		printf("Pattern Matched at first!!\n");
//		/*	1.
//		 *  2.
//		 *  3.
//		 */
//		for (timeout=0;timeout<32;timeout++) {
//			idx = setClkPsc(1);							// phase�� ������Ų��.
//			if (isPatternMatched()) {
//				continue;
//			}
//			else {
//				end = idx;
//				break;
//			}
//		}
//		if (timeout==32) {
//			printf("+matched at every phase[%2d] !!\n", idx);
//			end = idx;
//		}
//		else {
//			setClkPsc(0);								// 1 additional phase decreament.
//		}
//
//		for (timeout=0;timeout<32;timeout++) {
//			idx = setClkPsc(0);							// phase�� ���ҽ�Ų��.
//			if (isPatternMatched()) {
//				continue;
//			}
//			else {
//				if (++redge==2) {
//					start = idx;
//					break;
//				}
//			}
//			if (idx==0) {
//				start = idx;
//				break;
//			}
//		}
//		center = (start + end)/2;
//		printf("+ %3d<-->%3d<-->%3d\n",start,center,end);
//		while (1) {
//			if (center == idx) {
//				break;
//			}
//			idx = setClkPsc(1);
//			printf("+");
//		}
//		printf("\n");
//	}
//	else {
//		printf("Pattern UnMatched at first!!\n");
//		for (timeout=0;timeout<32;timeout++) {
//			idx = setClkPsc(1);							// phase�� ������Ų��.
//			if (!isPatternMatched()) {
//				redge = 0;
//				printf("%02d+",idx);
//				continue;
//			}
//			else {
//				printf("-");
//				if (++redge==2) {
//					start = idx;
//					break;
//				}
//			}
//		}
//		if (timeout==32) {
//			printf("- err step1\n");
//			return (0);
//		}
//
//		for (;timeout<32;timeout++) {
//			idx = setClkPsc(1);							// phase�� ���ҽ�Ų��.
//			if (isPatternMatched()) {
//				fedge = 0;
//				continue;
//			}
//			else {
//				if (++fedge==2) {
//					end = idx;
//					break;
//				}
//			}
//		}
//		if (timeout==32) {
//			//printf("- err step2\n");
//			//return (0);
//			printf("-matched at the end of phase[%2d] !!\n", idx);
//			end = idx;
//		}
//		center = (start + end)/2;
//		printf("- %3d<-->%3d<-->%3d\n",start,center,end);
//		while (1) {
//			if (center == idx) {
//				break;
//			}
//			idx = setClkPsc(0);
//			printf("-");
//		}
//		printf("\n");
//	}
//
//	setAdcOutMode(0);
//	dcb_on(1);
//
//	return (1);
//}
//
//char *AdcHelp[] = {
//	"ADS4246",
//	"[application_info]",
//	0,
//};
//
//int
//AdcCmd(int argc, char *argv[])
//{
//    char 			*arg1;
//    char			*arg2;
//    int 			opt;
//	int 			i;
//	int 			val=0;
//
//    while((opt=getopt(argc,argv,"v:")) != -1) {
//        switch (opt) {
//            //case 'v':
//            //	val = (int)strtoul(optarg,0,0);
//            //    break;
//            default:
//                return(0);
//        }
//    }
//
//    arg1 = argv[optind];
//    arg2 = argv[optind+1];
//	if (strcmp(arg1, "init") == 0) {
//		init_adc(1);
//	}
//	else if (strcmp(arg1, "reg") == 0) {
//		showRegInfo();
//	}
//	else if (strcmp(arg1, "info") == 0) {
//		showAdcInfo();
//	}
//    //else if (!strcmp(arg1,"gain0")) {
//	//}
//    //else if (!strcmp(arg1,"test")) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		setAdcOutMode(0x8);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		setAdcOutMode(0x0);
//	//	}
//	//}
//    //else if (!strcmp(arg1,"dcb")) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		dcb_on(1);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		dcb_on(0);
//	//	}
//	//}
//    //else if (!strcmp(arg1,"psc")) {
//	//	ushort adc_data[4];
//	//	if (strcmp(arg2, "+") == 0) {
//	//		val = setClkPsc(&adc[CurrentDev],1,adc_data);
//	//		printf("PSC Value[%3d] = 0x%04x:0x%04x:0x%04x:0x%04x\n", val,adc_data[0],adc_data[1],adc_data[2],adc_data[3]);
//	//	}
//	//	else if (strcmp(arg2, "-") == 0) {
//	//		val = adc[CurrentDev].setClkPsc(&adc[CurrentDev],0,adc_data);
//	//		printf("PSC Value[%3d] = 0x%04x:0x%04x:0x%04x:0x%04x\n", val,adc_data[0],adc_data[1],adc_data[2],adc_data[3]);
//	//	}
//    //}
//
//    return(CMD_SUCCESS);
//}
//
//#endif
