#define ADC_GAIN_0_0		0x0
#define ADC_GAIN_0_5		0x1
#define ADC_GAIN_1_0		0x2
#define ADC_GAIN_1_5		0x3
#define ADC_GAIN_2_0		0x4
#define ADC_GAIN_2_5		0x5
#define ADC_GAIN_3_0		0x6
#define ADC_GAIN_3_5		0x7
#define ADC_GAIN_4_0		0x8
#define ADC_GAIN_4_5		0x9
#define ADC_GAIN_5_0		0xA
#define ADC_GAIN_5_5		0xB
#define ADC_GAIN_6_0		0xC

struct ads4246_struct {
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	READOUT						: 1;
			unsigned char 	RESET						: 1;
			unsigned char 	RSVD						: 6;
		} _bitval;
	} REG00;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 2;
			unsigned char 	LVDS_SWING					: 6;
		} _bitval;
	} REG01;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 6;
			unsigned char 	HIGH_PERF_MODE_3			: 1;
			unsigned char 	RSVD2						: 1;
		} _bitval;
	} REG02;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	HIGH_PERF_MODE_1			: 1;
			unsigned char 	HIGH_PERF_MODE_2			: 1;
			unsigned char 	RSVD						: 6;
		} _bitval;
	} REG03;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	CH_A_TEST_PATTERNS			: 3;
			unsigned char 	RSVD						: 1;
			unsigned char 	CH_A_GAIN					: 4;
		} _bitval;
	} REG25;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 3;
			unsigned char 	DATA_FORMAT					: 2;
			unsigned char 	RSVD2						: 3;
		} _bitval;
	} REG29;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	CH_B_TEST_PATTERNS			: 3;
			unsigned char 	RSVD						: 1;
			unsigned char 	CH_B_GAIN					: 4;
		} _bitval;
	} REG2B;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 5;
			unsigned char 	ENABLE_OFFSET_CORR			: 1;
			unsigned char 	RSVD2						: 2;
		} _bitval;
	} REG3D;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	CUSTOM_PATTERN_D 			: 6;
			unsigned char 	RSVD						: 2;
		} _bitval;
	} REG3F;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	CUSTOM_PATTERN_D			: 8;
		} _bitval;
	} REG40;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	DIS_OBUF 					: 2;
			unsigned char 	RSVD						: 2;
			unsigned char 	CMOS_CLKOUT_STRENGTH		: 2;
			unsigned char 	LVDS_CMOS 					: 2;
		} _bitval;
	} REG41;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 3;
			unsigned char 	EN_DIGITAL 					: 1;
			unsigned char 	CLKOUT_RISE_POSN			: 2;
			unsigned char 	CLKOUT_FALL_POSN 			: 2;
		} _bitval;
	} REG42;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 2;
			unsigned char 	PDN_GLOBAL 					: 1;
			unsigned char 	RSVD2						: 2;
			unsigned char 	LVDS_DATA_STRENGTH			: 1;
			unsigned char 	LVDS_CLKOUT_STRENGTH		: 1;
			unsigned char 	STBY						: 1;
		} _bitval;
	} REG45;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	HIGH_FREQ_MODE_CH_B			: 1;
			unsigned char 	RSVD						: 7;
		} _bitval;
	} REG4A;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	HIGH_FREQ_MODE_CH_A			: 1;
			unsigned char 	RSVD						: 7;
		} _bitval;
	} REG58;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 2;
			unsigned char 	CH_A_OFFSET_PEDESTAL		: 6;
		} _bitval;
	} REGBF;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 2;
			unsigned char 	CH_B_OFFSET_PEDESTAL		: 6;
		} _bitval;
	} REGC1;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 2;
			unsigned char 	OFFSET_CORR_TIME_CONSTANT	: 4;
			unsigned char 	RSVD2						: 1;
			unsigned char 	FREEZE_OFFSET_CORR			: 1;
		} _bitval;
	} REGCF;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 4;
			unsigned char 	EN_LOW_SPEED_MODE			: 1;
			unsigned char 	RSVD2						: 3;
		} _bitval;
	} REGEF;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	EN_LVDS_SWING				: 2;
			unsigned char 	RSVD						: 6;
		} _bitval;
	} REGF1;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 3;
			unsigned char 	LOW_SPEED_MODE_CH_A			: 1;
			unsigned char 	RSVD2						: 4;
		} _bitval;
	} REGF2;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 3;
			unsigned char 	HIGH_PERF_MODE_5			: 1;
			unsigned char 	HIGH_PERF_MODE_4			: 1;
			unsigned char 	RSVD2						: 3;
		} _bitval;
	} REGD5;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	RSVD						: 2;
			unsigned char 	HIGH_PERF_MODE_7			: 1;
			unsigned char 	HIGH_PERF_MODE_6			: 1;
			unsigned char 	RSVD2						: 4;
		} _bitval;
	} REGD7;
	union {
		unsigned char		_intval;
		struct {
			unsigned char 	LOW_SPEED_MODE_CH_B			: 1;
			unsigned char 	RSVD						: 4;
			unsigned char 	HIGH_PERF_MODE_8			: 1;
			unsigned char 	RSVD2						: 2;
		} _bitval;
	} REGDB;
};                                                  
