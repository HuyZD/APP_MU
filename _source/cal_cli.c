#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "ics_private.h"
#include "psa.h"
#include "cal.h"

//char *_CalHelp[] = {
//    "Cal",
//    "-[c:] -[v:] -[p:]",
//    "",
//    "Options:",
//    "-c channel  specifiy fa 1 ~ 4",
//    "-v value    specifiy value",
//    "-p path     fwd or rvs path",
//    "",
//    "Operations:",
//    "band,dev,sb,coe",
//	"on/off,coe", 
//	"info", 
//	"dly1,dly2,dly", 
//	"load,save", 
//    "",
//    "NOTE: none",
//    0
//};
//
//int 
//_Cal(int argc, char *argv[])
//{
//    char 				*arg1,*arg2;
//    int 				opt;
//    int 				rval=0;
//    int 				i;
//	int 				path=0,val=0;
//    char    			*fname=NULL;
//	long				fsize = 0;
//	char    			*f_buffer;
//	struct cal_struct 	*pdev;
//	
//	/* File Save */
//	int					fd;
//
//	int ival,fval;
//	
//    if (argc < 2) {
//#if DEBUG_PRINT
//        printf("No arguments, exiting... use help\n");
//#endif
//        return(0);
//    }
//
//	pdev = getCalDev(CurrentCal);
//
//    while((opt=getopt(argc,argv,"duc:v:F:")) != -1) {
//        switch (opt) {
//        case 'F':
//            fname = optarg;
//            break;
//        case 'd':
//            path = DL;
//            break;
//        case 'u':
//            path = UL;
//            break;
//        case 'v':
//        	val = (int)strtoul(optarg,0,0);
//            break;
//        default:
//            return(0);
//        }
//    }
//    
//    arg1 = argv[optind];
//    arg2 = argv[optind+1];
//    
//    if (strcmp(arg1, "band") == 0 || strcmp(arg1, "dev") == 0) {
//        int tmpdev;
//        if (argc == 3) {
//            tmpdev = atoi(argv[2]);
//            if (tmpdev < 2) {
//                CurrentCal = tmpdev;
//			}
//            printf("Subsequent ics ops apply to dev %d\n",CurrentCal);
//        }
//        else 
//            printf("Current ics dev: %d\n",CurrentCal);
//	    return(0);
//	}
//	else if (strcmp(arg1, "load") == 0) {
////		struct cal_file coe;
//    	
//		//fd = open("/data/app/cal.coe",O_RDWR|O_SYNC|O_CREAT,777);
//		//if(fd > 0) {
//    	//	fsize=read(fd,(void *)&coe,sizeof(struct cal_file));
//		//	close(fd);
//    	//	
//		//	printf("Nco : %2d\n",(int)coe.nco);
//		//	printf("Size : %2d\n",(int)coe.size);
//		//	for (i=0;i<coe.size;i++) {
//		//		printf("%4d> %6d %6d|%6d %6d|\n",i,coe.hi[DL][i],coe.hq[DL][i],coe.hi[UL][i],coe.hq[UL][i]);
//		//	}
//    	//}
//    	//else {
//		//	printf( "read() open cal.coe fail\n");
//    	//}
//	}
// 	else if (strcmp(arg1, "save") == 0) {
////		char 			buf[32];
//		int 			num;
//		struct cal_file coe;
//        
//		num = getCalCoe(pdev,&coe);
//
//		printf("\n%5s"," ");
//		printf("%s\n","|<--Downlink-->|<---Uplink-->|");
//		printf("\n");
//		for (i=0;i<num;i++) {
//			printf("%4d> %6d %6d|%6d %6d|\n",i,coe.hi[DL][i],coe.hq[DL][i],coe.hi[UL][i],coe.hq[UL][i]);
//		}
//  		
//  		/* Store To File System */
//		//fd = open("/data/app/cal.coe",O_RDWR|O_SYNC|O_CREAT,777);
//		//
//		//if(fd > 0) {
//		//	write(fd,(void *)&coe,sizeof(struct cal_file));
//		//	close(fd);
//    	//}
//    	//else {
//		//	printf("write() open cal.coe fail\n");
//    	//}
//	}
//	else if (strcmp(arg1, "force") == 0) {
//		pdev = getCalDev(CurrentCal);
//		loadCalCoe(pdev);
//	}
//	else if (strcmp(arg1, "init") == 0) {
//		startCal(pdev, path, 1);
//	}
//    return(rval);
//}
