/* ics register map */
#define ICS_REG_BASE			0x8000
#define ICS_REG_SIZE			128

#define ICS_VERSION				0x00
#define ICS_CONFIG0				0x00
#define ICS_CONFIG1				0x04
#define ICS_CONFIG2				0x08
#define ICS_CONFIG3				0x0C

#define ICS_DELAY				0x08
#define FA_THR_LVL				0x0C
//#define SLEEP_PERIOD			0x3C

#define DL_AVG_DET				0x24
#define DL_PEAK_DET				0x26
#define UL_AVG_DET				0x24
#define UL_GAIN_DET				0x26

#define MU_LVL1					0x3C
#define MU_LVL2					0x48
#define MU_LVL3					0x7C

#define MU_CFG					0x44

///////////////////////////////////////////////////////////////////////////////////////
//	ICS Register Map
///////////////////////////////////////////////////////////////////////////////////////
#define ICS_DELAY1				0x08

#define ICS_AGC_SETP			0x3C
#define FA1_AGC_GAIN			0x10
#define FA2_AGC_GAIN			0x10
#define FA3_AGC_GAIN			0x14
#define FA4_AGC_GAIN			0x14

#define ICS_USR_GAIN			0x20
#define ICS_ILC_GAIN			0x3C

#define ICS_ADC_PWR				0x2C		// 추후 수정되어야 함

#define LTE_DDC_PWR				0x00
#define LTE_DDC_PKK				0x04
#define LTE_LMS_PWR				0x08
#define LTE_LMS_PKK				0x0C
#define LTE_FBK_PWR				0x10
#define LTE_FBK_PKK				0x14
#define LTE_ATT_PWR				0x18
#define LTE_ATT_PKK				0x1C
#define LTE_USR_PWR				0x20
#define LTE_USR_PKK				0x24

///////////////////////////////////////////////////////////////////////////////////////
//	LTE Register Map
///////////////////////////////////////////////////////////////////////////////////////
#define LTE_DELAY1				0x08
#define LTE_DELAY2				0x08
#define LTE_DELAY3				0x04

#define LTE_ALC_SETP			0x10			/* */
#define LTE_ILC_TGET			0x30			/* */
#define LTE_ILC_SETP			0x14			/* */
#define LTE_LPC_SETP			0x18			/* */

#define LTE_AGC_SETP			0x18
#define LTE_MAN_GAIN			0x28
#define LTE_REF_GAIN			0x28

#define LTE_USR_GAIN			0x20
#define LTE_ATT_GAIN			0x20
#define LTE_ALC_GAIN			0x24
#define LTE_LIM_GAIN			0x24
#define LTE_MAN_GAIN			0x28
#define LTE_REF_GAIN			0x28
#define LTE_ILC_TATT			0x2C
#define LTE_ILC_GAIN			0x2C
#define LTE_ILC_X_TH			0x30
#define LTE_LMS_NOW				0x1C
#define LTE_LMS_TGET			0x34
#define LTE_LMS_STEP			0x38

#define SLEEP_PERIOD			0x70

#define LTE_ADC_PWR				0x20

/* ICS G LED Alarm */
#define DL_INPUT				0x0A
#define LED_ISO					0x0A

#define ALARM_DL_INPUT			0x100

#define ICS_ON      			0x00010000
#define AGC_ON      			0x00020000
#define ALC_ON      			0x00040000
#define DAC_ON      			0x00080000
#define LPC_ON      			0x00800000
#define FA_MODE      			0x01000000
#define ILC_ON      			0x02000000
#define LIM_ON      			0x04000000
                            	
#define PATH_ON					0x0010
                            	
                            	
#define	WIN_ON					0x0040
                            	
#define LAMDA					0x0080

/* Frtek KT향
 * Shift Register Field 정의 */
#define SR_START			0x4000	
#define SR_DONE				0x8000	
#define SR_DL_HPA_EN		0x0001
#define SR_UL_HPA_EN		0x0002
#define SR_PLL_EN			0x0004
#define SR_DL_ATT1_EN		0x0008
#define SR_DL_ATT2_EN		0x0010
#define SR_UL_ATT1_EN		0x0020
#define SR_UL_ATT2_EN		0x0040
#define SR_UL_ATT3_EN		0x0080
#define SR_COMM_CLK			0x0100
#define SR_COMM_DATA		0x0200

#define ANT_DONOR_SEL      	0x80000000
#define ANT_AUTO_ONOFF		0x40000000

///////////////////////////////////////////////////////////////////////////////////////
// Private 함수들 
extern unsigned int getIcsOffset(struct ics_struct *pdev, int path);
extern unsigned int getIcsUnitTap(struct ics_struct *pdev, int path);
extern unsigned int getIcsNumbTap(struct ics_struct *pdev, int path);
extern unsigned int getIcsNumbFa(struct ics_struct *pdev);

extern int cal_power(int val);
///////////////////////////////////////////////////////////////////////////////////////

#define TDD_CFG_0				0x40
#define TDD_CFG_1				0x44
#define TDD_CFG_2				0x48
#define TDD_CFG_3				0x4C
#define TDD_CFG_4				0x50
#define TDD_CFG_5				0x54
#define TDD_CFG_6				0x58
#define TDD_CFG_7				0x5C

#define TDD_EXT      			0x80000000

