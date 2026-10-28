/* ADF4106 */

enum {
		
	__RCounter = 0,
	__NCounter,
	__FUNCSet,
	__INIT
};


/// Reference Counter Latch Map
typedef union {
	unsigned int Data;

	struct {
		unsigned int	tot		:24;
	} Grp;
	
	struct {
		unsigned int control	:2;			/// CONTROL  BITS
		unsigned int r_count	:14;		/// 14-BIT REFERENCE COUNTER

		unsigned int anti_bw	:2;			/// ANTI BACKLASH WIDTH
		unsigned int test_mode	:2;			/// TEST MODE BITS
		unsigned int LDPRE		:1;			/// LOCK Detect PRECISION 
		unsigned int _SYNC		:1;			/// SYNC
		unsigned int DLY		:1;			/// DLY
		unsigned int RSV		:1;			/// RESERVED

	} Bit;
} _RCounterReg;


/// AB Counter Latch Map
typedef union {
	unsigned int Data;

	struct {
		unsigned int tot		:24;
	} Grp;

	struct {
		unsigned int control		:2;				/// CONTROL  BITS
		unsigned int a_count		:6;				/// 6-BIT A COUNTER
		unsigned int b_count		:13;			/// 13-BIT B COUNTER

		unsigned int cp_gain	:1;					/// CP GAIN

		unsigned int RSV		:2;					/// RESERVED
	} Bit;
} _NCounterReg;


#if 0
enum {
    __ICP2_7K_1_09mA=0,		//	0
    __ICP2_7K_2_18mA,		//	1
    __ICP2_7K_3_26mA,		//	2
    __ICP2_7K_4_35mA,		//	3
    __ICP2_7K_5_44mA,		//	4
    __ICP2_7K_6_53mA,		//	5
    __ICP2_7K_7_62mA,		//	6
    __ICP2_7K_8_70mA,		//	7

};

enum {
    __ICP4_7K_0_63mA = 0,	// 	0
    __ICP4_7K_1_25mA,		// 	1
    __ICP4_7K_1_88mA,		// 	2
    __ICP4_7K_2_50mA,		// 	3
    __ICP4_7K_3_13mA,		// 	4
    __ICP4_7K_3_75mA,		// 	5
    __ICP4_7K_4_38mA,		// 	6
    __ICP4_7K_5_00mA = 7,	// 	7
};

enum {
    __ICP10K_0_29mA = 0,	// 	0
    __ICP10K_0_59mA,		// 	1
    __ICP10K_0_88mA,		// 	2
    __ICP10K_1_76mA,		// 	3
    __ICP10K_1_47mA,		// 	4
    __ICP10K_1_76_1mA,		// 	5
    __ICP10K_2_06mA,		// 	6
    __ICP10K_2_35mA,		// 	7
};

#endif

/// Function Latch Map
typedef union {
	unsigned int Data;

	struct {
		unsigned int tot			:24;
	} Grp;

	struct {
		unsigned int control		:2;			/// CONTROL  BITS

			#define __RSTDISABLED	0	// NORMAL
			#define __RSTENABLED	1	// R, A, B COUNTERS HELD IN RESET
		unsigned int CNTRST			:1;			/// COUNTER RESET

			#define __PWRUP			0
			#define __PWRDN			1
		unsigned int PWRDN			:1;			/// POWER DOWN 1

			#define __LDTRI			0	// TRI-STATE
			#define __LDDIG			1	// Digital Lock Deteck
			#define __NDIVOut		2	// N DIVIDER OUTPUT
			#define __LDHIGH		3	// DVDD  Active High
			#define __RDIVOut		4	// R DIVIDER OUTPUT
			#define __OpenDrain		5	// N Channel Open Drain
			#define __SDataOut		6	// Serial Data Out
			#define __LDLOW			7	// DGND Active Low
		unsigned int mux_sel		:3;			/// MUXOUT CONTROL
		
			#define __NEG			0
			#define __POS			1
		unsigned int pd_pol			:1; 		/// PD POLARITY

			#define __CPNOR			0
			#define __CPTRI			1
		unsigned int cp_tri		:1;				/// CP THREE STATE
			
			#define __NOFAST		0
		unsigned int FLOCKEN		:1;			/// FASTLOCK ENABLE

		unsigned int FLOCKCTRL		:1;			/// FASTLOCK MODE
		unsigned int TIMEOUTCNT		:4;			/// TIMER COUNTER CONTROL
		unsigned int CUR1SET		:3;			/// CURRENT SETTTING 1
		unsigned int CUR2SET		:3;			/// CURRENT SETTTING 2
		unsigned int PWREN     		:1;			/// POWER DOWN 2

			#define __PreScl_8 		0	// 8/9
			#define __PreScl_16 	1	// 16/17
			#define __PreScl_32		2	// 32/33
			#define __PreScl_64		3	// 64/65
		unsigned int PLVALUE		:2;			/// PRESCALER VALUE
	} Bit;
} _FUNCReg;




