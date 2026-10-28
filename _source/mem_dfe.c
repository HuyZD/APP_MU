#include <FreeRTOS.h>
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "config.h"
#include "genlib.h"
//#include "fpga.h"
#include "xparameters.h"
//#include "xintc.h"
#include "xil_exception.h"
#include "xil_io.h"
#include "stdlib.h"

void showMemory(uint32_t baseAddr, int len){
	int i,j;

	printf("%7s"," ");

	for (i=0;i<4;i++) {
		printf("%8x ",i*4);
	}

	printf("\n");

    for (i=0;i<len/4;i++) {
   		printf("0x%04x:", i*16);
   		for (j=0;j<4;j++) {
   			printf("%08x ", Xil_In32(baseAddr + i*16+j*4));
   		}
		printf("\n");
    }
}


char *DisplayMemoryHelp[] = {
    "DisplayMemory",
    "NOTE: none",
    0
};

int
DisplayMemory(int argc, char *argv[]){
    uint32_t addr = 0;
	int len = 0;
	int opt;
	int val;

    if (argc < 3) {
        printf("No arguments, exiting... use help\n");
        return(0);
    }

    while((opt=getopt(argc,argv,"l:a:")) != -1) {
	   switch (opt) {
		   case 'l':
			   len = (int)strtoul(optarg,0,0);
			   break;
		   case 'a':
			   addr = (uint32_t)strtoul(optarg,0,0);
			   break;
		   default:
			   return(0);
	   }
   }

    showMemory(addr, len);

	return (0);
}

void writeMemory_32(uint32_t addr, uint32_t val){
	int i,j;

	Xil_Out32(addr, val);
}
void writeMemory_16(uint32_t addr, uint32_t val){
	int i,j;

	Xil_Out16(addr, (uint16_t)val);
}
void writeMemory_8(uint32_t addr, uint32_t val){
	int i,j;

	Xil_Out8(addr, (uint8_t)val);
}

int
WriteMemory(int argc, char *argv[]){
    uint32_t addr = 0;
	int type = 8;
	int opt;
	int val;

    if (argc < 3) {
        printf("No arguments, exiting... use help\n");
        return(0);
    }

    while((opt=getopt(argc,argv,"v:a:124")) != -1) {
	   switch (opt) {
		   case 'v':
			   val = (uint32_t)strtoul(optarg,0,0);
			   break;
		   case 'a':
			   addr = (uint32_t)strtoul(optarg,0,0);
			   break;
		   case '1' :
			   type = 8;
			   break;
		   case '2' :
			   type = 16;
			   break;
		   case '4' :
			   type = 32;
			   break;
		   default:
			   return(0);
	   }
   }
    if(type == 8){
    	writeMemory_8(addr, val);
    }
    else if(type == 16){
    	writeMemory_16(addr, val);
    }
    else if(type == 32){
    	writeMemory_32(addr, val);
    }


	return (0);
}
