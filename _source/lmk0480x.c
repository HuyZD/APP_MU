//#include "config.h"
//#include "stddefs.h"
////#include "genlib.h"
//#include "cli.h"
//#include "lmk0480x.h"
////#include "xstatus.h"
//#include "xspips.h"
//#include "xparameters.h"
//#include "gpio.h"
//
//static XSpiPs 					SpiInstance;
//static struct lmk0480x_struct 	lmk0480x;
//
//static void spiWriteClkDev(unsigned int val, int verbose, uchar special)
//{
//    u8 		tx[4];
//    u8 		rx[4];
//
//    tx[0] = (val >> 24) & 0xFF;
//    tx[1] = (val >> 16) & 0xFF;
//    tx[2] = (val >>  8) & 0xFF;
//    tx[3] = (val >>  0) & 0xFF;
//
//    XSpiPs_PolledTransfer(&SpiInstance, tx, rx, 4);
//
//    if (verbose)
//        printf("LMK W:0x%02X%02X%02X%02X\n",tx[0],tx[1],tx[2],tx[3]);
//
//    return;
//}
//static unsigned int spiReadClkDev(unsigned int addr)
//{
//    u8 				tx[4];
//    u8 				rx[4];
//    unsigned int	ret = 0;
//
//	lmk0480x.REG31._bitval.Address				= 0x1F;
//	lmk0480x.REG31._bitval.uWire_LOCK			= 0x0;
//	lmk0480x.REG31._bitval.RSVD1				= 0x0;
//	lmk0480x.REG31._bitval.READBACK_ADDR		= addr;
//	lmk0480x.REG31._bitval.READBACK_LE			= 0x0;
//	lmk0480x.REG31._bitval.RSVD2				= 0x0;		/* 0x001F001F */
//	spiWriteClkDev(lmk0480x.REG31._intval,0,0);
//
//    tx[0] = 0x00 | (addr & 0x1F);
//    tx[1] = 0;
//    tx[2] = 0;
//    tx[3] = 0;
//
//    XSpiPs_PolledTransfer(&SpiInstance, tx, rx, 4);
//
//	ret = (rx[0]<<25) | (rx[1]<<17) | (rx[2]<<9) | (rx[3]<<1);
//
//    return (ret | (addr & 0x1F));
//}
//
//static int CLK_LOCKED(void)
//{
//	extern XGpioPs 		Gpio;
//
//	return (XGpioPs_ReadPin(&Gpio, MIO_GPIO_LMK_LOCK));
//}
//
//static int CLK_SYNC(void)
//{
//	extern XGpioPs 		Gpio;
//
//	XGpioPs_WritePin(&Gpio, MIO_GPIO_LMK_SYNC, 0);
//	XGpioPs_WritePin(&Gpio, MIO_GPIO_LMK_SYNC, 1);
//	XGpioPs_WritePin(&Gpio, MIO_GPIO_LMK_SYNC, 0);
//}
//
//static void showClkReg(void)
//{
//	int i,j;
//
//	printf("%7s"," ");
//	for (i=0;i<8;i++) {
//		printf("%8x ",i);
//	}
//	printf("\n");
//    for (i=0;i<4;i++) {
//   		printf("0x%04x:", i*8);
//   		for (j=0;j<8;j++) {
//   			printf("%08x ", spiReadClkDev(i*8+j));
//   		}
//		printf("\n");
//    }
//}
//
//int init_clk(ushort mask, int verbose)
//{
//	//struct lmk0480x_struct lmk0480x;
//	int 			ctrl;
//	int 			i;
//
//    XSpiPs_Config 	*Config;
//    int 			Status;
//	static int 		SpiInitialized = 0;
//
//    if (!SpiInitialized) {
//    	Config = XSpiPs_LookupConfig(XPAR_XSPIPS_0_DEVICE_ID);
//    	if (Config == NULL)
//    	    return -1;
//
//    	Status = XSpiPs_CfgInitialize(&SpiInstance, Config, Config->BaseAddress);
//    	if (Status != XST_SUCCESS)
//    	    return -1;
//
//    	XSpiPs_SetOptions(&SpiInstance,
//    	        XSPIPS_MASTER_OPTION |
//    	        XSPIPS_FORCE_SSELECT_OPTION);
//
//    	XSpiPs_SetClkPrescaler(&SpiInstance, XSPIPS_CLK_PRESCALE_64);
//    	XSpiPs_SetSlaveSelect(&SpiInstance, 0);
//
//    	SpiInitialized = 1;
//    }
//
//	spiWriteClkDev(0x80160100,verbose,0);
//	spiWriteClkDev(0x00140180,verbose,0);		//0x00140180
//	spiWriteClkDev(0x00140041,verbose,0);       //0x00140041
//	spiWriteClkDev(0x00140802,verbose,0);       //0x00140802
//	spiWriteClkDev(0x001400C3,verbose,0);       //0x001400C3
//	spiWriteClkDev(0x001400C4,verbose,0);       //0x001400C4
//	spiWriteClkDev(0x001400C5,verbose,0);       //0x001400C5
//	spiWriteClkDev(0x04670006,verbose,0);       //0x04670006
//	spiWriteClkDev(0x10200007,verbose,0);       //0x10200007
//	spiWriteClkDev(0x50100008,verbose,0);       //0x50100008
//	spiWriteClkDev(0x55555549,verbose,0);       //0x55555549
//	spiWriteClkDev(0x9841412A,verbose,0);       //0x9841410A  (*)
//	spiWriteClkDev(0x0400100B,verbose,0);       //0x3400100B
//	spiWriteClkDev(0x138C006C,verbose,0);       //0x138C006C
//	spiWriteClkDev(0x1303E20D,verbose,0);       //0x1303E20D
//	spiWriteClkDev(0x0200000E,verbose,0);       //0x0200000E
//	spiWriteClkDev(0x8000800F,verbose,0);       //0x8000800F
//	spiWriteClkDev(0xC1550410,verbose,0);       //0xC1550410
//	spiWriteClkDev(0x00000058,verbose,0);       //0x00000058
//	spiWriteClkDev(0x02C9C419,verbose,0);       //0x02C9C419
//	spiWriteClkDev(0xAFA8001A,verbose,0);       //0xAFA8001A
//	spiWriteClkDev(0x10001F5B,verbose,0);       //0x10001F5B
//	spiWriteClkDev(0x0020601C,verbose,0);       //0x0020601C
//	spiWriteClkDev(0x008003DD,verbose,0);       //0x008003DD  (*)
//	spiWriteClkDev(0x020003DE,verbose,0);       //0x020003DE
//	spiWriteClkDev(0x001F001F,verbose,0);       //0x001F001F
//
//	for (i=0;i<1000000;i++) {
//		if (CLK_LOCKED()) {
//			printf("Locked(%d)!!\n",i);
//			break;
//		}
//	}
//	if (i==1000000) {
//		printf("UnLocked!!!!!!!!!!!!!!!!!!!!!!\n");
//	}
//
//	CLK_SYNC();
//
////	showClkReg();
//}
//
//char *ClkHelp[] = {
//	"CLK ",
//	"[application_info]",
//	0,
//};
//int
//ClkCmd(int argc, char *argv[])
//{
//    char 			*arg1;
//    char			*arg2;
//    int 			opt;
//	unsigned int 	data=0;
//	extern int 		optind;
//	extern char 	*optarg;
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
//
//    if (!strcmp(arg1,"init")) {
//		init_clk(0,1);
//	}
//    //else if (!strcmp(arg1,"reset")) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		reset_lmk(1);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		reset_lmk(0);
//	//	}
//	//}
//    else if (!strcmp(arg1,"write")) {
//   		data = strtoul(arg2,(char **)0,0);
//		spiWriteClkDev(data,1,0);
//    }
//    else if (!strcmp(arg1,"reg")) {
//		showClkReg();
//	}
//
//    return(CMD_SUCCESS);
//}
