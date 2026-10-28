//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "psa.h"
#include "cli.h"
#include "lmk0480x.h"

/************************************************************************** 
 *	LMK04805 제어 함수
 *
 **************************************************************************/
extern char syslock[4];

void init_sysclk(void)
{
	struct psa_struct 	*pdev;
	unsigned int 		ioaddr,iodata;
	int					i;

	pdev = getPsaDev();

	/* SYS_RST 해제. sys_clk/dac_clk 블록 동작 개시. */
	ioaddr = pdev->psa_reg_base + PSA_CFG2;
    iodata = FPGA_inl(ioaddr);
    iodata |= 0x00008000;
	FPGA_outl(ioaddr,iodata);
    iodata &= ~0x00008000;
	FPGA_outl(ioaddr,iodata);

	ioaddr = pdev->psa_reg_base + PSA_CFG0;
	iodata = FPGA_inl(ioaddr);
	for (i=0;i<9999;i++) {
    	iodata = FPGA_inl(ioaddr);
    	syslock[0] = (iodata & 0x00010000) ? 1:0;
    	syslock[1] = (iodata & 0x00020000) ? 1:0;
    	syslock[2] = (iodata & 0x00040000) ? 1:0;
    	syslock[3] = (iodata & 0x00080000) ? 1:0;

    	if ((iodata & 0x00010000) && (iodata & 0x00020000)) {
    		/* wait until LOCKED0='1' & LOCKED1='1'*/
			printf("Locked @%4d : [%d][%d][%d][%d]\n",i,syslock[0],syslock[1],syslock[2],syslock[3]);
			break;
    	}
	}
	for (;i<9999;i++) {
    	iodata = FPGA_inl(ioaddr);
    	syslock[0] = (iodata & 0x00010000) ? 1:0;
    	syslock[1] = (iodata & 0x00020000) ? 1:0;
    	syslock[2] = (iodata & 0x00040000) ? 1:0;
    	syslock[3] = (iodata & 0x00080000) ? 1:0;
    	if (!(iodata & 0x00080000)) {
    		/* HRESET -> 0 */
			printf("HRESET @%4d : [%d][%d][%d][%d]\n",i,syslock[0],syslock[1],syslock[2],syslock[3]);
			break;
    	}
	}
	for (;i<9999;i++) {
    	iodata = FPGA_inl(ioaddr);
    	syslock[0] = (iodata & 0x00010000) ? 1:0;
    	syslock[1] = (iodata & 0x00020000) ? 1:0;
    	syslock[2] = (iodata & 0x00040000) ? 1:0;
    	syslock[3] = (iodata & 0x00080000) ? 1:0;
	}
	if (i == 9999) {
		printf("Status @%4d : [%d][%d][%d][%d]\t\n",i,syslock[0],syslock[1],syslock[2],syslock[3]);
	}
}

