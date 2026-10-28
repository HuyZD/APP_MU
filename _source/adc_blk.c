//#include <stdlib.h>
//#include <string.h>
//#include <math.h>
//#include "config.h"
//#include "genlib.h"
//#include "fpga.h"
//#include "ics.h"
//#include "psa.h"
//#include "cli.h"
//#include "adc_blk.h"
//
//#define	NUM_ADC			1
//
//static int CurrentDev = 0;
//static char *AdcName[NUM_ADC] = {"ADS4246-160MSPS"};
//
//struct adc_struct adc[NUM_ADC];
//	
//struct adc_struct *getAdcDev(int id)
//{
//	return (&adc[id]);
//}
//
//int init_adc(void) 
//{
//	int status = 0;
//	int i;
//	int ret[2] = {0,};
//	for (i=0;i<NUM_ADC;i++) {
//		printf("Initialize ADC[%d]\n",i);
//
//		adc[i].adc_name = AdcName[i];
//		adc[i].adc_id = i;
//		init_ads4246(&adc[i]);
//	}
//	
//	return status;
//};
//
//int run_adc(void) 
//{
//	int i;
//	for (i=0;i<NUM_ADC;i++) {
//		if (isAdcAlive(i)) {
//			adc[i].rxAdc(&adc[i]);
//		}
//	}
//	return	0;
//
//};
//
//char *AdcHelp[] = {                                                       
//	"ADS4246",
//	"[application_info]",
//	0,                                                                    
//};                                                                            
//
//int
//Adc(int argc, char *argv[])
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
//		if (strcmp(arg2, "all") == 0) {
//			for (i=0;i<NUM_ADC;i++) {
//				adc[i].init_adc(&adc[i],1);
//			}
//		}
//		else {
//			adc[CurrentDev].init_adc(&adc[CurrentDev],1);
//		}
//	}
//	else if (strcmp(arg1, "dev") == 0) {
//        int tmpdev;
//        if (argc == 3) {
//            tmpdev = atoi(argv[2]);
//            if (tmpdev < NUM_ADC) {
//                CurrentDev = tmpdev;
//			}
//            printf("Subsequent ADC ops apply to dev %d\n",CurrentDev);
//        }
//        else 
//            printf("Current ADC dev: %d\n",CurrentDev);
//	}
//	else if (strcmp(arg1, "reg") == 0) {
//		if (strcmp(arg2, "all") == 0) {
//			for (i=0;i<NUM_ADC;i++) {
//				adc[i].showRegInfo(&adc[i]);
//			}
//		}
//		else {
//			adc[CurrentDev].showRegInfo(&adc[CurrentDev]);
//		}
//	}
//	else if (strcmp(arg1, "info") == 0) {
//		adc[CurrentDev].showAdcInfo(&adc[CurrentDev]);
//	}
//	else if (strcmp(arg1, "reset") == 0) {
//		adc[CurrentDev].reset_adc(&adc[CurrentDev]);
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
//    else if (!strcmp(arg1,"psc")) {
//		ushort adc_data[4];
//		if (strcmp(arg2, "+") == 0) {
//			val = adc[CurrentDev].setClkPsc(&adc[CurrentDev],1,adc_data);
//			printf("PSC Value[%3d] = 0x%04x:0x%04x:0x%04x:0x%04x\n", val,adc_data[0],adc_data[1],adc_data[2],adc_data[3]);
//		}
//		else if (strcmp(arg2, "-") == 0) {
//			val = adc[CurrentDev].setClkPsc(&adc[CurrentDev],0,adc_data);
//			printf("PSC Value[%3d] = 0x%04x:0x%04x:0x%04x:0x%04x\n", val,adc_data[0],adc_data[1],adc_data[2],adc_data[3]);
//		}
//    }
//	//else {
//	//	showAdcReg();
//	//}
//
//    return(CMD_SUCCESS);
//}
//
