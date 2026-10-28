#define CLK_CH0_EN	0x001
#define CLK_CH1_EN	0x002
#define CLK_CH2_EN	0x004
#define CLK_CH3_EN	0x008
#define CLK_CH4_EN	0x010
#define CLK_CH5_EN	0x020
#define CLK_CH6_EN	0x040
#define CLK_CH7_EN	0x080
#define CLK_CH8_EN	0x100
#define CLK_CH9_EN	0x200

#define CLKOUT_Power_Down 		0x0
#define CLKOUT_LVDS 			0x1
#define CLKOUT_LVPECL_700mVpp 	0x2
#define CLKOUT_LVPECL_1200mVpp 	0x3
#define CLKOUT_LVPECL_1600mVpp 	0x4
#define CLKOUT_LVPECL_2000mVpp 	0x5
#define CLKOUT_LVCMOS_NormInv 	0x6
#define CLKOUT_LVCMOS_InvNorm 	0x7
#define CLKOUT_LVCMOS_NormNorm 	0x8
#define CLKOUT_LVCMOS_InvInv 	0x9
#define CLKOUT_LVCMOS_LowNorm 	0xA
#define CLKOUT_LVCMOS_LowInv 	0xB
#define CLKOUT_LVCMOS_NormLow 	0xC
#define CLKOUT_LVCMOS_InvLow 	0xD
#define CLKOUT_LVCMOS_LowLow 	0xE

#define OSCOut0_Power_Down 		0x0
#define OSCOut0_LVDS 			0x1
#define OSCOut0_LVPECL_700mVpp 	0x2
#define OSCOut0_LVPECL_1200mVpp 0x3
#define OSCOut0_LVPECL_1600mVpp 0x4
#define OSCOut0_LVPECL_2000mVpp 0x5
#define OSCOut0_LVCMOS_NormInv 	0x6
#define OSCOut0_LVCMOS_InvNorm 	0x7
#define OSCOut0_LVCMOS_NormNorm 0x8
#define OSCOut0_LVCMOS_InvInv 	0x9
#define OSCOut0_LVCMOS_LowNorm 	0xA
#define OSCOut0_LVCMOS_LowInv 	0xB
#define OSCOut0_LVCMOS_NormLow 	0xC
#define OSCOut0_LVCMOS_InvLow 	0xD
#define OSCOut0_LVCMOS_LowLow 	0xE

#define OSCOut1_LVPECL_700mVpp 	0x0
#define OSCOut1_LVPECL_1200mVpp 0x1
#define OSCOut1_LVPECL_1600mVpp 0x2
#define OSCOut1_LVPECL_2000mVpp 0x3

#define OSCOut_DIV_2			0x2
#define OSCOut_DIV_3			0x3
#define OSCOut_DIV_4			0x4
#define OSCOut_DIV_5			0x5
#define OSCOut_DIV_6			0x6
#define OSCOut_DIV_7			0x7
#define OSCOut_DIV_8			0x0

#define VCO_DIV_2				0x2
#define VCO_DIV_3				0x3
#define VCO_DIV_4				0x4
#define VCO_DIV_5				0x5
#define VCO_DIV_6				0x6
#define VCO_DIV_7				0x7
#define VCO_DIV_8				0x0

#define MODE_DualPLL_IntVCO			0x0
#define MODE_DualPLL_IntVCO_0dly	0x2
#define MODE_DualPLL_ExtVCO			0x3
#define MODE_DualPLL_ExtVCO_0dly	0x5
#define MODE_PLL2_IntVCO			0x6
#define MODE_PLL2_IntVCO_0dly		0x8
#define MODE_PLL2_ExtVCO			0xB
#define MODE_Clock_Distribution		0x10

#define LD_MUX_PLL1_DLD				0x1
#define LD_MUX_PLL2_DLD				0x2
#define LD_MUX_PLL1_AND_PLL2_DLD	0x3

#define OUTPUT_PUSH_PULL			0x3
#define OUTPUT_INV_PUSH_PULL		0x4
#define OUTPUT_OPEN_SOURCE			0x5
#define OUTPUT_OPEN_DRAIN			0x6

#define HOLDOVER_MUX_PLL1_DLD			0x1
#define HOLDOVER_MUX_PLL2_DLD			0x2
#define HOLDOVER_MUX_PLL1_AND_PLL2_DLD	0x3
#define HOLDOVER_MUX_uWire_Readback		0x7

struct lmk0480x_struct {
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout0_1_DIV		: 11;
			unsigned int 	CLKout0_1_HS		:  1;
			unsigned int 	RESET				:  1;
			unsigned int 	CLKout0_1_DDLY		: 10;
			unsigned int 	CLKout0_ADLY_SEL	:  1;
			unsigned int 	CLKout1_ADLY_SEL	:  1;
			unsigned int 	ZERO				:  1;
			unsigned int 	CLKout0_1_PD		:  1;
		} _bitval;
	} REG00;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout2_3_DIV		: 11;
			unsigned int 	CLKout2_3_HS		:  1;
			unsigned int 	POWERDOWN			:  1;
			unsigned int 	CLKout2_3_DDLY		: 10;
			unsigned int 	CLKout2_ADLY_SEL	:  1;
			unsigned int 	CLKout3_ADLY_SEL	:  1;
			unsigned int 	ZERO				:  1;
			unsigned int 	CLKout2_3_PD		:  1;
		} _bitval;
	} REG01;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout4_5_DIV		: 11;
			unsigned int 	CLKout4_5_HS		:  1;
			unsigned int 	ZERO_1				:  1;
			unsigned int 	CLKout4_5_DDLY		: 10;
			unsigned int 	CLKout4_ADLY_SEL	:  1;
			unsigned int 	CLKout5_ADLY_SEL	:  1;
			unsigned int 	ZERO_2				:  1;
			unsigned int 	CLKout4_5_PD		:  1;
		} _bitval;
	} REG02;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout6_7_DIV		: 11;
			unsigned int 	CLKout6_7_HS		:  1;
			unsigned int 	ZERO_1				:  1;
			unsigned int 	CLKout6_7_DDLY		: 10;
			unsigned int 	CLKout6_ADLY_SEL	:  1;
			unsigned int 	CLKout7_ADLY_SEL	:  1;
			unsigned int 	CLKout6_7_OSCIn_Sel	:  1;
			unsigned int 	CLKout6_7_PD		:  1;
		} _bitval;
	} REG03;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout8_9_DIV		: 11;
			unsigned int 	CLKout8_9_HS		:  1;
			unsigned int 	ZERO_1				:  1;
			unsigned int 	CLKout8_9_DDLY		: 10;
			unsigned int 	CLKout8_ADLY_SEL	:  1;
			unsigned int 	CLKout9_ADLY_SEL	:  1;
			unsigned int 	CLKout8_9_OSCIn_Sel	:  1;
			unsigned int 	CLKout8_9_PD		:  1;
		} _bitval;
	} REG04;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout10_11_DIV		: 11;
			unsigned int 	CLKout10_11_HS		:  1;
			unsigned int 	ZERO_1				:  1;
			unsigned int 	CLKout10_11_DDLY		: 10;
			unsigned int 	CLKout10_ADLY_SEL	:  1;
			unsigned int 	CLKout11_ADLY_SEL	:  1;
			unsigned int 	ZERO_2				:  1;
			unsigned int 	CLKout10_11_PD		:  1;
		} _bitval;
	} REG05;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout0_1_ADLY		:  5;
			unsigned int 	ZERO				:  1;
			unsigned int 	CLKout2_3_ADLY		:  5;
			unsigned int 	CLKout0_TYPE		:  4;
			unsigned int 	CLKout1_TYPE		:  4;
			unsigned int 	CLKout2_TYPE		:  4;
			unsigned int 	CLKout3_TYPE		:  4;
		} _bitval;
	} REG06;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout4_5_ADLY		:  5;
			unsigned int 	ZERO				:  1;
			unsigned int 	CLKout6_7_ADLY		:  5;
			unsigned int 	CLKout4_TYPE		:  4;
			unsigned int 	CLKout5_TYPE		:  4;
			unsigned int 	CLKout6_TYPE		:  4;
			unsigned int 	CLKout7_TYPE		:  4;
		} _bitval;
	} REG07;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	CLKout8_9_ADLY		:  5;
			unsigned int 	ZERO				:  1;
			unsigned int 	CLKout10_11_ADLY		:  5;
			unsigned int 	CLKout8_TYPE		:  4;
			unsigned int 	CLKout9_TYPE		:  4;
			unsigned int 	CLKout10_TYPE		:  4;
			unsigned int 	CLKout11_TYPE		:  4;
		} _bitval;
	} REG08;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	FEEDBACK_MUX		:  3;
			unsigned int 	VCO_DIV				:  3;
			unsigned int 	EN_FEEDBACK_MUX		:  1;
			unsigned int 	VCO_MUX				:  1;
			unsigned int 	RSVD1				:  3;
			unsigned int 	OSCout_DIV			:  3;
			unsigned int 	PD_OSCin			:  1;
			unsigned int 	OSCout0_MUX			:  1;
			unsigned int 	OSCout1_MUX			:  1;
			unsigned int 	EN_OSCout0			:  1;
			unsigned int 	EN_OSCout1			:  1;
			unsigned int 	OSCout0_TYPE		:  4;
			unsigned int 	RSVD2				:  2;
			unsigned int 	OSCout1_LVPECL_AMP	:  2;
		} _bitval;
	} REG10;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	EN_PLL2_XTAL		:  1;
			unsigned int 	RSVD1				:  6;
			unsigned int 	SYNC_TYPE			:  3;
			unsigned int 	SYNC_EN_AUTO		:  1;
			unsigned int 	SYNC_POL_INV		:  1;
			unsigned int 	SYNC_QUAL			:  1;
			unsigned int 	SYNC_MUX			:  2;
			unsigned int 	NO_SYNC_CLKout0_1	:  1;
			unsigned int 	NO_SYNC_CLKout2_3	:  1;
			unsigned int 	NO_SYNC_CLKout4_5	:  1;
			unsigned int 	NO_SYNC_CLKout6_7	:  1;
			unsigned int 	NO_SYNC_CLKout8_9	:  1;
			unsigned int 	NO_SYNC_CLKout10_11	:  1;
			unsigned int 	EN_SYNC				:  1;
			unsigned int 	MODE				:  4;
		} _bitval;
	} REG11;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	RSVD1				:  1;
			unsigned int 	HOLDOVER_MODE		:  2;
			unsigned int 	EN_TRACK			:  1;
			unsigned int 	RSVD2				: 13;
			unsigned int 	SYNC_PLL1_DLD		:  1;
			unsigned int 	SYNC_PLL2_DLD		:  1;
			unsigned int 	LD_TYPE				:  3;
			unsigned int 	LD_MUX				:  5;
		} _bitval;
	} REG12;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	EN_CLKin0			:  1;
			unsigned int 	EN_CLKin1			:  1;
			unsigned int 	RSVD1				:  1;
			unsigned int 	CLKin_Sel_INV		:  1;
			unsigned int 	CLKin_Select_MODE	:  3;
			unsigned int 	Status_CLKin0_MUX	:  3;
			unsigned int 	DISABLE_DLD1_DET	:  1;
			unsigned int 	Status_CLKin0_TYPE	:  3;
			unsigned int 	RSVD2				:  1;
			unsigned int 	Status_CLKin1_MUX	:  3;
			unsigned int 	RSVD3				:  1;
			unsigned int 	HOLDOVER_TYPE		:  3;
			unsigned int 	HOLDOVER_MUX		:  5;
		} _bitval;
	} REG13;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	EN_VTUNE_RAIL_DET	:  1;
			unsigned int 	DAC_LOW_TRIP		:  6;
			unsigned int 	RSVD1				:  2;
			unsigned int 	DAC_HIGH_TRIP		:  6;
			unsigned int 	CLKin0_BUF_TYPE		:  1;
			unsigned int 	CLKin1_BUF_TYPE		:  1;
			unsigned int 	RSVD2				:  2;
			unsigned int 	Status_CLKin1_TYPE	:  3;
			unsigned int 	RSVD3				:  1;
			unsigned int 	EN_LOS				:  1;
			unsigned int 	RSVD4				:  1;
			unsigned int 	LOS_TIMEOUT			:  2;
		} _bitval;
	} REG14;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	FORCE_HOLDOVER		:  1;
			unsigned int 	HOLDOVER_DLD_CNT	: 14;
			unsigned int 	EN_MAN_DAC			:  1;
			unsigned int 	RSVD1				:  1;
			unsigned int 	MAN_DAC				: 10;
		} _bitval;
	} REG15;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	RSVD1				: 25;
			unsigned int 	XTAL_LVL			:  2;
		} _bitval;
	} REG16;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	RSVD1				:  1;
			unsigned int 	PLL1_WIND_SIZE		:  2;
			unsigned int 	PLL1_R_DLY			:  3;
			unsigned int 	RSVD2				:  1;
			unsigned int 	PLL1_N_DLY			:  3;
			unsigned int 	RSVD3				:  1;
			unsigned int 	PLL2_R3_LF			:  3;
			unsigned int 	RSVD4				:  1;
			unsigned int 	PLL2_R4_LF			:  3;
			unsigned int 	RSVD5				:  1;
			unsigned int 	PLL2_C3_LF			:  4;
			unsigned int 	PLL2_C4_LF			:  4;
		} _bitval;
	} REG24;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	RSVD1				:  1;
			unsigned int 	PLL1_DLD_CNT		: 14;
			unsigned int 	RSVD2				:  2;
			unsigned int 	DAC_CLK_DIV			: 10;
		} _bitval;
	} REG25;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	PLL2_CP_TRI			:  1;
			unsigned int 	PLL2_DLD_CNT		: 14;
			unsigned int 	RSVD2				:  6;
			unsigned int 	PLL2_CP_GAIN		:  2;
			unsigned int 	PLL2_CP_POL			:  1;
			unsigned int 	EN_PLL2_REF_2X		:  1;
			unsigned int 	PLL2_WND_SIZE		:  2;
		} _bitval;
	} REG26;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	PLL1_CP_TRI			:  1;
			unsigned int 	PLL1_R				: 14;
			unsigned int 	CLKin0_PreR_DIV		:  2;
			unsigned int 	CLKin1_PreR_DIV		:  2;
			unsigned int 	RSVD1				:  2;
			unsigned int 	PLL1_CP_GAIN		:  2;
			unsigned int 	PLL1_CP_POL			:  1;
			unsigned int 	RSVD2				:  3;
		} _bitval;
	} REG27;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	RSVD1				:  1;
			unsigned int 	PLL1_N				: 14;
			unsigned int 	PLL2_R				: 12;
		} _bitval;
	} REG28;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	PLL2_N_CAL			: 18;
			unsigned int 	PLL2_FAST_PDF		:  1;
			unsigned int 	OSCin_FREQ			:  3;
			unsigned int 	RSVD1				:  5;
		} _bitval;
	} REG29;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	PLL2_N				: 18;
			unsigned int 	RSVD1				:  1;
			unsigned int 	PLL2_P				:  3;
			unsigned int 	RSVD2				:  5;
		} _bitval;
	} REG30;
	union {
		unsigned int		_intval;
		struct {
			unsigned int 	Address				:  5;
			unsigned int 	uWire_LOCK			:  1;
			unsigned int 	RSVD1				: 10;
			unsigned int 	READBACK_ADDR		:  5;
			unsigned int 	READBACK_LE			:  1;
			unsigned int 	RSVD2				: 10;
		} _bitval;
	} REG31;
};
