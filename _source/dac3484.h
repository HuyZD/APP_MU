#define	FIR_X1		0x0
#define	FIR_X2		0x1
#define	FIR_X4		0x2
#define	FIR_X8		0x3

#define	MOD_X1		0x0
#define	MOD_X2		0x1
#define	MOD_X4		0x2
#define	MOD_X8		0x3

#define ALARM_MASK_FRAME_PARITY		0x0004
#define ALARM_MASK_FPARITY			0x0008
#define ALARM_MASK_RPARITY			0x0010
#define ALARM_MASK_PLL				0x0020
#define ALARM_MASK_OUTPUT_GONE		0x0100
#define ALARM_MASK_DATACLK_GONE		0x0200
#define ALARM_MASK_DACCLK_GONE		0x0400
#define ALARM_MASK_FIFO_2WAY		0x0800
#define ALARM_MASK_FIFO_1WAY		0x1000
#define ALARM_MASK_COLLISION		0x2000

#define DACA_SLEEP					0x8000
#define DACB_SLEEP					0x4000
#define DACC_SLEEP					0x2000
#define DACD_SLEEP					0x1000
#define CLKRECV_SLEEP				0x0800
#define LVDS_DATA_SLEEP				0x0400
#define LVDS_CTRL_SLEEP				0x0200
#define PLL_SLEEP					0x0100
#define TSENSE_SLEEP				0x0080
#define BIAS_SLEEP					0x0020

struct dac3484_config {
	uchar	interp;
	uchar	fifoen;
	uchar	mixen;
	uchar	ncoen;
	int		nco_A;
	int		nco_B;
	uchar	cmix;
	uchar	fifo_offset;
	uchar	clkdly;
	uchar	datadly;
};
struct dac3484_struct {
	union {													/* 0x00 : Comm */
		unsigned int		_intval;						/*  */
		struct {											/*  */
			unsigned int 	invsincCD_ena			: 1;	/*  */
			unsigned int 	invsincAB_ena			: 1;	/*  */
			unsigned int 	clkdiv_sync_ena			: 1;	/*  */
			unsigned int 	alarm_out_pol			: 1;	/*  */
			unsigned int 	alarm_out_ena			: 1;	/*  */
			unsigned int 	reserved_2				: 2;	/*  */
			unsigned int 	fifo_ena				: 1;	/*  */
			unsigned int 	interp					: 4;	/*  */
			unsigned int 	qmc_corrCD_ena			: 1;	/*  */
			unsigned int 	qmc_corrAB_ena			: 1;	/*  */
			unsigned int 	qmc_offsetCD_ena		: 1;	/*  */
			unsigned int 	qmc_offsetAB_ena		: 1;	/*  */
		} _bitval;											/*  */
	} CONFIG00;												
	union {													/* 0x01 : Power Control */
		unsigned int		_intval;						/*  */
		struct {											/*  */
			unsigned int 	reserved_1				: 1;	/*  */
			unsigned int 	alarm_collision_ena		: 1;	/*  */
			unsigned int 	alarm_1away_ena			: 1;	/*  */
			unsigned int 	alarm_2away_ena			: 1;	/*  */
			unsigned int 	dacD_complement			: 1;	/*  */
			unsigned int 	dacC_complement			: 1;	/*  */
			unsigned int 	dacB_complement			: 1;	/*  */
			unsigned int 	dacA_complement			: 1;	/*  */
			unsigned int 	quad_ena				: 1;	/*  */
			unsigned int 	frame_parity_ena		: 1;	/*  */
			unsigned int 	word_parity_ena			: 1;	/*  */
			unsigned int 	oddeven_parity			: 1;	/*  */
			unsigned int 	cnt64_ena				: 1;	/*  */
			unsigned int 	reserved_2				: 2;	/*  */
			unsigned int 	iotest_ena				: 1;	/*  */
		} _bitval;											/*  */
	} CONFIG01;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved_1				: 1;		/*  */
			unsigned int 	twos					: 1;		/*  */
			unsigned int 	reserved_2				: 1;		/*  */
			unsigned int 	revbus					: 1;		/*  */
			unsigned int 	nco_ena					: 1;		/*  */
			unsigned int 	mixer_gain				: 1;		/*  */
			unsigned int 	mixer_ena				: 1;		/*  */
			unsigned int 	sif4_ena				: 1;		/*  */
			unsigned int 	reserved_3				: 4;		/*  */
			unsigned int 	collision_gone_ena		: 1;		/*  */
			unsigned int 	dataclk_gone_ena		: 1;		/*  */
			unsigned int 	dacclk_gone_ena			: 1;		/*  */
			unsigned int 	bit16_in				: 1;		/*  */
		} _bitval;											/*  */
	} CONFIG02;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	sif_txenable			: 1;		/*  */
			unsigned int 	reserved				:11;		/*  */
			unsigned int 	coarse_dac				: 4;		/*  */
		} _bitval;											/*  */
	} CONFIG03;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_results			: 16;		/*  */
		} _bitval;											/*  */
	} CONFIG04;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved_1				: 2;		/*  */
			unsigned int 	alarm_frame_parity		: 1;		/*  */
			unsigned int 	alarm_fparity			: 1;		/*  */
			unsigned int 	alarm_rparity			: 1;		/*  */
			unsigned int 	alarm_from_pll			: 1;		/*  */
			unsigned int 	reserved_2				: 1;		/*  */
			unsigned int 	alarm_from_iotest		: 1;		/*  */
			unsigned int 	alarm_output_gone		: 1;		/*  */
			unsigned int 	alarm_dataclk_gone		: 1;		/*  */
			unsigned int 	alarm_dacclk_gone		: 1;		/*  */
			unsigned int 	alarms_from_fifo		: 3;		/*  */
			unsigned int 	reserved_3				: 1;		/*  */
			unsigned int 	alarms_from_zerochk		: 1;		/*  */
		} _bitval;											/*  */
	} CONFIG05;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved				: 8;		/*  */
			unsigned int 	tempdata				: 8;		/*  */
		} _bitval;											/*  */
	} CONFIG06;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	alarms_mask				: 16;		/*  */
		} _bitval;											/*  */
	} CONFIG07;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_offsetA				: 13;		/*  */
			unsigned int 	reserved				: 3;		/*  */
		} _bitval;											/*  */
	} CONFIG08;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_offsetB				: 13;		/*  */
			unsigned int 	fifo_offset				: 3;		/*  */
		} _bitval;											/*  */
	} CONFIG09;
	union {														/* 0x02 : CONFIG10 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_offsetC				: 13;		/*  */
			unsigned int 	reserved				:  3;		/*  */
		} _bitval;												/*  */
	} CONFIG10;
	union {														/* 0x02 : CONFIG10 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_offsetD				: 13;		/*  */
			unsigned int 	reserved				:  3;		/*  */
		} _bitval;												/*  */
	} CONFIG11;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_gainA				: 11;		/*  */
			unsigned int 	reserved				:  5;		/*  */
		} _bitval;												/*  */
	} CONFIG12;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_gainB				: 11;		/*  */
			unsigned int 	reserved				:  1;		/*  */
			unsigned int 	cmix_mode				:  4;		/*  */
		} _bitval;												/*  */
	} CONFIG13;
	union {														/* 0x02 : CONFIG10 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_gainC				: 11;		/*  */
			unsigned int 	reserved				:  5;		/*  */
		} _bitval;												/*  */
	} CONFIG14;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_gainD				: 11;		/*  */
			unsigned int 	reserved				:  1;		/*  */
			unsigned int 	output_delayCD			:  2;		/*  */
			unsigned int 	output_delayAB			:  2;		/*  */
		} _bitval;												/*  */
	} CONFIG15;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_phaseAB				: 12;		/*  */
			unsigned int 	dual_ena				:  2;		/*  */
			unsigned int 	reserved				:  2;		/*  */
		} _bitval;												/*  */
	} CONFIG16;
	union {														/* 0x02 : CONFIG10 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	qmc_phaseCD				: 12;		/*  */
			unsigned int 	reserved				:  4;		/*  */
		} _bitval;												/*  */
	} CONFIG17;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	phase_offsetAB			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG18;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	phase_offsetCD			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG19;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	phase_addAB				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG20;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	phase_addAB				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG21;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	phase_addCD				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG22;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	phase_addCD				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG23;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	pll_lfvolt				: 3;		/*  */
			unsigned int 	pll_p					: 3;		/*  */
			unsigned int 	pll_cp					: 2;		/*  */
			unsigned int 	reserved_1				: 2;		/*  */
			unsigned int 	pll_ena					: 1;		/*  */
			unsigned int 	pll_ndivsync_ena		: 1;		/*  */
			unsigned int 	pll_reset				: 1;		/*  */
			unsigned int 	reserved_2				: 3;		/*  */
		} _bitval;												/*  */
	} CONFIG24;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved				: 2;		/*  */
			unsigned int 	pll_vcoitune			: 2;		/*  */
			unsigned int 	pll_n					: 4;		/*  */
			unsigned int 	pll_m					: 8;		/*  */
		} _bitval;												/*  */
	} CONFIG25;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	sleepD					: 1;		/*  */
			unsigned int 	sleepC					: 1;		/*  */
			unsigned int 	sleepB					: 1;		/*  */
			unsigned int 	sleepA					: 1;		/*  */
			unsigned int 	clkrecv_sleep			: 1;		/*  */
			unsigned int 	pll_sleep				: 1;		/*  */
			unsigned int 	tsense_sleep			: 1;		/*  */
			unsigned int 	bias_sleep				: 1;		/*  */
			unsigned int 	reserved_2				: 2;		/*  */
			unsigned int 	pll_vco					: 6;		/*  */
		} _bitval;												/*  */
	} CONFIG26;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved_1				: 11;		/*  */
			unsigned int 	fuse_sleep				: 1;		/*  */
			unsigned int 	reserved_2				: 3;		/*  */
			unsigned int 	extref_ena				: 1;		/*  */
		} _bitval;												/*  */
	} CONFIG27;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG28;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG29;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	syncsel_qmcorrCD		: 4;		/*  */
			unsigned int 	syncsel_qmcorrAB		: 4;		/*  */
			unsigned int 	syncsel_qmoffsetCD		: 4;		/*  */
			unsigned int 	syncsel_qmoffsetAB		: 4;		/*  */
		} _bitval;												/*  */
	} CONFIG30;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved_1				: 1;		/*  */
			unsigned int 	sif_sync				: 1;		/*  */
			unsigned int 	syncsel_dataformatter	: 2;		/*  */
			unsigned int 	syncsel_nco				: 4;		/*  */
			unsigned int 	syncsel_mixerCD			: 4;		/*  */
			unsigned int 	syncsel_mixerAB			: 4;		/*  */
		} _bitval;												/*  */
	} CONFIG31;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	clkdiv_sync_sel			: 1;		/*  */
			unsigned int 	reserved				: 7;		/*  */
			unsigned int 	syncsel_fifoout			: 4;		/*  */
			unsigned int 	syncsel_fifoin			: 4;		/*  */
		} _bitval;												/*  */
	} CONFIG32;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG33;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	DACD_out_sel			:  2;		/*  */
			unsigned int 	DACC_out_sel			:  2;		/*  */
			unsigned int 	DACB_out_sel			:  2;		/*  */
			unsigned int 	DACA_out_sel			:  2;		/*  */
			unsigned int 	pathD_in_sel			:  2;		/*  */
			unsigned int 	pathC_in_sel			:  2;		/*  */
			unsigned int 	pathB_in_sel			:  2;		/*  */
			unsigned int 	pathA_in_sel			:  2;		/*  */
		} _bitval;												/*  */
	} CONFIG34;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	sleep_cntl				: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG35;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	reserved				: 10;		/*  */
			unsigned int 	clkdly					: 3;		/*  */
			unsigned int 	datadly					: 3;		/*  */
		} _bitval;												/*  */
	} CONFIG36;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern0			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG37;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern1			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG38;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern2			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG39;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern3			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG40;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern4			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG41;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern5			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG42;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern6			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG43;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	iotest_pattern7			: 16;		/*  */
		} _bitval;												/*  */
	} CONFIG44;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	sifdac_ena				: 1;		/*  */
			unsigned int 	reserved_1				:12;		/*  */
			unsigned int 	ramp_ena				: 1;		/*  */
			unsigned int 	ostrtodig_sel			: 1;		/*  */
			unsigned int 	reserved_2				: 1;		/*  */
		} _bitval;												/*  */
	} CONFIG45;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	grp_delayB				: 8;		/*  */
			unsigned int 	grp_delayA				: 8;		/*  */
		} _bitval;												/*  */
	} CONFIG46;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	grp_delayD				: 8;		/*  */
			unsigned int 	grp_delayC				: 8;		/*  */
		} _bitval;												/*  */
	} CONFIG47;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	sifdac					:16;		/*  */
		} _bitval;												/*  */
	} CONFIG48;
	union {														/* 0x02 : CONFIG2 */
		unsigned int		_intval;							/*  */
		struct {												/*  */
			unsigned int 	versionid				: 3;		/*  */
			unsigned int 	deviceid				: 2;		/*  */
			unsigned int 	reserved				:11;		/*  */
		} _bitval;												/*  */
	} VERSION;
};                                                  

