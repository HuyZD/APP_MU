//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "psa.h"

int		CurrentBand = 0;

/*----------------------------------------------------------------------
 *	isBandAlive()
 * 	Description : FPGA는 총 4개의 band까지 지원할 수 있다. 
 * 			현재 FPGA가 지원하는 band가 있는지 조회한다. 
 * 	Return : 
 * 			0 -> Dead, 	1->	Alive
 *---------------------------------------------------------------------*/
//int isBandAlive(int band)
//{
//	unsigned int 		ioaddr;
//	unsigned int 		iodata;
//	unsigned int 		mask[4] = {0x00001000,0x00002000,0x00004000,0x00008000};
//	struct psa_struct 	*pdev;
//	
//	pdev = getPsaDev();
//	
//	ioaddr = pdev->psa_reg_base + 0x0;
//	iodata = FPGA_inl(ioaddr);
//
//	return ((iodata & mask[band]) ? 1:0);
//}

/* isBandValid()
 * FPGA 내부의 레지스터에는 HW적으로 지원하는 Band를 지원하는지를 가리키는 레지스터가 있다. 
 * PSA_REG0
 */
int isBandAlive(int band)
{
    struct psa_struct 	*ppsa;
	int					ioaddr,iodata;

	ppsa 	= getPsaDev();
	ioaddr 	= ppsa->psa_reg_base+PSA_CFG0;
	iodata 	= FPGA_inl(ioaddr);

    switch (band) { 
    case 0:					
		return ((iodata & 0x1000) ? 1:0);	/* BAND0 800M */
    case 1:
    	return ((iodata & 0x2000) ? 1:0);	/* BAND0 1.8G */
    case 2:
		return ((iodata & 0x4000) ? 1:0);	/* BAND0 2.1G */
    case 3:
		return ((iodata & 0x8000) ? 1:0);	/* BAND0 3.5G */
    default:
        return (-1);
    }
}
/* isAdcAlive()
 * FPGA 내부의 레지스터에는 FPGA가 최대 몇개까지 ADC를 지원하는지를 가리키는 레지스터가 있다. 
 * PSA_REG0
 */
int isAdcAlive(int band)
{
	unsigned int 		ioaddr;
	unsigned int 		iodata;
	unsigned int 		mask[4] = {0x00010000,0x00020000,0x00040000,0x00080000};
	struct psa_struct 	*pdev;
	
	pdev = getPsaDev();
	
	ioaddr = pdev->psa_reg_base + 0x0;
	iodata = FPGA_inl(ioaddr);

	return ((iodata & mask[band]) ? 1:0);
}
/* isDacAlive()
 * FPGA 내부의 레지스터에는 FPGA가 최대 몇개까지 ADC를 지원하는지를 가리키는 레지스터가 있다. 
 * PSA_REG0
 */
int isDacAlive(int band)
{
	unsigned int 		ioaddr;
	unsigned int 		iodata;
	unsigned int 		mask[4] = {0x00100000,0x00200000,0x00400000,0x00800000};
	struct psa_struct 	*pdev;
	
	pdev = getPsaDev();
	
	ioaddr = pdev->psa_reg_base + 0x0;
	iodata = FPGA_inl(ioaddr);

	return ((iodata & mask[band]) ? 1:0);
}
