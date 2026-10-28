//#include <stdlib.h>
//#include <string.h>
//#include <math.h>
//#include "config.h"
//#include "genlib.h"
//#include "fpga.h"
//#include "ics.h"
//#include "config.h"
//#include "genlib.h"
//#include "psa.h"
//#include "cli.h"
//#include "dac_blk.h"
//#include "xparameters.h"
////#include "mb_interface.h"
//#include "xil_io.h"
//
//#define	MAX_DAC			1
//
//static int CurrentDev = 0;
//
//static char *DacName[MAX_DAC] = {"DAC3484"};
//
//struct dac_struct dac;
//	
//struct dac_struct *getDacDev(void)
//{
//	return (&dac);
//}
//
//extern void init_dac3484(struct dac_struct *pdac, int band);
//extern void init_dac37j84(struct dac_struct *pdac);
//
//void init_dac(void) 
//{
//	int i;
//	printf("init_dac3484\n");
//
//	dac.dac_name = DacName[i];
//	dac.dac_id = 0;
//	init_dac3484(&dac,0);
//};
//
//void run_dac(void) 
//{
//	dac.txDac(&dac,1);
//};
//
//#if 1
//char *DacHelp[] = {                                                       
//	"DAC ",
//	"[application_info]",
//	0,                                                                    
//};                                                                            
//                                                                              
//int
//Dac(int argc, char *argv[])
//{                                                                             
//    char 			*arg1,*arg2;
//    int 			opt;
//	int 			i;
//	int 			addr,wdata,rdata;
//	int 			val=0;
//
//    while((opt=getopt(argc,argv,"a:d:")) != -1) {
//        switch (opt) {
//            case 'a':
//            	addr = (int)strtoul(optarg,0,0);
//                break;
//            case 'd':
//            	wdata = (int)strtoul(optarg,0,0);
//                break;
//            default:
//                return(0);
//        }
//    }
//
//    arg1 = argv[optind];
//    arg2 = argv[optind+1];
//	if (strcmp(arg1, "init") == 0) {
//		init_dac();
//	}
//	else if (strcmp(arg1, "wr") == 0) {
//		dac.writeDacReg(CurrentDev,addr,wdata,1);
//		rdata = dac.readDacReg(CurrentDev,addr);
//		printf("0x%04x : %02x\n", addr, rdata);
//	}
//	else if (strcmp(arg1, "reset") == 0) {
//		if (strcmp(arg2, "on") == 0) {
//			dac.reset_dac(CurrentDev,1);
//		}
//		else if (strcmp(arg2, "off") == 0) {
//			dac.reset_dac(CurrentDev,0);
//		}
//	}
//	else if (strcmp(arg1, "reg") == 0) {
//		dac.showRegInfo(&dac);
//	}
//	else if (strcmp(arg1, "info") == 0) {
//		dac.showDacInfo(&dac);
//	}
//	//else if (strcmp(arg1, "dev") == 0) {
//    //    int tmpdev;
//    //    if (argc == 3) {
//    //        tmpdev = atoi(argv[2]);
//    //        if (tmpdev < 3) {
//    //            CurrentDac = tmpdev;
//    //        	printf("Subsequent dac ops apply to dac #%d..\n",CurrentDac);
//	//		}
//    //    }
//    //    else 
//    //        printf("Current dac dev: %d\n",CurrentDac);
//	//}
//	//else if (strcmp(arg1, "interp") == 0) {
//	//	configDacInterp(val);
//	//}
//	//else if (strcmp(arg1, "clkdly") == 0) {
//	//	configDacClkDly(val);
//	//}
//	//else if (strcmp(arg1, "datadly") == 0) {
//	//	configDacDataDly(val);
//	//}
//	//else if (strcmp(arg1, "mixen") == 0) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		configDacMixEn(1);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		configDacMixEn(0);
//	//	}
//	//}
//	//else if (strcmp(arg1, "ncoen") == 0) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		configDacNcoEn(1);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		configDacNcoEn(0);
//	//	}
//	//}
//	//else if (strcmp(arg1, "cmix") == 0) {
//	//	configDacCmix(val);
//	//}
//	//else if (strcmp(arg1, "foffset") == 0) {
//	//	configDacFifoOffset(val);
//	//}
//	else if (strcmp(arg1, "clr") == 0) {
//		//spiWriteDacDev(0x05,0x0);
//	}
//	//else if (strcmp(arg1, "fifo") == 0) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		configDacFifoEn(1);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		configDacFifoEn(0);
//	//	}
//	//}
//	else if (strcmp(arg1, "tx") == 0) {
//		dac.txDac(&dac);
//	}
//	else if (strcmp(arg1, "sync") == 0) {
//		if (strcmp(arg2, "+") == 0) {
//			//DAC_SYNC(0,1);
//		}
//		else if (strcmp(arg2, "-") == 0) {
//			//DAC_SYNC(0,0);
//		}
//	}
//	else if (strcmp(arg1, "data") == 0) {
//		if (strcmp(arg2, "+") == 0) {
//			setDatadelay(1);
//		}
//		else if (strcmp(arg2, "-") == 0) {
//			setDatadelay(0);
//		}
//	}
//	else if (strcmp(arg1, "clk") == 0) {
//		if (strcmp(arg2, "+") == 0) {
//			setClockdelay(1);
//		}
//		else if (strcmp(arg2, "-") == 0) {
//			setClockdelay(0);
//		}
//	}
//    else if (!strcmp(arg1,"psc")) {
//		ushort iotest;
//		if (strcmp(arg2, "+") == 0) {
//			val = dac.setClkPsc(&dac,1,&iotest);
//			printf("PSC Value[%3d],iotest_result[%04x]\n", val,iotest);
//		}
//		else if (strcmp(arg2, "-") == 0) {
//			val = dac.setClkPsc(&dac,0,&iotest);
//			printf("PSC Value[%3d],iotest_result[%04x]\n", val,iotest);
//		}
//    }
//
//    return(CMD_SUCCESS);
//}
//#endif
