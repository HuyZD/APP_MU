//#include "config.h"
//#include "stddefs.h"
//#include "cli.h"
//#include "fpga.h"
//#include "psa.h"
//#include "dac3484.h"
////#include "xspips.h"
//#include "xparameters.h"
//#include "gpio.h"
//
//#define	DAC_NWR		0x800000
//
//static XSpiPs 		SpiInstance;
//extern XGpioPs 		Gpio;
//
///**************************************************************************
// *	DAC3484 제어 함수
// *
// **************************************************************************/
//static void DAC_RESET(unsigned char val)
//{
//	if (val == 1)
//		XGpioPs_WritePin(&Gpio, MIO_GPIO_DAC_RESETB, 1); 		//DAC_RESET High
//	else if (val == 0)
//		XGpioPs_WritePin(&Gpio, MIO_GPIO_DAC_RESETB, 0);		//DAC_RESET Low
//}
//static void DAC_SYNC(uchar val)
//{
//	struct psa_struct 		*ppsa;
//	unsigned int 			iodata;
//
//    ppsa = getPsaDev();
//	iodata = FPGA_inl(ppsa->psa_reg_base + 0x04);
//	iodata = val ? (iodata | 0x0800) : (iodata & ~0x0800);
//	FPGA_outl(ppsa->psa_reg_base + 0x04, iodata);
//}
//void DAC_TXEN(uchar val)
//{
//	if (val == 1)
//		XGpioPs_WritePin(&Gpio, MIO_GPIO_DAC_TXEN, 1); 		//DAC_TXEN High
//	else if (val == 0)
//		XGpioPs_WritePin(&Gpio, MIO_GPIO_DAC_TXEN, 0);		//DAC_TXEN Low
//}
//static void spiWriteDacDev(uchar id, ushort addr, ushort data, int verbose)
//{
//    u8 				TX[3];
//    u8 				RX[3];
//	unsigned int	val=0;
//
//	val |= (addr << 16);
//	val |= (data <<  0);
//	val &= ~DAC_NWR;
//
//    TX[0] = (val >> 16) & 0xFF;
//    TX[1] = (val >>  8) & 0xFF;
//    TX[2] = (val >>  0) & 0xFF;
//
//	//swap4(TX);
//
//    XSpiPs_PolledTransfer(&SpiInstance, TX, RX, 3);
//
//    if (verbose)
//        printf("DAC W:0x%08X\n",TX);
//
//    return;
//}
//
//static unsigned int spiReadDacDev(uchar id, unsigned int addr)
//{
//    u8 				TX[3];
//    u8 				RX[3];
//	unsigned int	val=0;
//	unsigned int	ret=0;
//
//	val |= (addr << 16);
//	val |= DAC_NWR;
//
//    TX[0] = (val >> 16) & 0xFF;
//    TX[1] = (val >>  8) & 0xFF;
//    TX[2] = (val >>  0) & 0xFF;
//
//    XSpiPs_PolledTransfer(&SpiInstance, TX, RX, 3);
//
//	ret = (RX[1]<<8) | (RX[2]<<0);
//
//    return (ret);
//}
//
//static struct dac3484_struct dac3484;
//static struct dac3484_config dac_default = {
//	2,				/* LPR DAC Interp */
//	1,				/* LPR DAC fifoen */
//	1,				/* LPR DAC mixen */
//	1,				/* LPR DAC ncoen */
//	0xC0000000,		/* LPR DAC Ch0 ncoval -230.4/921.6 */
//	0xC0000000,		/* LPR DAC Ch1 ncoval -230.4/921.6 */
//	0,				/* LPR DAC cmix */
//	4,				/* LPR DAC fifo_offset */
//	0,				/* LPR DAC clkdly */
//	7,				/* LPR DAC datadly,4 */
//};
//
///* loadDacDef()
// * load dac Defalut from Device */
//static void loadDac3484(void)
//{
//	dac3484.CONFIG00._intval = spiReadDacDev(0,0x00); dac3484.CONFIG01._intval = spiReadDacDev(0,0x01);
//	dac3484.CONFIG02._intval = spiReadDacDev(0,0x02); dac3484.CONFIG03._intval = spiReadDacDev(0,0x03);
//	dac3484.CONFIG04._intval = spiReadDacDev(0,0x04); dac3484.CONFIG05._intval = spiReadDacDev(0,0x05);
//	dac3484.CONFIG06._intval = spiReadDacDev(0,0x06); dac3484.CONFIG07._intval = spiReadDacDev(0,0x07);
//	dac3484.CONFIG08._intval = spiReadDacDev(0,0x08); dac3484.CONFIG09._intval = spiReadDacDev(0,0x09);
//	dac3484.CONFIG10._intval = spiReadDacDev(0,0x0A); dac3484.CONFIG11._intval = spiReadDacDev(0,0x0B);
//	dac3484.CONFIG12._intval = spiReadDacDev(0,0x0C); dac3484.CONFIG13._intval = spiReadDacDev(0,0x0D);
//	dac3484.CONFIG14._intval = spiReadDacDev(0,0x0E); dac3484.CONFIG15._intval = spiReadDacDev(0,0x0F);
//	dac3484.CONFIG16._intval = spiReadDacDev(0,0x10); dac3484.CONFIG17._intval = spiReadDacDev(0,0x11);
//	dac3484.CONFIG18._intval = spiReadDacDev(0,0x12); dac3484.CONFIG19._intval = spiReadDacDev(0,0x13);
//	dac3484.CONFIG20._intval = spiReadDacDev(0,0x14); dac3484.CONFIG21._intval = spiReadDacDev(0,0x15);
//	dac3484.CONFIG22._intval = spiReadDacDev(0,0x16); dac3484.CONFIG23._intval = spiReadDacDev(0,0x17);
//	dac3484.CONFIG24._intval = spiReadDacDev(0,0x18); dac3484.CONFIG25._intval = spiReadDacDev(0,0x19);
//	dac3484.CONFIG26._intval = spiReadDacDev(0,0x1A); dac3484.CONFIG27._intval = spiReadDacDev(0,0x1B);
//	dac3484.CONFIG28._intval = spiReadDacDev(0,0x1C); dac3484.CONFIG29._intval = spiReadDacDev(0,0x1D);
//	dac3484.CONFIG30._intval = spiReadDacDev(0,0x1E); dac3484.CONFIG31._intval = spiReadDacDev(0,0x1F);
//	dac3484.CONFIG32._intval = spiReadDacDev(0,0x20); dac3484.CONFIG33._intval = spiReadDacDev(0,0x21);
//	dac3484.CONFIG34._intval = spiReadDacDev(0,0x22); dac3484.CONFIG35._intval = spiReadDacDev(0,0x23);
//	dac3484.CONFIG36._intval = spiReadDacDev(0,0x24); dac3484.CONFIG37._intval = spiReadDacDev(0,0x25);
//	dac3484.CONFIG38._intval = spiReadDacDev(0,0x26); dac3484.CONFIG39._intval = spiReadDacDev(0,0x27);
//	dac3484.CONFIG40._intval = spiReadDacDev(0,0x28); dac3484.CONFIG41._intval = spiReadDacDev(0,0x29);
//	dac3484.CONFIG42._intval = spiReadDacDev(0,0x2A); dac3484.CONFIG43._intval = spiReadDacDev(0,0x2B);
//	dac3484.CONFIG44._intval = spiReadDacDev(0,0x2C); dac3484.CONFIG45._intval = spiReadDacDev(0,0x2D);
//	dac3484.CONFIG46._intval = spiReadDacDev(0,0x2E); dac3484.CONFIG47._intval = spiReadDacDev(0,0x2F);
//	dac3484.CONFIG48._intval = spiReadDacDev(0,0x30); dac3484.VERSION._intval  = spiReadDacDev(0,0x7F);
//}
//
//static void showRegInfo(void)
//{
//	int i,j;
//
//	printf("[DAC3484]");
//	printf("\n");
//	printf("%7s"," ");
//
//	for (i=0;i<16;i++) {
//		printf("%04x ",i);
//	}
//	printf("\n");
//    for (i=0;i<8;i++) {
//   		printf("0x%04x:", i*16);
//   		for (j=0;j<16;j++) {
//   			printf("%04x ", spiReadDacDev(0, i*16+j));
//   		}
//		printf("\n");
//    }
//}
//
//static void showDacInfo(void)
//{
//	char *OnOff[2] = {"Off", "On"};
//	struct dac3484_struct	*pdev;
//
//	pdev = (struct dac3484_struct *)&dac3484;
//
//	spiWriteDacDev(0,0x05,0x0,0);
//
//	loadDac3484();
//	printf("Interp        : %x\n", pdev->CONFIG00._bitval.interp);
//	printf("Quad          : %s\n", OnOff[pdev->CONFIG01._bitval.quad_ena]);
//	printf("NCO           : %s\n", OnOff[pdev->CONFIG02._bitval.nco_ena]);
//	printf("MixGain       : %s\n", OnOff[pdev->CONFIG02._bitval.mixer_gain]);
//	printf("Mix           : %s\n", OnOff[pdev->CONFIG02._bitval.mixer_ena]);
//	printf("bit16         : %s\n", OnOff[pdev->CONFIG02._bitval.bit16_in]);
//	printf("PLL Alarm     : %x\n", pdev->CONFIG05._bitval.alarm_from_pll);
//	printf("IO Test Alarm : %x\n", pdev->CONFIG05._bitval.alarm_from_iotest);
//	printf("Output Gone   : %x\n", pdev->CONFIG05._bitval.alarm_output_gone);
//	printf("DataClk Gone  : %x\n", pdev->CONFIG05._bitval.alarm_dataclk_gone);
//	printf("DacClk Gone   : %x\n", pdev->CONFIG05._bitval.alarm_dacclk_gone);
//	printf("FIFO Alarm    : %x\n", pdev->CONFIG05._bitval.alarms_from_fifo);
//	printf("PLL En        : %s\n", OnOff[pdev->CONFIG24._bitval.pll_ena]);
//	printf("PLL Prescale  : %d\n", pdev->CONFIG24._bitval.pll_p);
//	printf("PLL N         : %d\n", pdev->CONFIG25._bitval.pll_n);
//	printf("PLL M         : %d\n", pdev->CONFIG25._bitval.pll_m);
//	printf("\n");
//}
//
//void init_dac(int verbose)
//{
//	struct dac3484_struct 	*pdev;
//	struct dac3484_config 	*pcfg;
//	int 					i;
//
//    XSpiPs_Config 			*Config;
//    int 					Status;
//	static int 				SpiInitialized = 0;
//
//	/* GPIO Initialization to release DAC_RESET to high. */
//	DAC_RESET(1);
//
//	/* SPI1 Initialization */
//    if (!SpiInitialized) {
//    	Config = XSpiPs_LookupConfig(XPAR_XSPIPS_1_DEVICE_ID);
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
//	pdev = (struct dac3484_struct *)&dac3484;
//	/* Init Sequence
// 	 * 1. Set TXENABLE Low
// 	 * 2. Power-up the device
// 	 * 3. Apply LVPECL DACCLKP/N for PLL reference clock
// 	 * 4. Toggle RESETB pin
// 	 * 5. QMC offset and correction enabled, 2x int, FIFO enabled, Alarm enabled,clock divider sync enabled,
// 	 *    inverse sinc filter enabled.
// 	 * 6. Single parity enabled, FIFO alarms enabled (2 away, 1 away, and collision).Note: bit8 = ‘0’
// 	 * 7. Output shut-off when DACCLK gone, DATACLK gone, and FIFO collision.
// 	 *    Mixer block with NCO enabled, twos complement. Word wide interface.
// 	 * 8. Output current set to 20mAFS with internal reference and 1.28kohm RBIAS resistor.
// 	 * 9. Un-mask FIFO collision, DACCLK-gone, and DATACLK-gone alarms to the Alarm output.
// 	 *10. Program the desired channel I QMC offset value. (Causes Auto-Sync for QMC Offset Block)
// 	 *11. Program the desired FIFO offset value and channel Q QMC offset value.
// 	 *12. Program the desired channel I QMC gain value.
// 	 *13. Coarse mixer mode not used. Program the desired channel Q QMC gain value.
// 	 *14. Program the desired channel IQ QMC phase value. (Causes Auto-Sync QMC Correction Block) Note : bit13 and bit12 = ‘1’
// 	 *15. Program the desired channel IQ NCO phase offset value. (Causes Auto-Sync for Channel IQ NCO Mixer)
// 	 *16. Program the desired channel IQ NCO frequency value
// 	 *17. Program the desired channel IQ NCO frequency value
// 	 *18. PLL enabled, PLL N-dividers sync enabled, single charge pump,prescaler=4
// 	 *19. M = 32, N = 16, PLL VCO bias tune = “01”
// 	 *20. PLL VCO coarse tune = 59
// 	 *21. Internal reference
// 	 *22. QMC offset IQ and QMC correction IQ can be synced by sif_sync or auto-sync from register write
// 	 *23. Mixer IQ values synced by SYNCP/N. NCO accumulator synced by SYNCP/N. FIFO data formatter synced by FRAMEP/N.
// 	 *24. FIFO Input Pointer Sync Source = FRAME
// 	 *    FIFO Output Pointer Sync Source = OSTR (from PLL N-divider output)
// 	 *    Clock Divider Sync Source = OSTR
// 	 */
//    //if (strcmp(band, "RF_DUO") == 0) {
//		pcfg = &dac_default;
//    //}
//
//	pdev->CONFIG00._bitval.invsincCD_ena			= 1;		/* */
//	pdev->CONFIG00._bitval.invsincAB_ena			= 1;		/* */
//	pdev->CONFIG00._bitval.clkdiv_sync_ena			= 1;		/* */
//	pdev->CONFIG00._bitval.alarm_out_pol			= 1;		/* '0'->Neg, '1'->Pos, This bit changes the polarity of the ALARM signal.*/
//	pdev->CONFIG00._bitval.alarm_out_ena			= 1;		/* '0'->3-State, '1'->Out, When set, the ALARM pin becomes an output. When cleared, the ALARM pin is 3-stated */
//	pdev->CONFIG00._bitval.reserved_2				= 0;
//	pdev->CONFIG00._bitval.fifo_ena					= pcfg->fifoen;	/* */
//	pdev->CONFIG00._bitval.interp					= pcfg->interp;	/* "0000"->1x,"0001"->2x,"0010"->4x,"0100"->8x,"1000"->16x*/
//	pdev->CONFIG00._bitval.qmc_corrCD_ena			= 0;		/* When set, the QMC phase and gain correction circuitry is enabled.*/
//	pdev->CONFIG00._bitval.qmc_corrAB_ena			= 0;		/* When set, the QMC phase and gain correction circuitry is enabled.*/
//	pdev->CONFIG00._bitval.qmc_offsetCD_ena			= 0;		/*  */
//	pdev->CONFIG00._bitval.qmc_offsetAB_ena			= 0;		/* When set, the digital Quadrature Modulator Correction (QMC) offset correction is enabled */
//	spiWriteDacDev(0,0x00,pdev->CONFIG00._intval,verbose);
//
//	pdev->CONFIG01._bitval.reserved_1				= 0;
//	pdev->CONFIG01._bitval.alarm_collision_ena		= 1;		/* When set, the alarm from the FIFO indicating a collision between the write and read pointers is enabled*/
//	pdev->CONFIG01._bitval.alarm_1away_ena			= 1;		/* When set, the alarm from the FIFO indicating the write and read pointers being 1 away is enabled*/
//	pdev->CONFIG01._bitval.alarm_2away_ena			= 1;		/* When set, the alarm from the FIFO indicating the write and read pointers being 2 away is enabled*/
//	pdev->CONFIG01._bitval.dacD_complement			= 0;		/* When set, the DACQ output is complemented. This allows to effectively change the + and ? designations of the LVDS data lines*/
//	pdev->CONFIG01._bitval.dacC_complement			= 0;		/* When set, the DACQ output is complemented. This allows to effectively change the + and ? designations of the LVDS data lines*/
//	pdev->CONFIG01._bitval.dacB_complement			= 0;		/* When set, the DACQ output is complemented. This allows to effectively change the + and ? designations of the LVDS data lines*/
//	pdev->CONFIG01._bitval.dacA_complement			= 0;		/* */
//	pdev->CONFIG01._bitval.quad_ena					= 1;		/* */
//	pdev->CONFIG01._bitval.frame_parity_ena			= 0;		/* When set, enables parity checking using the FRAME signal to source the parity bit*/
//	pdev->CONFIG01._bitval.word_parity_ena			= 0;		/* When set, enables parity checking of each input word using the PARITYP/N parity input. It should match the oddeven_parity register setting*/
//	pdev->CONFIG01._bitval.oddeven_parity			= 0;		/* 0: Even parity,1: Odd parity Selects between odd and even parity check*/
//	pdev->CONFIG01._bitval.cnt64_ena				= 0;		/* */
//	pdev->CONFIG01._bitval.reserved_2				= 0;
//	pdev->CONFIG01._bitval.iotest_ena				= 0;		/* When set, enables the data pattern checker test. The outputs are deactivated regardless of the state of TXENABLE and sif_txenable*/
//	spiWriteDacDev(0,0x01,pdev->CONFIG01._intval,verbose);
//
//	pdev->CONFIG02._bitval.reserved_1				= 0;
//	pdev->CONFIG02._bitval.twos						= 1;		/* When set, the input data format is expected to be 2’s complement,When cleared, the input is expected to be offset-binary.*/
//	pdev->CONFIG02._bitval.reserved_2				= 0;
//	pdev->CONFIG02._bitval.revbus					= 0;		/* When set, the input bits for the data bus are reversed. MSB becomes LSB.*/
//	pdev->CONFIG02._bitval.nco_ena					= pcfg->ncoen;	/* When set, the NCO is enabled. This is not required for coarse mixing*/
//	pdev->CONFIG02._bitval.mixer_gain				= 0;		/* When set, a 6dB gain is added to the mixer output*/
//	pdev->CONFIG02._bitval.mixer_ena				= pcfg->mixen;	/* When set, the mixer block is enabled*/
//	pdev->CONFIG02._bitval.sif4_ena					= 1;		/* When set, the serial interface (SIF) is a 4 bit interface, otherwise it is a 3 bit interface*/
//	pdev->CONFIG02._bitval.reserved_3				= 0;
//	pdev->CONFIG02._bitval.collision_gone_ena		= 0;		/* When set, the FIFO collision alarms can be used to shut off the DAC outputs. The corresponding alarms, alarm_fifo_collision and alarm_output_gone, must not be masked (i.e.Config7, bit <13> and bit <8> must set to "0").*/
//	pdev->CONFIG02._bitval.dataclk_gone_ena			= 0;		/* When set, the DATACLK-gone signal from the clock monitor circuit can be used to shut off the DAC outputs. The corresponding alarms,alarm_dataclk_gone and alarm_output_gone, must not be masked (i.e.Config7, bit <9> and bit <8> must set to "0").*/
//	pdev->CONFIG02._bitval.dacclk_gone_ena			= 0;		/* When set, the DACCLK-gone signal from the clock monitor circuit can be used to shut off the DAC outputs. The corresponding alarms,alarm_dacclk_gone and alarm_output_gone, must not be masked (i.e.Config7, bit <10> and bit <8> must set to "0").*/
//	pdev->CONFIG02._bitval.bit16_in					= 1;		/* When set, the input interface is set to word-wide mode. When cleared, the input interface is set to byte-wide mode.*/
//	spiWriteDacDev(0,0x02,pdev->CONFIG02._intval,verbose);
//
//	pdev->CONFIG03._bitval.sif_txenable				= 0x0;		/* "00"->16bit, "01"->8bit, "10"->4bit */
//	pdev->CONFIG03._bitval.reserved					= 0x0;
//	pdev->CONFIG03._bitval.coarse_dac				= 0xF;
//	spiWriteDacDev(0,0x03,pdev->CONFIG03._intval,verbose);
//
//	pdev->CONFIG04._bitval.iotest_results			= 0x0;
//	spiWriteDacDev(0,0x04,pdev->CONFIG04._intval,verbose);
//
//	pdev->CONFIG05._bitval.alarm_frame_parity		= 0x0;		/* Alarm indicating a parity error when using the FRAME as parity bit. */
//	pdev->CONFIG05._bitval.alarm_fparity			= 0x0;		/* Alarm indicating a parity error on data captured on the falling edge of DATACLKP/N.*/
//	pdev->CONFIG05._bitval.alarm_rparity			= 0x0;		/* Alarm indicating a parity error on data captured on the rising edge of DATACLKP/N.*/
//	pdev->CONFIG05._bitval.alarm_from_pll			= 0x0;      /* Alarm indicating the PLL has lost lock. For version ID "100" or */
//	pdev->CONFIG05._bitval.alarm_from_iotest		= 0x0;		/* Alarm indicating the input data pattern does not match the pattern in*/
//	pdev->CONFIG05._bitval.alarm_output_gone		= 0x0;		/* Alarm indicating either alarm_dacclk_gone, alarm_dataclk_gone, or*/
//	pdev->CONFIG05._bitval.alarm_dataclk_gone		= 0x0;		/* Alarm indicating the DATACLK has been stopped*/
//	pdev->CONFIG05._bitval.alarm_dacclk_gone		= 0x0;		/* Alarm indicating the DACCLK has been stopped. If the bit is set*/
//	pdev->CONFIG05._bitval.alarms_from_fifo			= 0x0;		/* Alarm indicating FIFO pointer collisions and nearness:*/
//	pdev->CONFIG05._bitval.alarms_from_zerochk		= 0x0;		/* This alarm indicates the 8-bit FIFO write pointer address has an all*/
//	spiWriteDacDev(0,0x05,pdev->CONFIG05._intval,verbose);
//
//	pdev->CONFIG06._bitval.reserved					= 0x0;
//	pdev->CONFIG06._bitval.tempdata					= 0x0;		/* This is the output from the chip temperature sensor. The value of this register in two’s complement format represents the temperature in degrees Celsius. This register must be read with a minimum SCLK period of 1μs.*/
//	spiWriteDacDev(0,0x06,pdev->CONFIG06._intval,verbose);
//
//	/* These bits control the masking of the alarms. (0=not masked, 1= masked)
//	 * alarm_mask       Alarm that is Masked
//	 *         15		alarm_from_zerochk
//	 *         14		not used
//	 *         13		alarm_fifo_collision
//	 *         12		alarm_fifo_1away
//	 *         11		alarm_fifo_2away
//	 *         10		alarm_dacclk_gone
//	 *          9		alarm_dataclk_gone
//	 *          8		alarm_output_gone
//	 *          7		alarm_from_iotest
//	 *          6		not used
//	 *          5		alarm_from_pll
//	 *          4		alarm_rparity
//	 *          3		alarm_lparity
//	 *          2		alarm_frame_parity
//	 *          1 		not used
//	 *          0 		not used
//	 */
//	pdev->CONFIG07._bitval.alarms_mask				= 0xFFFF;
//	pdev->CONFIG07._bitval.alarms_mask				&= ~ALARM_MASK_DACCLK_GONE;
//	pdev->CONFIG07._bitval.alarms_mask				&= ~ALARM_MASK_DATACLK_GONE;
//	pdev->CONFIG07._bitval.alarms_mask				&= ~ALARM_MASK_COLLISION;
//	pdev->CONFIG07._bitval.alarms_mask				&= ~ALARM_MASK_FIFO_1WAY;
//	pdev->CONFIG07._bitval.alarms_mask				&= ~ALARM_MASK_FIFO_2WAY;
//	pdev->CONFIG07._bitval.alarms_mask				&= ~ALARM_MASK_OUTPUT_GONE;
//	pdev->CONFIG07._bitval.alarms_mask				&= ~ALARM_MASK_PLL;
//	spiWriteDacDev(0,0x07,pdev->CONFIG07._intval,verbose);
//
//	pdev->CONFIG08._bitval.qmc_offsetA				= 0xFFF;		/* */
//	pdev->CONFIG08._bitval.reserved					= 0x0;
//	spiWriteDacDev(0,0x08,pdev->CONFIG08._intval,verbose);
//
//	pdev->CONFIG09._bitval.qmc_offsetB				= 0xFFF;		/* */
//	pdev->CONFIG09._bitval.fifo_offset				= pcfg->fifo_offset;		/* */
//	spiWriteDacDev(0,0x09,pdev->CONFIG09._intval,verbose);
//
//	pdev->CONFIG10._bitval.qmc_offsetC				= 0xFFF;		/* */
//	pdev->CONFIG10._bitval.reserved					= 0x0;
//	spiWriteDacDev(0,0x0A,pdev->CONFIG10._intval,verbose);
//
//	pdev->CONFIG11._bitval.qmc_offsetD				= 0xFFF;		/* */
//	pdev->CONFIG11._bitval.reserved					= 0x0;
//	spiWriteDacDev(0,0x0B,pdev->CONFIG11._intval,verbose);
//
//	pdev->CONFIG12._bitval.qmc_gainA				= 0x400;		/* */
//	pdev->CONFIG12._bitval.reserved					= 0x0;
//	spiWriteDacDev(0,0x0C,pdev->CONFIG12._intval,verbose);
//
//	pdev->CONFIG13._bitval.qmc_gainB				= 0x400;		/* */
//	pdev->CONFIG13._bitval.reserved					= 0x0;
//	pdev->CONFIG13._bitval.cmix_mode				= pcfg->cmix;		/* */
//	spiWriteDacDev(0,0x0D,pdev->CONFIG13._intval,verbose);
//
//	pdev->CONFIG14._bitval.qmc_gainC				= 0x400;		/* */
//	pdev->CONFIG14._bitval.reserved					= 0x0;
//	spiWriteDacDev(0,0x0E,pdev->CONFIG14._intval,verbose);
//
//	pdev->CONFIG15._bitval.qmc_gainD				= 0x400;		/* */
//	pdev->CONFIG15._bitval.reserved					= 0x0;
//	pdev->CONFIG15._bitval.output_delayCD			= 0x0;		/* */
//	pdev->CONFIG15._bitval.output_delayAB			= 0x0;		/* */
//	spiWriteDacDev(0,0x0F,pdev->CONFIG15._intval,verbose);
//
//	pdev->CONFIG16._bitval.qmc_phaseAB				= 0x0;
//	pdev->CONFIG16._bitval.dual_ena					= 0x0;
//	pdev->CONFIG16._bitval.reserved					= 0x0;
//	spiWriteDacDev(0,0x10,pdev->CONFIG16._intval,verbose);
//
//	pdev->CONFIG17._bitval.qmc_phaseCD				= 0x0;
//	pdev->CONFIG17._bitval.reserved					= 0x3;
//	spiWriteDacDev(0,0x11,pdev->CONFIG17._intval,verbose);
//
//	pdev->CONFIG18._bitval.phase_offsetAB			= 0x0;
//	spiWriteDacDev(0,0x12,pdev->CONFIG18._intval,verbose);
//
//	pdev->CONFIG19._bitval.phase_offsetCD			= 0x0;
//	spiWriteDacDev(0,0x13,pdev->CONFIG19._intval,verbose);
//
//	pdev->CONFIG20._bitval.phase_addAB				= (pcfg->nco_A>> 0) & 0xffff;
//	spiWriteDacDev(0,0x14,pdev->CONFIG20._intval,verbose);
//
//	pdev->CONFIG21._bitval.phase_addAB				= (pcfg->nco_A>>16) & 0xffff;
//	spiWriteDacDev(0,0x15,pdev->CONFIG21._intval,verbose);
//
//	pdev->CONFIG22._bitval.phase_addCD				= (pcfg->nco_B>> 0) & 0xffff;
//	spiWriteDacDev(0,0x16,pdev->CONFIG22._intval,verbose);
//
//	pdev->CONFIG23._bitval.phase_addCD				= (pcfg->nco_B>>16) & 0xffff;
//	spiWriteDacDev(0,0x17,pdev->CONFIG23._intval,verbose);
//
//	pdev->CONFIG24._bitval.pll_lfvolt				= 0x0;
//	pdev->CONFIG24._bitval.pll_p					= 3;
//	pdev->CONFIG24._bitval.pll_cp					= 1;			/* 0:No charge pump, 1: Single pump charge, 2: Not used, 11: Dual pump charge*/
//	pdev->CONFIG24._bitval.reserved_1				= 0x0;
//	pdev->CONFIG24._bitval.pll_ena				    = 0;
//	pdev->CONFIG24._bitval.pll_ndivsync_ena		    = 1;
//	pdev->CONFIG24._bitval.pll_reset				= 0;
//	pdev->CONFIG24._bitval.reserved_2				= 0x1;
//	spiWriteDacDev(0,0x18,pdev->CONFIG24._intval,verbose);
//
//	pdev->CONFIG25._bitval.reserved					= 0x0;
//	pdev->CONFIG25._bitval.pll_vcoitune				= 0x1;
//	pdev->CONFIG25._bitval.pll_n					= 0;
//	pdev->CONFIG25._bitval.pll_m					= 32;
//	spiWriteDacDev(0,0x19,pdev->CONFIG25._intval,verbose);
//
//	pdev->CONFIG26._bitval.sleepD					= 0x1;  ////0
//	pdev->CONFIG26._bitval.sleepC					= 0x0;
//	pdev->CONFIG26._bitval.sleepB					= 0x0;
//	pdev->CONFIG26._bitval.sleepA					= 0x1;  ///0
//	pdev->CONFIG26._bitval.clkrecv_sleep			= 0x0;
//	pdev->CONFIG26._bitval.pll_sleep				= 0x1;
//	pdev->CONFIG26._bitval.tsense_sleep				= 0x0;
//	pdev->CONFIG26._bitval.bias_sleep				= 0x0;
//	pdev->CONFIG26._bitval.reserved_2				= 0x0;
//	pdev->CONFIG26._bitval.pll_vco					= 37;
//	spiWriteDacDev(0,0x1A,pdev->CONFIG26._intval,verbose);
//
//	pdev->CONFIG27._bitval.reserved_1				= 0x0;
//	pdev->CONFIG27._bitval.fuse_sleep				= 0x1;
//	pdev->CONFIG27._bitval.reserved_2				= 0x0;
//	pdev->CONFIG27._bitval.extref_ena				= 0x0;
//	spiWriteDacDev(0,0x1B,pdev->CONFIG27._intval,verbose);
//
//	pdev->CONFIG30._bitval.syncsel_qmcorrCD			= 0x1;		/* */
//	pdev->CONFIG30._bitval.syncsel_qmcorrAB			= 0x1;		/* */
//	pdev->CONFIG30._bitval.syncsel_qmoffsetCD		= 0x1;		/* */
//	pdev->CONFIG30._bitval.syncsel_qmoffsetAB		= 0x1;		/* */
//	spiWriteDacDev(0,0x1E,pdev->CONFIG30._intval,verbose);
//
//	pdev->CONFIG31._bitval.reserved_1				= 0x0;
//	pdev->CONFIG31._bitval.sif_sync					= 0x0;
//	pdev->CONFIG31._bitval.syncsel_dataformatter	= 0x0;		/* FRAME */
//	pdev->CONFIG31._bitval.syncsel_nco				= 0x4;		/* SYNC */
//	pdev->CONFIG31._bitval.syncsel_mixerCD			= 0x4;		/* SYNC */
//	pdev->CONFIG31._bitval.syncsel_mixerAB			= 0x4;		/* SYNC */
//	spiWriteDacDev(0,0x1F,pdev->CONFIG31._intval,verbose);
//
//	pdev->CONFIG32._bitval.clkdiv_sync_sel			= 0x0;		/* 0->OSTR,1->FRAME,SYNC,SIF SYNC */
//	pdev->CONFIG32._bitval.reserved					= 0x0;
//	/*  Selects the syncing source(s) of the FIFO output side. A ‘1’ in the bit enables the
//	 *  signal as a sync source. More than one sync source is permitted.
//	 *  "1000" -> sif_sync
//	 * 	"0100" -> OSTR (Dual Sync Source Mode)
//	 * 	"0010" -> FRAME
//	 *  "0001" -> SYNC */
//	pdev->CONFIG32._bitval.syncsel_fifoout			= 0x4;		/* OSTR */
//	/*  Selects the syncing source(s) of the FIFO input side. A ‘1’ in the bit enables the
//	 *  signal as a sync source. More than one sync source is permitted.
//	 *  "1000" -> sif_sync
//	 * 	"0100" -> Always zero
//	 * 	"0010" -> FRAME
//	 *  "0001" -> SYNC */
//	pdev->CONFIG32._bitval.syncsel_fifoin			= 0x2;		/* FRAME */
//	spiWriteDacDev(0,0x20,pdev->CONFIG32._intval,verbose);
//
//	pdev->CONFIG35._bitval.sleep_cntl				= 0xFFFF; //DACB_SLEEP|DACD_SLEEP|PLL_SLEEP;
//	spiWriteDacDev(0,0x23,pdev->CONFIG35._intval,verbose);
//
//	pdev->CONFIG36._bitval.reserved					= 0;
//	pdev->CONFIG36._bitval.clkdly					= pcfg->clkdly;
//	pdev->CONFIG36._bitval.datadly					= pcfg->datadly;
//	spiWriteDacDev(0,0x24,pdev->CONFIG36._intval,verbose);
//
//	pdev->CONFIG37._bitval.iotest_pattern0			= 0x7A7A;
//	spiWriteDacDev(0,0x25,pdev->CONFIG37._intval,verbose);
//
//	pdev->CONFIG38._bitval.iotest_pattern1			= 0xB6B6;
//	spiWriteDacDev(0,0x26,pdev->CONFIG38._intval,verbose);
//
//	pdev->CONFIG39._bitval.iotest_pattern2			= 0xEAEA;
//	spiWriteDacDev(0,0x27,pdev->CONFIG39._intval,verbose);
//
//	pdev->CONFIG40._bitval.iotest_pattern3			= 0x4545;
//	spiWriteDacDev(0,0x28,pdev->CONFIG40._intval,verbose);
//
//	pdev->CONFIG41._bitval.iotest_pattern4			= 0x1A1A;
//	spiWriteDacDev(0,0x29,pdev->CONFIG41._intval,verbose);
//
//	pdev->CONFIG42._bitval.iotest_pattern5			= 0x1616;
//	spiWriteDacDev(0,0x2A,pdev->CONFIG42._intval,verbose);
//
//	pdev->CONFIG43._bitval.iotest_pattern6			= 0xAAAA;
//	spiWriteDacDev(0,0x2B,pdev->CONFIG43._intval,verbose);
//
//	pdev->CONFIG44._bitval.iotest_pattern7			= 0xC6C6;
//	spiWriteDacDev(0,0x2C,pdev->CONFIG44._intval,verbose);
//
//	pdev->CONFIG45._bitval.sifdac_ena				= 0x0;
//	pdev->CONFIG45._bitval.ramp_ena					= 0x0;
//	pdev->CONFIG45._bitval.ostrtodig_sel			= 0x1;
//	spiWriteDacDev(0,0x2D,pdev->CONFIG45._intval,verbose);
//
//	pdev->CONFIG46._bitval.grp_delayB				= 0x0;
//	pdev->CONFIG46._bitval.grp_delayA				= 0x0;
//	spiWriteDacDev(0,0x2E,pdev->CONFIG46._intval,verbose);
//
//	pdev->CONFIG47._bitval.grp_delayD				= 0x0;
//	pdev->CONFIG47._bitval.grp_delayC				= 0x0;
//	spiWriteDacDev(0,0x2F,pdev->CONFIG47._intval,verbose);
//
//	pdev->CONFIG48._bitval.sifdac					= 0x0;
//	spiWriteDacDev(0,0x30,pdev->CONFIG48._intval,verbose);
//
//	showRegInfo();
//	showDacInfo();
//	return;
//}
//
//#define	PS_INIT		0x0001
//#define	PS_INC		0x0002
//#define	PS_DONE		0x0004
//
//static int setClkPsc(struct dac_struct *pdac, uchar inc, ushort *data)
//{
//	struct psa_struct 	*ppsa;
//    unsigned int 		ioaddr;
//    unsigned int 		i,iodata;
//	static int			psc = 0;
//
//	ppsa = getPsaDev();
//	ioaddr = ppsa->psa_reg_base + 0x8;
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
//    	psc -= 1;
//    }
//	if (data) {
//		*data = (ushort)spiReadDacDev(0,0x04);
//	}
//	spiWriteDacDev(0,0x04,0,0);		//	clear
//	spiWriteDacDev(0,0x05,0,0);		//	clear
//    return psc;
//}
//
//int txDac(uchar onoff)
//{
//	ushort 					ctrl;
//	int						i,j,timeout;
//	struct dac3484_struct	*pdev;
//
//	pdev = (struct dac3484_struct *)&dac3484;
//
//	if (onoff) {
//		/* Sync Confirm Sequences after FPGA Config
// 		 *25. Provide all the LVDS DATA and DATACLK
// 		 *    Provide rising edge FRAMEP/N and rising edge SYNCP/N to sync the FIFO input pointer and PLL N-dividers.
// 		 *26. Read back pll_lfvolt(2:0). If the value is not optimal, adjust pll_vco(5:0) in 0x1A.
// 		 *27. Clear all alarms in 0x05
// 		 *28. Read back all alarms in 0x05. Check for PLL lock, FIFO collision, DACCLK-gone, DATACLK-gone, etc.
// 		 *    Fix the error appropriately. Repeat step 26 and 27 as necessary.
// 		 */
//		for (i=0;i<16;i++) {
//			ctrl = spiReadDacDev(0,0x05);
//			if ((ctrl & ALARM_MASK_COLLISION) || (ctrl & ALARM_MASK_FIFO_1WAY) || (ctrl & ALARM_MASK_FIFO_2WAY) ||
//				(ctrl & ALARM_MASK_DACCLK_GONE) || (ctrl & ALARM_MASK_DATACLK_GONE) || (ctrl & ALARM_MASK_OUTPUT_GONE)) {
//				printf(".");
//				DAC_SYNC(0);
//				for(timeout=100;timeout>0;timeout--);
//				DAC_SYNC(1);
//				for(timeout=100;timeout>0;timeout--);
//				spiWriteDacDev(0,0x05,0x0,0);
//				for(timeout=100;timeout>0;timeout--);
//				continue;
//			}
//			else {
//				/* Sync Disable
// 				 *29. Sync all the QMC blocks using sif_sync.
// 				 *    These blocks can also be synced via auto-sync through appropriate register writes.
// 				 *30. Disable clock divider sync.
// 				 *31. Disable FIFO data formatter sync. Set sif_sync to “0” for the next sif_sync event.
// 				 *32. Disable FIFO input and output pointer sync.
// 				 *33. Disable PLL N-dividers sync.
// 				 */
//
//				/* Fire DAC_TX
// 				 *34. Set TXENABLE high. Enable data transmission.
// 				 */
//				printf("DAC Sync Success!!\n");
//				DAC_TXEN(1);
// 				return (1);
//			}
//		}
//		if (i==16) {
// 			printf("DAC Sync Failed!!\n");
// 			return (-1);
// 		}
//	}
// 	else {
//		DAC_TXEN(0);
//		DAC_SYNC(0);
// 		return (1);
// 	}
//}
//
////void init_dac3484(struct dac_struct *pdac, int band)
////{
////    pdac->showRegInfo = (void(*)())showRegInfo;//pdac->reset_adc = (int(*)())reset_adc;
////    pdac->showDacInfo = (void(*)())showDacInfo;
////    pdac->setClkPsc = (int(*)())setClkPsc;
////    pdac->txDac = (int(*)())txDac;
////	pdac->dac_type = (void *)&dac3484;
////	pdac->init_dac = (void(*)())init_dac;
////
////	init_dac(pdac,0);
////	showRegInfo(pdac); //showDacInfo(pdac);
////};
//
//#if 1
//char *DacHelp[] = {
//	"DAC3484 ",
//	"[application_info]",
//	0,
//};
//
//int
//DacCmd(int argc, char *argv[])
//{
//    char 			*arg1,*arg2;
//    int 			opt;
//	int 			val=0;
//	extern int 		optind;
//	extern char 	*optarg;
//
//    while((opt=getopt(argc,argv,"v:")) != -1) {
//        switch (opt) {
//            case 'v':
//            	val = (int)strtoul(optarg,0,0);
//                break;
//            default:
//                return(0);
//        }
//    }
//
//    arg1 = argv[optind];
//    arg2 = argv[optind+1];
//	if (strcmp(arg1, "reg") == 0) {
//		showRegInfo();
//	}
//	else if (strcmp(arg1, "init") == 0) {
//		init_dac(1);
//	}
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
//		spiWriteDacDev(0,0x05,0x0,0);
//	}
//	else if (strcmp(arg1, "reset") == 0) {
//		if (strcmp(arg2, "on") == 0) {
//			printf("DAC RESET On...\n");
//			DAC_RESET(0);
//		}
//		else if (strcmp(arg2, "off") == 0) {
//			printf("DAC RESET Off..\n");
//			DAC_RESET(1);
//		}
//	}
//	//else if (strcmp(arg1, "tx") == 0) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		DAC_TXEN(1);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		DAC_TXEN(0);
//	//	}
//	//}
//	//else if (strcmp(arg1, "sync") == 0) {
//	//	if (strcmp(arg2, "on") == 0) {
//	//		DAC_SYNC(1);
//	//	}
//	//	else if (strcmp(arg2, "off") == 0) {
//	//		DAC_SYNC(0);
//	//	}
//	//}
//	//else {
//	//	showDacDesc();
//	//}
//
//    return(CMD_SUCCESS);
//}
//#endif
//
//
////void setDatadelay(int dir)
////{
////	struct dac_struct *pdac;
////	struct dac3484_struct *pdev;
////	struct dac3484_config *pcfg;
////
////	pdac = getDacDev(0);
////	pdev = (struct dac3484_struct *)pdac->dac_type;
////	pcfg = &dac_default;
////
////	if(dir == 1){
////		if(pcfg->datadly == 255);
////		else pcfg->datadly++;
////	}
////	else if(dir == 0){
////		if(pcfg->datadly == 0);
////		else pcfg->datadly--;
////	}
////	pdev->CONFIG36._bitval.datadly = pcfg->datadly;
////	spiWriteDacDev(0, 0x24, pdev->CONFIG36._intval, 0);
////	printf("Dac datadly : %d\n", pcfg->datadly);
////}
////
////void setClockdelay(int dir){
////	struct dac_struct *pdac;
////	struct dac3484_struct *pdev;
////	struct dac3484_config *pcfg;
////
////	pdac = getDacDev(0);
////	pdev = (struct dac3484_struct *)pdac->dac_type;
////	pcfg = &dac_default;
////
////	if(dir == 1){
////		if(pcfg->clkdly == 255);
////		else pcfg->clkdly++;
////	}
////	else if(dir == 0){
////		if(pcfg->clkdly == 0);
////		else pcfg->clkdly--;
////	}
////	pdev->CONFIG36._bitval.clkdly = pcfg->clkdly;
////	spiWriteDacDev(0, 0x24, pdev->CONFIG36._intval, 0);
////	printf("Dac clkdly : %d\n", pcfg->clkdly);
////}
