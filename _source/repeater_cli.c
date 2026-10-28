/*	repeater_cli.c
 *	Repeater API
 */
//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"
#include "psa.h"
#include "rf.h"

#include "tbl.h"
#include "protocol_private.h"

#include "xuartlite.h"
#include "xuartlite_l.h"
#include "xuartps.h"

#include "flash.h"
#if INCLUDE_REPEATER

char *OnOff[2] = {"Off","On"};

extern int		IWDG_TEST_FLAG;	
extern char	*band_name[];


void modem_PWRKey(uchar onoff)
{
	unsigned int		ioaddr,iodata;
    struct psa_struct 	*ppsa;
	unsigned int 		ctrl;

	ppsa = getPsaDev();

	ioaddr = ppsa->psa_reg_base+ 0x00;
    ctrl = FPGA_inl(ioaddr);

	iodata = (onoff == 1) ? (ctrl | (0x1 << 26)) : (ctrl & ~(0x1 << 26));

 	FPGA_outl(ioaddr,iodata);
}

void modem_RST(uchar onoff)
{
	unsigned int		ioaddr,iodata;
    struct psa_struct 	*ppsa;
	char 				idx = 0;
	unsigned int 		ctrl;

	ppsa = getPsaDev();

	ioaddr = ppsa->psa_reg_base+ 0x00;
    ctrl = FPGA_inl(ioaddr);

	iodata = (onoff == 1) ? (ctrl | (0x1 << 27)) : (ctrl & ~(0x1 << 27));

 	FPGA_outl(ioaddr,iodata);
}


void init_modem(void)
{
	modem_PWRKey(0);
	modem_RST(0);

	vTaskDelay(100); //1 second
	printf("init_modem1\n");

	modem_PWRKey(1);
	modem_RST(1);

	vTaskDelay(100);
	modem_PWRKey(0);
	printf("init_modem2\n");

	vTaskDelay(100);
	modem_PWRKey(1);
	printf("init_modem3\n");

}



void showRptInfo(struct repeater_struct *prpt, int band)
{
	int sb;
	char 	fwd[32],rvs[32];
	
	printf("%16s|","Tree");
	printf("\n");
	printf("  %d|",prpt->Tree.sau_pri_mu);
	printf("  %d|",prpt->Tree.sau_pri_ru[0][0]);
	printf("  %d|",prpt->Tree.sau_pri_ru[0][1]);
	printf("  %d|",prpt->Tree.sau_pri_ru[0][2]);
	printf("  %d|",prpt->Tree.sau_pri_ru[0][3]);
	printf("\n");
	printf("  %d|",prpt->Tree.sau_pri_mu);
	printf("  %d|",prpt->Tree.sau_pri_ru[1][0]);
	printf("  %d|",prpt->Tree.sau_pri_ru[1][1]);
	printf("  %d|",prpt->Tree.sau_pri_ru[1][2]);
	printf("  %d|",prpt->Tree.sau_pri_ru[1][3]);
	printf("\n");
	printf("  %d|",prpt->Tree.sau_pri_mu);
	printf("  %d|",prpt->Tree.sau_pri_ru[2][0]);
	printf("  %d|",prpt->Tree.sau_pri_ru[2][1]);
	printf("  %d|",prpt->Tree.sau_pri_ru[2][2]);
	printf("  %d|",prpt->Tree.sau_pri_ru[2][3]);
	printf("\n");
	printf("  %d|",prpt->Tree.sau_pri_mu);
	printf("  %d|",prpt->Tree.sau_pri_ru[3][0]);
	printf("  %d|",prpt->Tree.sau_pri_ru[3][1]);
	printf("  %d|",prpt->Tree.sau_pri_ru[3][2]);
	printf("  %d|",prpt->Tree.sau_pri_ru[3][3]);
	printf("\n");


}


//static char *bandType[2] = {"LTE","WCDMA"};
//static char *pathType[2] = {"DL","UL"};
//static char *downType[2] = {"DNC","UPC"};  

char *RptHelp[] = {
    "Rpt",
    "-[v:] -[frduk]",
    "",
    "Options:",
    "-f forward path",
    "-r reverse path",
    "-d down converter",
    "-u up converter",
    "-k feedback",
    "-v value    specifiy value",
    "",
    "Operations:",
    "gain, offset, info",
    "",
    "NOTE: none",
    0
};

extern int tcp_Alarm_flag;



// UART0: Modem protocol data
#define UART0_DEVICE_ID    		XPAR_UARTLITE_0_DEVICE_ID

// UART1: Modem AT command
#define UART1_DEVICE_ID    		XPAR_UARTLITE_1_DEVICE_ID

XUartLite UartLite_TR;        /* The instance of the NMS UartLite Device */
XUartLite UartLite_IR_BR1;        /* The instance of the ATC UartLite Device */

// 占쌓쏙옙트占쏙옙 占쏙옙占쏙옙占쏙옙 크占쏙옙 (AXI UART Lite占쏙옙 占썩본 FIFO占쏙옙 16占쏙옙占쏙옙트占쌉니댐옙)
#define TEST_BUFFER_SIZE 16


static u8 SendBuffer[TEST_BUFFER_SIZE]; // 占쌜쏙옙 占쏙옙占쏙옙
static u8 RecvBuffer[TEST_BUFFER_SIZE]; // 占쏙옙占쏙옙 占쏙옙占쏙옙
u8 Debug_Prn = 0;

int ATC_UartLite_LoopbackTest(u16 DeviceId) {
    int Status;
    unsigned int SentCount = 0;
    unsigned int RecvCount = 0;
    int Index;

    // 1. Initialize UART Lite
	Status = XUartLite_Initialize(&UartLite_IR_BR1, UART1_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		printf("ATC XUartLite_Initialize ATC fail\r\n");
		return XST_FAILURE;
	}

    // 2. Preparing send/receive buffer data
    for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
        SendBuffer[Index] = 'A' + Index; // Test data in the form of A', B', C
        RecvBuffer[Index] = 0;           // Receive buffer initializes to zero
    }

    printf("ATC Starting UART Loopback Test...\r\n");

    // 3. Send Data (Polling Method)
    // XUartLite_Send占쏙옙 Non-blocking 占쌉쇽옙占싱므뤄옙, 占쏙옙占� 占쏙옙占쏙옙占싶몌옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쌥븝옙占쌌니댐옙.
    while (SentCount < TEST_BUFFER_SIZE) {
        SentCount += XUartLite_Send(&UartLite_IR_BR1, SendBuffer + SentCount, TEST_BUFFER_SIZE - SentCount);
    }

    // 4. Receive Data (Polling Method)
    // It will read as many times as it has been sent until the data enters the RX FIFO.
    while (RecvCount < TEST_BUFFER_SIZE) {
        RecvCount += XUartLite_Recv(&UartLite_IR_BR1, RecvBuffer + RecvCount, TEST_BUFFER_SIZE - RecvCount);
    }

    // 5. Verifying the comparison of transmitted and received data
    for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
        if (SendBuffer[Index] != RecvBuffer[Index]) {
            printf("ATC Test Failed! Error at Index %d: Sent '%c', Received '%c'\r\n",
                       Index, SendBuffer[Index], RecvBuffer[Index]);
            return XST_FAILURE;
        }
		printf("0 ATC Test Index %d: Sent '%c', Received '%c'\r\n",
				   Index, SendBuffer[Index], RecvBuffer[Index]);
		
    }

    xil_printf("1 ATC Test Passed! Successfully received all loopback data.\r\n");
    return XST_SUCCESS;
}



int NMS_UartLite_LoopbackTest(u16 DeviceId) {
    int Status;
    unsigned int SentCount = 0;
    unsigned int RecvCount = 0;
    int Index;

    // 1. Initialize UART Lite
	Status = XUartLite_Initialize(&UartLite_TR, UART0_DEVICE_ID);
	
	if (Status != XST_SUCCESS) {
		printf("NMS XUartLite_Initialize ATC fail\r\n");
		return XST_FAILURE;
	}

    // 2. Preparing send/receive buffer data
    for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
        SendBuffer[Index] = 'A' + Index; // Test data in the form of A', B', C
        RecvBuffer[Index] = 0;           // Receive buffer initializes to zero
    }

    printf("NMS Starting UART Loopback Test...\r\n");

    // 3. Send Data (Polling Method)
    // XUartLite_Send占쏙옙 Non-blocking 占쌉쇽옙占싱므뤄옙, 占쏙옙占� 占쏙옙占쏙옙占싶몌옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쌥븝옙占쌌니댐옙.
    while (SentCount < TEST_BUFFER_SIZE) {
        SentCount += XUartLite_Send(&UartLite_TR, SendBuffer + SentCount, TEST_BUFFER_SIZE - SentCount);
    }

    // 4. Receive Data (Polling Method)
    // It will read as many times as it has been sent until the data enters the RX FIFO.
    while (RecvCount < TEST_BUFFER_SIZE) {
        RecvCount += XUartLite_Recv(&UartLite_TR, RecvBuffer + RecvCount, TEST_BUFFER_SIZE - RecvCount);
    }

    // 5. Verifying the comparison of transmitted and received data
    for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
        if (SendBuffer[Index] != RecvBuffer[Index]) {
            printf("NMS Test Failed! Error at Index %d: Sent '%c', Received '%c'\r\n",
                       Index, SendBuffer[Index], RecvBuffer[Index]);
            return XST_FAILURE;
        }
		printf("0 NMS Test Index %d: Sent '%c', Received '%c'\r\n",
		   Index, SendBuffer[Index], RecvBuffer[Index]);
    }

    xil_printf("1 NMS Test Passed! Successfully received all loopback data.\r\n");
    return XST_SUCCESS;
}




extern int FlashReadID(void);
extern void FlashSubSectorErase();
extern void FlashTest();

extern void Download_load(int _Addr,unsigned int size, u8* buf);
static char xFlash_buf[1024*1024*7] = {0,};	 //2048
static char yFlash_buf[1024*1024*7] = {0,};	 //2048
extern void saveTemp(void);
extern void DefaultBackup(struct repeater_struct *pdev);

int 
Rpt(int argc, char *argv[]) {
	char *arg1,*arg2;
    int opt;
	int val;
	int band;
	int	path=0;
	int	downup=0;
	int sb = 0;
	char fwd[32],rvs[32];
	struct repeater_struct *prpt;
	//struct lte_struct 	*ldev;
	//struct ics_struct 	*idev;

	prpt = getRptDev();
	
    if (argc < 2) {
#if DEBUG_PRINT
        printf("No arguments, exiting... use help\n");
#endif
        return(0);
    }

    while((opt=getopt(argc,argv,"v:dufr")) != -1) {
        switch (opt) {;
            case 'd':					/* Forward 	*/
				path = 0;
                break;
            case 'u':					/* Reverse 	*/
				path = 1;
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
    
   if (strcmp(arg1, "band") == 0) {
        int tmpband;
        if (argc == 3) {
            tmpband = atoi(argv[2]);
            if (tmpband < 4) {
                CurrentBand = tmpband;
            	printf("Subsequent Att ops apply to band %s(%d)\n",band_name[CurrentBand],CurrentBand);
			}
        }
        else 
           	printf("Current Band %s(%d)\n",band_name[CurrentBand],CurrentBand);
	}
	
	if (strcmp(arg1, "init") == 0) {
	}
	else if (strcmp(arg1, "att") == 0) {	
		printf("att [%d][%d]\n",path,val);
	}	
	else if (strcmp(arg1, "hpa") == 0) {
		printf("hpa[%d][%d]\n",path,val);
		
	}
	else if (strcmp(arg1, "path") == 0) {
		if (strcmp(arg2, "on") == 0) {
			if (!path) {
			}
			else {
			}
		}
		else if (strcmp(arg2, "off") == 0) {
			if (!path) {
			}
			else {
			}
		}

	}	
	else if (strcmp(arg1, "info") == 0) {
		showRptInfo(prpt, CurrentBand);
	}
	else if (strcmp(arg1, "oper") == 0) {
	}
	else if (strcmp(arg1, "def") == 0) {
		Def_Backup(prpt);
		printf("Flash Default \n");
	}
	else if (strcmp(arg1, "ship") == 0) {
		DefaultBackup(prpt);
		saveRepeaterInfo(prpt);
		loadRepeaterInfo(prpt);
		printf("Ship Default \n");
	}
	else if (strcmp(arg1, "fload") == 0) {
		printf("Flash load \n");
		Download_load(SPIF_APPZ_IMAGE_BASE,409608,xFlash_buf);
		Download_load(SPIF_APPX_IMAGE_BASE,409608,yFlash_buf);

	#if 0	
		for(int i =0 ; i < 10;i++)
		{
			printf("Flash load[%d][%x] \n",i,Flash_buf[i]);

		}
	#endif
	}
	else if (strcmp(arg1, "fsave") == 0) {
		printf("Flash Save \n");
		Download_write(409608,yFlash_buf);
	}
	else if (strcmp(arg1, "fcomp") == 0) {
		uint32_t size;
		uint32_t cnt_A = 0,cnt_B = 0;
		unsigned short			xCrc_gen,yCrc_gen;

		printf("Flash Comp[%d] \n",val);
		size = 409608;

		for( int i = 0; i < size ; i++){
			if(xFlash_buf[i] == yFlash_buf[i])
			{
		//		printf("0 flash Comp[%d]:[%x][%x]\n",i,xFlash_buf[i],info_buf_Y[i]);
				cnt_A++;
			}
			else 
			{
				cnt_B++;
				if(path == 1)
					printf("1 flash Comp[%d]:[%x][%x]\n",i,xFlash_buf[i],yFlash_buf[i]);
			}
		}
		xCrc_gen=Generate_CRC((char *)xFlash_buf, size);
		yCrc_gen=Generate_CRC((char *)yFlash_buf, size);
		printf("2 flash Comp[%d]:[%d]>[0x%x][0x%x]\n",cnt_A,cnt_B,xCrc_gen,yCrc_gen);
	}
	else if (strcmp(arg1, "recall") == 0) {
		printf("Flash Recall \n");

		saveRepeaterInfoDef(prpt);
		saveRepeaterInfo(prpt);
		loadRepeaterInfo(prpt);
	}

	else if (strcmp(arg1, "load") == 0) {
		printf("Flash load \n");

		loadRepeaterInfo(prpt);
	}
	else if (strcmp(arg1, "erase") == 0) {
		printf("Flash earse \n");

		FlashSubSectorErase();
	}
	else if (strcmp(arg1, "test") == 0) {
		printf("Flash test \n");
		FlashTest();
	}
	else if (strcmp(arg1, "flash") == 0) {
		printf("Flash test \n");

	}
	else if (strcmp(arg1, "ferase") == 0) {
		printf("Flash earse 0 \n");

		QSPI_Erase(SPIF_FILE_SAVE_BASE);
	}
	else if (strcmp(arg1, "mode") == 0) {
		unsigned char			pl_mode,app_mode;

		getBootInfo((char *)&pl_mode, (char *)&app_mode);

		if (strcmp(arg2, "app") == 0) {
			BootInfoTbl(pl_mode,val);
		}

		if (strcmp(arg2, "pl") == 0) {
			BootInfoTbl(val,app_mode);
		}

		getBootInfo((char *)&pl_mode, (char *)&app_mode);
		
		
	}

	else if (strcmp(arg1, "dnload") == 0) {
		unsigned char			pl_mode,app_mode;

		Flash_write();
	}

	else if (strcmp(arg1, "temp") == 0) {

	}

	else if (strcmp(arg1, "at") == 0) {
#if DEBUG_PRINT
		printf("AT cmd   : %d	\n",	val);
#endif

	}
	else if (strcmp(arg1, "modemkey") == 0) {
		if (strcmp(arg2, "on") == 0) {
			modem_PWRKey(1);
		}
		else if (strcmp(arg2, "off") == 0) {
			modem_PWRKey(0);
		}
	}
	else if (strcmp(arg1, "modemrst") == 0) {
		if (strcmp(arg2, "on") == 0) {
			modem_RST(1);
		}
		else if (strcmp(arg2, "off") == 0) {
			modem_RST(0);
		}
	}
	else if (strcmp(arg1, "spi") == 0) {
		FlashReadID();
	}
	else if (strcmp(arg1, "test") == 0) {
	//	spi_test();
	}
	else if (strcmp(arg1, "led") == 0) {
		
	}
	else if (strcmp(arg1, "earfcn") == 0) {
		if (strcmp(arg2, "set") == 0) {
			setFilterType(prpt,val,CurrentBand);
		}
	}
	else if (strcmp(arg1, "iso") == 0) {
		
	}	
	else if (strcmp(arg1, "atc") == 0) {
		ATC_UartLite_LoopbackTest(0);
	}
	else if (strcmp(arg1, "nms") == 0) {
		NMS_UartLite_LoopbackTest(0);
	}
	else if (strcmp(arg1, "loop") == 0) {
		ATC_UartLite_LoopbackTest(0);
		NMS_UartLite_LoopbackTest(0);
	}
	else if (strcmp(arg1, "down") == 0) {
		

	}
	else if (strcmp(arg1, "debug") == 0) {
		Debug_Prn = val;

		printf("Debug_Prn [%d] \n",Debug_Prn);
	}

	return (0);
}
#endif
