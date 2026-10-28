#ifndef __PROTOCOL_PRIVATE_H__
#define __PROTOCOL_PRIVATE_H__

#include "xil_types.h"
#include "device.h"
#include "protocol_ad4t.h"
#include "FreeRTOSConfig.h"

#define TRY_NO	0x30

#define SerBuffNo	4096

#define DEBUG_PORT	0
#define RCPU_PORT	1

/*FPGA File Select */
#define BAND_5M			0
#define BAND_10M		1
#define BAND_15M		2
#define BAND_20M		3

#define B700				0
#define B800				1
#define B1700				2
#define B2100				3

/* Protocol ���ɾ� ���� */

#define CHECK_DAS				0x08
#define	PSA_STAT_REQ			0x09
#define PSA_STAT_ACK			0x89

#define	DEVICE_ID_LOOKUP_REQ	0x0A
#define DEVICE_ID_LOOKUP_ACK	0x8A

#define	MAIN_STAT_REQ			0x00
#define MAIN_STAT_ACK			0x80
#define	MAIN_STAT_REQ2			0x02
#define MAIN_STAT_ACK2			0x82

#define MAIN_CTRL_REQ			0x01
#define MAIN_CTRL_ACK			0x81

#define TREE_REQ				0x60
#define TREE_ACK				0x61

#define	SYS_STAT_REQ			0x20
#define SYS_STAT_ACK			0xA0
#define SYS_CTRL_REQ			0x21
#define SYS_CTRL_ACK			0xA1

#define	MODEM_STAT_REQ			0x30
#define MODEM_STAT_ACK			0xB0
#define MODEM_CTRL_REQ			0x31
#define MODEM_CTRL_ACK			0xB1

#define ICS_STAT_REQ			0x02
#define ICS_STAT_ACK			0x82
#define ICS_CTRL_REQ			0x03
#define ICS_CTRL_ACK			0x83

#define TABLE_STAT_REQ			0x04
#define TABLE_STAT_ACK			0x84
#define TABLE_CTRL_REQ			0x05
#define TABLE_CTRL_ACK			0x85

#define TABLE_OFS_CTRL_REQ		0x22
#define TABLE_OFS_CTRL_ACK		0xA2

#define ADC_REQ					0x06
#define ADC_ACK					0x86

#define UNINSTALL_REQ			0x07
#define UNINSTALL_ACK			0x87

#define	MODEM_STAT_REQ			0x30
#define MODEM_STAT_ACK			0xB0
#define MODEM_CTRL_REQ			0x31
#define MODEM_CTRL_ACK			0xB1

#define M_MODEM_REQ				0x08
#define M_MODEM_ACK				0x88

#define DN_FIRM_START_REQ		0x10
#define DN_FIRM_START_ACK		0x90

#define DN_FIRM_DATA_REQ		0x11
#define DN_FIRM_DATA_ACK		0x91

#define DN_FIRM_END_REQ			0x12
#define DN_FIRM_END_ACK			0x92

#define DN_FPGA_START_REQ		0x13
#define DN_FPGA_START_ACK		0x93

#define DN_FPGA_DATA_REQ		0x14
#define DN_FPGA_DATA_ACK		0x94

#define DN_FPGA_END_REQ			0x15
#define DN_FPGA_END_ACK			0x95

#define LOG_LOAD_REQ			0x16
#define LOG_LOAD_ACK			0x96

#define LOG_ERASE_REQ			0x17
#define LOG_ERASE_ACK			0x97

#define LOG_CNT_REQ				0x18
#define LOG_CNT_ACK				0x98

#define SYS_RST_REQ				0x19
#define SYS_RST_ACK				0x99

/* Protocol-Ex ���� */
#define DOWN_START_REQ			0x10
#define DOWN_START_ACK			0x90

#define DOWN_DATA_REQ			0x11
#define DOWN_DATA_ACK			0x91

#define DOWN_END_REQ			0x12
#define DOWN_END_ACK			0x92

#define ERROR_STATUS			0xDA
#define LOG_LOAD_REQ			0x16
#define LOG_LOAD_ACK			0x96

#define LOG_ERASE_REQ			0x17
#define LOG_ERASE_ACK			0x97

#define LOG_CNT_REQ				0x18
#define LOG_CNT_ACK				0x98

#define SYS_RST_REQ				0x19
#define SYS_RST_ACK				0x99

#define FACTO_CTRL_REQ			0x33
#define FACTO_CTRL_ACK			0xB3

#define FACTO_LOAD_REQ			0x34
#define FACTO_LOAD_ACK			0xB4

#define LED_CTRL_REQ			0x35
#define LED_CTRL_ACK			0xB5

#define MODEM_OFF_CTRL_REQ		0x36
#define MODEM_OFF_CTRL_ACK		0xB6

// #define GUI_MAIN_STATUS_CMD				0x00
// #define GUI_MAIN_CONTROL_CMD			0x01

#define GUI_HID_STATUS_CMD				0xF0
#define GUI_HID_CONTROL_CMD				0xF1

// Table
#define GUI_TBL_STATUS_CMD				0x76
#define GUI_TBL_CONTROL_CMD				0x75

// Spectrum
#define GUI_DL_SPT_800M_IN_CMD			0x20
#define GUI_DL_SPT_800M_OUT_CMD			0x21
#define GUI_DL_SPT_1800M_IN_CMD			0x22
#define GUI_DL_SPT_1800M_OUT_CMD		0x23
#define GUI_DL_SPT_2100M_IN_CMD			0x24
#define GUI_DL_SPT_2100M_OUT_CMD		0x25

#define GUI_UL_SPT_800M_IN_CMD			0x26
#define GUI_UL_SPT_800M_OUT_CMD			0x27
#define GUI_UL_SPT_1800M_IN_CMD			0x28
#define GUI_UL_SPT_1800M_OUT_CMD		0x29
#define GUI_UL_SPT_2100M_IN_CMD			0x2A
#define GUI_UL_SPT_2100M_OUT_CMD		0x2B

#define GUI_DIG_CPU_START_CMD			0x0B
#define GUI_DIG_CPU_START2_CMD			0x0C
#define GUI_DIG_CPU_DATA_CMD			0x0D
#define GUI_DIG_CPU_END_CMD				0x0E

#define TOPOLOGY_REQ					0xBA
#define TOPOLOGY_RESP					0xBB
#define	TOPOLOGY_ACK					0xBB

#define StatusCMD					0x00	// �߰�� Staus
#define ControlCMD					0x01	// �߰�� Control
#define AlarmCMD					0x02	// �߰�� Alarm
#define DownloadCMD					0x0B	// �߰�� �ٿ�ε� 1,2�� �뺸
#define DownloadCMDConfirm			0x0C	// �߰�� �ٿ�ε� �뺸 Ȯ��
#define DownloadDataCMD				0x0D	// �߰�� �ٿ�ε� Data
#define DownloadDataCMDConfirm		0x0E	// �߰�� �ٿ�ε� Ȯ��
//#define TableControlCMD				0x75	// Table ����
//#define TableStatusCMD				0x76	// Table ��ȸ
#define TableControlCMD				0xF0	// Table ����
#define TableStatusCMD				0xF1	// Table ��ȸ
#define Conection_Report			0x05	// �߰�� ������� ����
#define AutoReportSCODE				0x04

//#define GUI_MAIN_STATUS_CMD			0x60
//#define GUI_MAIN_CONTROL_CMD		0x61
#define GUI_ICS_STATUS_CMD			0x62
#define GUI_ICS_CONTROL_CMD			0x63


#define NMSDownloadCMD				0x50	// �߰�� �ٿ�ε�
#define NMSDownloadConfirm			0x51	// �߰�� �ٿ�ε�




//#define	RF_CONTROL_ACK                  0x89

//#define   CRC_16_POLYNOMIAL 	0x1021
//#define   CRC_16_SEED			0x0000
//#define   CRC_16_STEP_SEED		(~((unsigned short) CRC_16_SEED))
//#define   CRC_TAB_SIZE			256

//////////////////////////////// AID

#define B28				0
#define B3				1
#define B1				2
#define N78				3


// Amp Reason
#define CASE_User				0x00		// ����ڿ� ����.
#define CASE_DeviceAutoSD		0x01		// Device Auto Shutdown
#define CASE_RvsFail			0x02		// ������ ���
#define CASE_TempHigh			0x03		// �µ� ����
#define CASE_OverPwr			0x04		// Over Power
#define CASE_PSU_SWOFF			0x05		// ���� Switch OFF
#define CASE_AmpLinkFail		0x06		// AMP Link Fail
#define CASE_VSWRFail			0x07		// VSWR Fail
#define CASE_LoopFail			0x08		// Loop Fail
#define CASE_DCFail				0x09		// DC Fail
#define CASE_SubPSU_SWOFF		0x0A		// SUB ���� Switch OFF
#define CASE_SwitchingFail		0x0B		// Switching �̻�
#define CASE_PLL_UNLOCK			0x0C		// PLL UNLOCK �̻�
#define CASE_PwrFail			0x0D		// �����̻�
#define CASE_DgBdErr			0x0E		// Digital Board �̻�
#define CASE_Unknown			0x0F		// Unknow
#define CASE_PDFail				0x10		// PD Fail
#define CASE_OverInPwr			0x14		// Over Power
#define CASE_SyncFail			0x17		// Sync  Fail
#define CASE_ISOFail			0x18		// ISO  Fail

#define CASE_ETC				0x19		// ISO  Fail



////////////////////////////////////////////////////////////////
//	Download ���� �ڷᱸ��
////////////////////////////////////////////////////////////////
#define	FIRM_MAX_SIZE			256*1024		/* 256KB */
#define	FIRM_MIN_SIZE			256*1024		/* 256KB */

#define	FPGA_MAX_SIZE			9*1024*1024		/*   9MB */
#define	FIRM_MAX_FRAME			256				/*  256Frame */
#define	FPGA_MAX_FRAME			3584			/* 3584Frame */

#define DL_FRAME_SIZE			256				/*  each data frame will be 256 bytes */

#define FILETYPE_FW_MODEM		1
#define FILETYPE_FW_MB			2
#define FILETYPE_FPGA			3
#define FILETYPE_PL_Y			4
#define FILETYPE_uMon			5
#define FILETYPE_ALL			6

#define MAX_TX_BUF_SIZE          2500
#define MAX_RX_BUF_SIZE          2500
#define SHORT_TX_BUF_SIZE        256

struct downParam {
	short				totFrame;
	short				curFrame;
	short				max_size;
	unsigned long       filesize;
	short				total_crc;

	char				down_status;
	int					f_offset;
	int 				f_start;
	unsigned long       realFileDataSize;

	int 				f_len;
	unsigned short		totCRC;
	unsigned char		version;	
};

struct downDesc {
	char				filename[16];
	int					filesize;
	char				filetype;
	char				compress;
	short				year;
	short				month;
	short				day;
	short				hous;
	short				minutes;
	short				seconds;
	char				down_status;
	char				*data;
	unsigned long       realFileDataSize;
	int					version;
	char				path[30];
	char				date[20];
};



#define _local_B1				231820000
#define _local_B3				135400000
#define _local_N78				368640000

extern int MainStatReq(char *buf, frame_info *fr_info);
extern int MainStatAck(char *buf, frame_info *fr_info, unsigned char cmd);
extern void MainCtrlReq(char *buf, short  *source_id, short  *dest_id);
extern int DeviceIDLookupAck(char *buf, unsigned char source_id[4], unsigned char dest_id[4], unsigned cmd);

extern int SysStatAck(char *buf, short source_id, short dest_id, short band, unsigned cmd);

extern int TreeAck(char *buf, int source_id, int dest_id, unsigned cmd);

extern unsigned char AnalyzeAidData(char *buf,unsigned char Type);

extern struct repeater_struct *getRptDev(void);

extern void getRepeaterInfo_AID();

extern int TopologyAck(char *buf, mu_dev_info *mu_info, frame_info *orig_fr_info);

#endif
