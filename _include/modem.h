////////////////////////////////////////////////////////////////////
//                                                                //
//                    Download Type Define                        // 
//                                                                //
////////////////////////////////////////////////////////////////////
#define DAU_Download		0x01
#define SAU_Download		0x02
#define DAU_SAU_Download 	0x03
#define MU_Download			0x04
#define DAU_MU_Download		0x05
#define SAU_MU_Download 	0x06
#define ALL_Download		0x07
#define IP3_Download		0x08

////////////////////////////////////////////////////////////////////
//                                                                //
//                      BIT Mask Define                           // 
//                                                                //
////////////////////////////////////////////////////////////////////
#define BIT0				0x01				// LSB
#define BIT1				0x02
#define BIT2 				0x04
#define BIT3				0x08
#define BIT4				0x10
#define BIT5			 	0x20
#define BIT6				0x40
#define BIT7				0x80				// MSB

////////////////////////////////////////////////////////////////////
//                                                                //
//                      Alarm Type Define                         // 
//                                                                //
////////////////////////////////////////////////////////////////////
#define SYSTEM0				0x00				
#define M700RF				0x01
#define M900RF 				0x02
#define M1700RF				0x03
#define M2100RF				0x04

// LTE Modem Status
#define LTE_NORMAL				0
//#define STATUS_ERROR			1
#define USIM_ERROR				1
#define LTE_MODEM_SIGNAL_ERROR	2
#define LTE_MODEM_LINK_FAIL		3
#define LTE_NET_CONNNECT_FAIL	4		
////////////////////////////////////////////////////////////////////
//                                                                //
//                      Define Alarm Mask                         // 
//                                                                //
////////////////////////////////////////////////////////////////////
#define 	tempHighAlarmMaskFlag 				1
#define 	linkFailAlarmMaskFlag				2
#define 	sauInputVoltageAlarmMaskFlag 		3
#define 	dauInputVoltageAlarmMaskFlag 		4
#define 	sauMuCommunicationAlarmMaskFlag 	5
#define 	m700IsolationAlarmMaskFlag			6
#define 	m900IsolationAlarmMaskFlag 			7
#define 	m1700IsolationAlarmMaskFlag 		8	
#define 	m2100IsolationAlarmMaskFlag 		9
#define 	m700DlShutdownAlarmMaskFlag 		10
#define 	m900DlShutdownAlarmMaskFlag 		11
#define 	m1700DlShutdownAlarmMaskFlag 		12
#define 	m2100DlShutdownAlarmMaskFlag 		13
#define 	m700UlShutdownAlarmMaskFlag 		14
#define 	m900UlShutdownAlarmMaskFlag 		15
#define 	m1700UlShutdownAlarmMaskFlag 		16
#define 	m2100UlShutdownAlarmMaskFlag 		17
#define 	m700LowPowerAlarmMaskFlag			18
#define 	m900LowPowerAlarmMaskFlag 			19
#define 	m1700LowPowerAlarmMaskFlag 			20
#define 	m2100LowPowerAlarmMaskFlag 			21
#define 	m700PllLockDetectAlarmMaskFlag 		22
#define 	m900PllLockDetectAlarmMaskFlag 		23
#define 	m1700PllLockDetectAlarmMaskFlag 	24
#define 	m2100PllLockDetectAlarmMaskFlag 	25	
#define 	m700AmpAlarmMaskFlag 				26
#define 	m900AmpAlarmMaskFlag 				27
#define 	m1700AmpAlarmMaskFlag 				28
#define 	m2100AmpAlarmMaskFlag 				29
#define 	muLowInputVoltageAlarmMaskFlag 		30
#define 	muInternalVoltageAlarmMaskFlag 		31
#define 	ruAlarmMaskFlag						32
#define 	br1CurrentAlarmMaskFlag				33
#define 	br2CurrentAlarmMaskFlag 			34
#define 	br3CurrentAlarmMaskFlag 			35
#define 	br4CurrentAlarmMaskFlag				36
////////////////////////////////////////////////////////////////////
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef char char8;
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;
typedef int sint32;
#if 0
/////////// System Info ///////////////////////////////////
#define	RPT_MODEL_NAME					101
#define RPT_TYPE						102
//#define RPT_MAKER						103
#define SERIAL_NUM						104
#define	FIRM_VER         				106
#define	FPGA_VER         				107
#define	MUFW_VER         				108
#define MODEM_TYPE						109
#define MODEM_MAKER						110
#define MODEM_VERSION					111
#define MODEM_MSN_NUMBER				112
#define MODEM_IMEI						113
#define MODEM_LOCAL_IP_ADDR				114
#define TARGET_LOCAL_IP_ADDR1			115
#define TARGET_LOCAL_IP_ADDR2			116
#define GW_PHONE_OR_IP_ADDR				117
#define GW_USER_ID						118
#define GW_PASSWORD						119

#define TEMP                    		122
#define TEMP_HIGH               		123
#define BAND_SEL                		124
#define SERVICE_AVAILABILITY			125
#define HEARTBEAT_TIME					126
#define HEARTBEAT_CTRL					127
#define AUTO_RESET_TIME					128
#define ALARM_DURATION					129
#define TEMP_HIGH_ALARMSTATE       		130
#define	SLEEP_MODE_ONOFF				131
#define SLEEP_MODE_STATE				132
#define RPT_RESET						133
#define MODEM_RESET						134
#define FW_DOWNLOAD						135
#define FW_DOWNLOAD_RESULT				136
#define FW_DOWNLOAD_IP_ADDR				137
#define FW_DOWNLOAD_FILE_PATH			138
#define FW_NAME							139
#define FW_DOWNLOAD_USER_ID				140
#define FW_DOWNLOAD_PASSWORD			141
#define BOOT_TIME						142
#define SYSTEM_TIME						143

/////////// MODEM Info ///////////////////////////////////
#define	MODEM_TX_POWER					201
#define MODEM_RSSI						202
#define	MODEM_SINR						203
#define MODEM_PLMN						204
#define MODEM_RAT						205
#define MODEM_CHANNEL_INFO				206
#define MODEM_BLOCK_ERROR_RATE			207
#define MODEM_PCI						208
#define MODEM_ICCID						209
#define LTE_MODEM_STATUS				210
#define MODEM_PSC						211
#define MODEM_RSRP						212
#define MODEM_RSCP						213
#define	MODEM_RSRQ						214
#define MODEM_ECIO						215

/////////// 1.7G RF Info ///////////////////////////////////
#define DL_GAIN							501
#define	DL_ALC_LEVEL					502
#define	DL_ALC_ONOFF					503
#define	DL_ISO_RECK_ONOFF				504
#define	ISO_VAL							505
#define DL_IN_PWR						506
#define	DL_OUT_PWR						507
#define	DL_AMP_ONOFF 					508
#define DL_ASD_ONOFF					509
#define DL_ASD_LEVEL					510
#define	DLUL_GAIN_OFFSET_ONOFF			511
#define	DLUL_GAIN_OFFSET_LEVEL			512
#define	ISO_ALARM_STATE					513
#define	DL_SHUTDOWN_ALARM_STATE			514
#define	AMP_ALARM_STATE					515
#define	PLL_ALARM_STATE					516
#define UL_GAIN							517
#define UL_IN_PWR						518
#define	UL_OUT_PWR						519
#define	UL_AMP_ONOFF					520
#define	UL_ALC_LEVEL					521
#define	UL_ALC_ONOFF					522
#define UL_ASD_ONOFF					523
#define UL_ASD_LEVEL					524
#define	UL_SHUTDOWN_ALARM_STATE			525
#define	LOWPOWER_ALARM_STATE			526
#define	DL_ISO_VAL						527
#define	UL_ISO_VAL						528
#define	DL_INPUT_HIGH_PWR_ALARM_STATE	529
#define	UL_INPUT_HIGH_PWR_ALARM_STATE	530
#define	DL_INPUT_HIGH_PWR_ALARM_LEVEL	531
#define	LOWPOWER_ALARM_LEVEL			532
#define	UL_INPUT_HIGH_PWR_ALARM_LEVEL	533
#define	ISO_ALARM_LEVEL					534
#define	UL_SLEEP_LEVEL					535

/////////// Alarm Mask ///////////////////////////////////
#define ALARM_MAX_CNT		         	3600       // Max Alarm Duration 60*60 초 
#define NETWORK_ACCESS_DELAY			600        // 10분
#define TEMP_HIGH_ALARM_MASK         	901
#define LINKFAIL_ALARM_MASK             902
#define ISO_ALARM_MASK              	908
#define DL_SHUTDOWN_ALARM_MASK			912
#define UL_SHUTDOWN_ALARM_MASK			916
#define LOWPOWER_ALARM_MASK            	920
#define PLL_ALARM_MASK					924
#define AMP_ALARM_MASK					928
#define DL_INPUT_HIGH_PWR_ALARM_MASK	938
#define UL_INPUT_HIGH_PWR_ALARM_MASK	939
#endif

#define NETWORK_ACCESS_DELAY			600        // 10분

////////// DownLoad Results //////////////////////////////
#define DOWNLOAD_SUCCESS 	0x00000000
#define IP3_CRC_ERROR    	0x00000002
#define NO_ACCESS_SEREVR   	0x00000008
#define DIFF_FILE_COMPANY   0x00000010
#define IP3_UPDATE_ERROR    0x00000200
#define FILE_PATH_ERROR     0x00002000
#define FILE_NAME_ERROR     0x00020000

/* 0x0000 0002: Fail - SAU CRC Error or IP3 CRC Error
0x0000 0004: Fail - MU CRC Error
0x0000 0008: Fail - No access to FW Download Server 
0x0000 0010: Fail - Different SAU File and Repeater companies or Different IP3 File and Repeater companies
0x0000 0020: Fail - Different DAU File and Repeater companies
0x0000 0040: Fail - Different MU File and Repeater companies
0x0000 0100: Fail - DAU Flash Update Error 
0x0000 0200: Fail - SAU Flash Update Error or IP3 Flash Update Error 
0x0000 0400: Fail - MU Flash Update Error
0x0000 0800 : Fail - Downa Load Command Error
0x0000 1000 : Fail - DAU Download Server Path Error
0x0000 2000 : Fail - SAU Download Server Path Error or IP3 Download Server Path Error
0x0000 4000 : Fail - MU Download Server Path Error
0x0001 0000 : Fail - DAU Download Server File name Error
0x0002 0000 : Fail - SAU Download Server File name Error or IP3 Download Server File name Error
0x0004 0000 : Fail - MU Download Server File name Error
// 정기보고송신 시, 이 항목은 제외   */
/////////////////////////////////////////////////////////////////////////////////////////////////

extern	int Prem1700tempHighAlarm1;
extern	unsigned int m1700tempHighAlarmCnt1;
extern	int Prem1700IsolationAlarm1;	
extern	unsigned int m1700IsolationAlarmCnt1;
extern	int Prem1700DlShutdownAlarm1;	
extern	unsigned int m1700DlShutdownAlarmCnt1;
extern	int Prem1700UlShutdownAlarm1;
extern	unsigned int m1700UlShutdownAlarmCnt1;
extern	int Prem1700LowPowerAlarm1;		
extern	unsigned int m1700LowPowerAlarmCnt1;
extern	int Prem1700PllLockDetectAlarm1;		
extern	unsigned int m1700LowPowerAlarmCnt1;
extern	int Prem1700DlInputHighPowerAlarm1; 	
extern	unsigned int m1700DlInputHighPowerAlarmCnt1;
extern	int Prem1700UlInputHighPowerAlarm1; 
extern	unsigned int m1700UlInputHighPowerAlarmCnt1;
extern	int Prem1700tempHighAlarm2;
extern	int Prem1700IsolationAlarm2 ;		
extern	int Prem1700DlShutdownAlarm2;			
extern	int Prem1700UlShutdownAlarm2;			
extern	int Prem1700LowPowerAlarm2;			
extern	int Prem1700PllLockDetectAlarm2;					
extern	int Prem1700DlInputHighPowerAlarm2; 	
extern	int Prem1700UlInputHighPowerAlarm2; 	
extern	int tempHighAlarmState;
extern	int m1700IsolationAlarmState;		
extern	int m1700DlShutdownAlarmState;			
extern	int m1700UlShutdownAlarmState;			
extern	int m1700LowPowerAlarmState;			
extern	int m1700PllLockDetectAlarmState;					
extern	int m1700DlInputHighPowerAlarmState; 	
extern	int m1700UlInputHighPowerAlarmState; 	

extern	unsigned short heartbeatduration;
extern	int fwdownloadtrap1, fwdownloadtrap2;
extern	unsigned int AlarmCheckCnt; 
extern	double heartbeatDuration, Alarm_Duration, Auto_Reset_Duration; 
extern	unsigned int AlarmCheckCnt; 
//extern	time_t heartbeatSentTime;
extern  void AlarmCheck();
extern  int check_heartbeattime();
extern  void saveSnmpInfo();
extern  int loadSnmpInfo();
//extern  time_t start_t, end_t, start_autoreset, end_autoreset, t, t1, t2, t3;
extern  int send_boot;

///////////////////////////////////////////////////////////////////////////////
typedef struct __attribute__((__packed__))	 {
///////////		Add IP-3 MIB Data					//////////////////////////////////////////////////////////////////////////////
		char	repeaterModelName[11];			    // const oid repeaterModelName_oid[] = { 1,3,6,1,4,1,56094,1,1,1 };
		char	repeaterType[11];			        // const oid repeaterType_oid[] = { 1,3,6,1,4,1,56094,1,1,2 };
		char	repeaterMaker[11];					// const oid repeaterMaker_oid[] = { 1,3,6,1,4,1,56094,1,1,3 };
		char	serialNumber[21];					// const oid serialNumber_oid[] = { 1,3,6,1,4,1,56094,1,1,4 };
		char	firmwareVersion[5]; 				// const firmwareVersion_oid[];  = { 1,3,6,1,4,1,56094,1,1,6 };
		char	blank2;
		char	FPGAVersion[5]; 					// const oid FPGAVersion_oid[] = { 1,3,6,1,4,1,56094,1,1,7 };
		char	muFirmwareVersion[5];				// const oid muFirmwareVersion_oid[] = { 1,3,6,1,4,1,56094,1,1,8 };
		char	modemType[11];						// const oid modemType_oid[] = { 1,3,6,1,4,1,56094,1,1,9 };
		char    blank3;  
		char	modemMaker[16]; 					// const oid modemMaker_oid[] = { 1,3,6,1,4,1,56094,1,1,10 };
		char	modemVersion[6];					// const oid modemVersion_oid[] = { 1,3,6,1,4,1,56094,1,1,11 };
		char    blank1[2];							// 2022-06-20 추가 
		char	modemMsnNumber[21]; 				// const oid modemMsnNumber_oid[] = { 1,3,6,1,4,1,56094,1,1,12 };
		char	modemIMEI[26];						// const oid modemIMEI_oid[] = { 1,3,6,1,4,1,56094,1,1,13 };
		u32 	modemLocalIpAddress;				// const oid modemLocalIpAddress_oid[] = { 1,3,6,1,4,1,56094,1,1,14 };
		u32 	targetLocalIpAddress1;				// const oid targetLocalIpAddress1_oid[] = { 1,3,6,1,4,1,56094,1,1,15 };
		u32 	targetLocalIpAddress2;				// const oid targetLocalIpAddress2_oid[] = { 1,3,6,1,4,1,56094,1,1,16 };
		char	gwAPNNumber[31];					// const oid gwAPNNumber_oid[] = { 1,3,6,1,4,1,56094,1,1,17 };
		char	gwUserID[21];						// const oid gwUserID_oid[] = { 1,3,6,1,4,1,56094,1,1,18 };
		char	gwPassword[21]; 					// const oid gwPassword_oid[] = { 1,3,6,1,4,1,56094,1,1,19 };
		int 	temperature;						// const oid temperature_oid[] = { 1,3,6,1,4,1,56094,1,1,22 };
		int 	tempHigh;							// const oid tempHigh_oid[] = { 1,3,6,1,4,1,56094,1,1,23 };
		int 	bandSelectStatus;					// const oid bandSelectStatus_oid[] = { 1,3,6,1,4,1,56094,1,1,24 };
		int 	serviceAvailablility;				// const oid serviceAvailablility_oid[] = { 1,3,6,1,4,1,56094,1,1,25 };
		unsigned short	heartbeatTime;				// const oid heartbeatTime_oid[] = { 1,3,6,1,4,1,56094,1,1,26 };
		//char	heartbeatTime[4];		
		int 	heartbeatControl;					// const oid heartbeatControl_oid[] = { 1,3,6,1,4,1,56094,1,1,27 };
		int 	autoResetTime;						// const oid autoResetTime_oid[] = { 1,3,6,1,4,1,56094,1,1,28 };
		unsigned short		alarmDuration;			// const oid alarmDuration_oid[] = { 1,3,6,1,4,1,56094,1,1,29 };								
		//char	alarmDuration[4];		
		int 	tempHighAlarmState; 				// const oid tempHighAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,1,30 };
		int 	linkFailAlarmState; 				// const oid linkFailAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,1,31 };
		int 	sleepModeOnOff; 					// const oid sleepModeOnOff_oid[] = { 1,3,6,1,4,1,56094,1,1,35 };
		int 	sleepMode; 					        // const oid sleepModeState_oid[] = { 1,3,6,1,4,1,56094,1,1,36 };
		int 	repeaterReset;						// const oid repeaterReset_oid[] = { 1,3,6,1,4,1,56094,1,1,37 };
		int 	modemReset; 						// const oid modemReset_oid[] = { 1,3,6,1,4,1,56094,1,1,38 };
		int 	fwDownload; 						// const oid fwDownload_oid[] = { 1,3,6,1,4,1,56094,1,1,39 };
		int 	fwDownloadResult;					// const oid fwDownloadResult_oid[] = { 1,3,6,1,4,1,56094,1,1,40 };
		u32		fwDownloadServerIpAddress;			// const oid fwDownloadServerIpAddress_oid[] = { 1,3,6,1,4,1,56094,1,1,41 };
		char	fwDownloadFilePath[51]; 			// const oid fwDownloadFilePath_oid[] = { 1,3,6,1,4,1,56094,1,1,42 };
		//char	fwName[20]; 						// const oid fwName_oid[] = { 1,3,6,1,4,1,56094,1,1,43 };
		char	fwDownloadServerUserId[21]; 		// const oid fwDownloadServerUserId_oid[] = { 1,3,6,1,4,1,56094,1,1,46 };
		char	fwDownloadServerUserPassword[21];	// const oid fwDownloadServerUserPassword_oid[] = { 1,3,6,1,4,1,56094,1,1,47 };
		char	bootTime[21];						// const oid bootTime_oid[] = { 1,3,6,1,4,1,56094,1,1,48 };
		int     m1700InputVolt;						// const oid bootTime_oid[] = { 1,3,6,1,4,1,56094,1,1,50 };
		int     m1700InputVoltAlarmState;           // const oid bootTime_oid[] = { 1,3,6,1,4,1,56094,1,1,51 };
		char	firmwareName[21]; 					// const oid firmwareName = { 1,3,6,1,4,1,56094,1,1,53 };
		char	modemTxPower[6];					// const oid modemTxPower_oid[] = { 1,3,6,1,4,1,56094,1,2,1 };
		char	modemRSSI[6];						// const oid modemRSSI_oid[] = { 1,3,6,1,4,1,56094,1,2,2 };
		char	modemTxRSSI[6];						// Non-MIB
		char	modemSINR[6];						// const oid modemSINR_oid[] = { 1,3,6,1,4,1,56094,1,2,3 };
		char	modemPLMN[11];						// const oid modemPLMN_oid[] = { 1,3,6,1,4,1,56094,1,2,4 };
		char	modemRAT[15];						// const oid modemRAT_oid[] = { 1,3,6,1,4,1,56094,1,2,5 };
		char	modemChannelInfo[6];				// const oid modemChannelInfo_oid[] = { 1,3,6,1,4,1,56094,1,2,6 };
		char	modemBlockErrorRate[6]; 			// const oid modemBlockErrorRate_oid[] = { 1,3,6,1,4,1,56094,1,2,7 };
		char	modemPCI[6];						// const oid modemPCI_oid[] = { 1,3,6,1,4,1,56094,1,2,8 };
		char    modemICCID[21];                     // None OID
		char    lte_modem_status;                   // None OID		
		char	modemPSC[6];						// const oid modemPSC_oid[] = { 1,3,6,1,4,1,56094,1,2,9 };
		char	modemRSRP[6];						// const oid modemRSRP_oid[] = { 1,3,6,1,4,1,56094,1,2,10 };
		char	modemRSCP[6];						// const oid modemRSCP_oid[] = { 1,3,6,1,4,1,56094,1,2,11 };
		char	modemRSRQ[6];						// const oid modemRSRQ_oid[] = { 1,3,6,1,4,1,56094,1,2,12 };
		char	modemEcIo[6];						// const oid modemEcIo_oid[] = { 1,3,6,1,4,1,56094,1,2,13 };
		int 	m1700DlGain;						// const oid m1700DlGain_oid[] = { 1,3,6,1,4,1,56094,1,5,1 };
		int 	m1700DlAlcLevel;					// const oid m1700DlAlcLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,2 };
		int 	m1700DlAlcOnOff;					// const oid m1700DlAlcOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,3 };
		int 	m1700IsolationRecheckOnOff; 		// const oid m1700IsolationRecheckOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,4 };
		int 	m1700IsolationValue;				// const oid m1700IsolationValue_oid[] = { 1,3,6,1,4,1,56094,1,5,5 };
		int 	m1700DlInputPower;					// const oid m1700DlInputPower_oid[] = { 1,3,6,1,4,1,56094,1,5,6 };
		int 	m1700DlOutputPower; 				// const oid m1700DlOutputPower_oid[] = { 1,3,6,1,4,1,56094,1,5,7 };
		int 	m1700DlAmpOnOff;					// const oid m1700DlAmpOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,8 };
		int 	m1700DlAsdOnOff;					// const oid m1700DlAsdOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,9 };
		int 	m1700DlAsdLevel;					// const oid m1700DlAsdLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,10 };
		int 	m1700DlUlGainOffsetOnOff;			// const oid m1700DlUlGainOffsetOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,11 };
		int 	m1700DlUlGainOffsetLevel;			// const oid m1700DlUlGainOffsetLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,12 };
		int 	m1700IsolationAlarmState;			// const oid m1700IsolationAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,13 };
		int 	m1700DlShutdownAlarmState;			// const oid m1700DlShutdownAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,14 };
		int 	m1700AmpAlarmState; 				// const oid m1700AmpAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,15 };
		int 	m1700PllLockDetectAlarmState;		// const oid m1700PllLockDetectAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,16 };
		int 	m1700UlGain;						// const oid m1700UlGain_oid[] = { 1,3,6,1,4,1,56094,1,5,17 };
		int 	m1700UlInputPower;					// const oid m1700UlInputPower_oid[] = { 1,3,6,1,4,1,56094,1,5,18 };
		int 	m1700UlOutputPower; 				// const oid m1700UlOutputPower_oid[] = { 1,3,6,1,4,1,56094,1,5,19 };
		int 	m1700UlAmpOnOff;					// const oid m1700UlAmpOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,20 };
		int 	m1700UlAlcLevel;					// const oid m1700UlAlcLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,21 };
		int 	m1700UlAlcOnOff;					// const oid m1700UlAlcOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,22 };
		int 	m1700UlAsdOnOff;					// const oid m1700UlAsdOnOff_oid[] = { 1,3,6,1,4,1,56094,1,5,23 };
		int 	m1700UlAsdLevel;					// const oid m1700UlAsdLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,24 };
		int 	m1700UlShutdownAlarmState;			// const oid m1700UlShutdownAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,25 };
		int 	m1700LowPowerAlarmState;			// const oid m1700LowPowerAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,26 };
		int     m1700DlIsolationValue;				// const oid m1700DlIsolationValue_oid[] = { 1,3,6,1,4,1,56094,1,5,27 };
		int		m1700UlIsolationValue;				// const oid m1700UlIsolationValue_oid[] = { 1,3,6,1,4,1,56094,1,5,28 };
		int		m1700DlInputHighPowerAlarmState;    // const oid m1700DlInputHighPowerAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,29 };
		int 	m1700UlInputHighPowerAlarmState;	// const oid m1700UlInputHighPowerAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,5,30 };
		int		m1700DlInputHighPowerAlarmLevel;    // const oid m1700DlInputHighPowerAlarmLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,31 };		
		int		m1700LowPowerAlarmLevel;       // const oid m1700LowPowerAlarmLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,32 };
		int		m1700UlInputHighPowerAlarmLevel;	// const oid m1700UlInputHighPowerAlarmLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,33 };
		int 	m1700IsolationAlarmLevel;			// const oid m1700IsolationAlarmLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,34 };
		int		m1700UlSleepLevel;					// const oid m1700UlSleepLevel_oid[] = { 1,3,6,1,4,1,56094,1,5,35 };				
		int 	tempHighAlarmMask;					// const oid tempHighAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,1 };
		int 	linkFailAlarmMask;					// const oid linkFailAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,2 };
		int 	m1700IsolationAlarmMask;			// const oid m1700IsolationAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,8 };
		int 	m1700DlShutdownAlarmMask;			// const oid m1700DlShutdownAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,12 };
		int 	m1700UlShutdownAlarmMask;			// const oid m1700UlShutdownAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,16 };
		int 	m1700LowPowerAlarmMask; 			// const oid m1700LowPowerAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,20 };
		int 	m1700PllLockDetectAlarmMask;		// const oid m1700PllLockDetectAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,24 };
		//int 	m1700AmpAlarmMask;					// const oid m1700AmpAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,28};
		int     m1700DlInputHighPowerAlarmMask;		// const oid m1700DlInputHighPowerAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,38};
		int     m1700UlInputHighPowerAlarmMask;		// const oid m1700UlInputHighPowerAlarmMask_oid[] = { 1,3,6,1,4,1,56094,1,9,39};		
		int     m1700InputVoltAlarmMaskNotUsed;		// 		
		char    LocalTime[21];      				// 현재 시간 	
		char    heartbeatTxTime[21];      			// heartbeat 전송 시간 
		char    lastAlarmTime[21];      	        // 마지막 Alarm 발생 시간
		char    lastTxPower[6];                     // 마지막 송신 츌력 
		int		in_svc;								// 망 접속을 하고 서비스 상태 

		
		char	modemIMSI[26];						// const oid modemIMEI_oid[] = { 1,3,6,1,4,1,56094,1,1,13 };
		char	Cell_ID[6];

		int 	tempHighAlarmState_Flag; 				// const oid tempHighAlarmState_oid[] = { 1,3,6,1,4,1,56094,1,1,30 };
		int 	B1_PllAlarmState;
		int 	B1_PllAlarmState_Flag;
		int 	B3_PllAlarmState;
		int 	B3_PllAlarmState_Flag;
		int 	N78_PllAlarmState;
		int 	N78_PllAlarmState_Flag;

		int		B1_DLOutputHighPowerAlarmState;  
		int		B1_DLOutputHighPowerAlarmState_Flag;  
		int		B3_DLOutputHighPowerAlarmState;  
		int		B3_DLOutputHighPowerAlarmState_Flag;  
		int		N78_DLOutputHighPowerAlarmState;  
		int		N78_DLOutputHighPowerAlarmState_Flag;  

		int 	B1_DLOutputLowPowerAlarmState;  
		int 	B1_DLOutputLowPowerAlarmState_Flag;  
		int 	B3_DLOutputLowPowerAlarmState;  
		int 	B3_DLOutputLowPowerAlarmState_Flag;  
		int 	N78_DLOutputLowPowerAlarmState;  
		int 	N78_DLOutputLowPowerAlarmState_Flag;  

		int 	B1_ULOutputHighPowerAlarmState;  
		int 	B1_ULOutputHighPowerAlarmState_Flag;  
		int 	B3_ULOutputHighPowerAlarmState;  
		int 	B3_ULOutputHighPowerAlarmState_Flag;  
		int 	N78_ULOutputHighPowerAlarmState;  
		int 	N78_ULOutputHighPowerAlarmState_Flag;  

		int     B1_IsolationAlarmState;
		int     B1_IsolationAlarmState_Flag;
		int 	B3_IsolationAlarmState;
		int 	B3_IsolationAlarmState_Flag;
		int     N78_IsolationAlarmState;
		int     N78_IsolationAlarmState_Flag;

		int 	TDD_Sync_AlarmAlarmState;
		int 	TDD_Sync_AlarmAlarmState_Flag;


		int 	B1_AmpOnOff_AlarmAlarmState;
		int 	B1_AmpOnOff_AlarmAlarmState_Flag;
		int 	B3_AmpOnOff_AlarmAlarmState;
		int 	B3_AmpOnOff_AlarmAlarmState_Flag;
		int 	N78_AmpOnOff_AlarmAlarmState;
		int 	N78_AmpOnOff_AlarmAlarmState_Flag;


		int 	B1_ULAmpOnOff_AlarmAlarmState;
		int 	B1_ULAmpOnOff_AlarmAlarmState_Flag;
		int 	B3_ULAmpOnOff_AlarmAlarmState;
		int 	B3_ULAmpOnOff_AlarmAlarmState_Flag;
		int 	N78_ULAmpOnOff_AlarmAlarmState;
		int 	N78_ULAmpOnOff_AlarmAlarmState_Flag;


//		int 	tempHighAlarmState;			// 

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} SYS_DATA;

struct NOS_nms_info{
	u32		modemLocalIpAddress;				// 4
	u32		targetLocalIpAddress1;				// 8
	u32		targetLocalIpAddress2;				// 12
	
	char	gwAPNNumber[31];					// 32 
	char	gwUserID[21];						// 52
	char	gwPassword[21]; 					// 72

	//int 	tempHigh;							// 76
	unsigned short	heartbeatTime;				// 76
	int 	heartbeatControl;					// 80 
	int 	autoResetTime;						// 84
	unsigned short	alarmDuration;				// 86

	u32		fwDownloadServerIpAddress;			// 90
	char	fwDownloadFilePath[51]; 			// 140
	char	firmwareName[21]; 					// 160
	char	fwDownloadServerUserId[21]; 		// 180
	char	fwDownloadServerUserPassword[21];	// 200
	int 	tempHighAlarmMask;					// 201
	int 	m1700IsolationAlarmMask;			// 202
	int 	m1700DlShutdownAlarmMask;			// 203
	int 	m1700UlShutdownAlarmMask;			// 204
	int 	m1700LowPowerAlarmMask; 			// 205
	int 	m1700PllLockDetectAlarmMask;		// 206
	int 	m1700DlInputHighPowerAlarmMask; 	// 207
	int 	m1700UlInputHighPowerAlarmMask; 	// 208
};

/*
struct modem_struct{
	// Modem
	signed  char	tx_power;
	signed  char 	SINR;
	char	PLMN[10];
	char	RAT[14];
	u16		channel_info;
	u16		PCI;
	signed  char	RSRP;
	signed  char	RSRQ;
	char			modem_alarm; 		// 2022-05-17
	char			modem_alarm_mask; 	// 2022-05-17	
	char	lte_modem_status;
	char	modem_type[10];
	char	modem_maker[15];
	char	modem_version[5];
	char	modem_MSN_number[20];
	char	modem_IMEI[25];
	char	modem_local_ip_addr[20];
	char 	target_local_ip_addr1[20];
	char	target_local_ip_addr2[20];
	char	GW_phone_number_or_ip_addr[20];
	char	GW_user_id[20];
	char	GW_password[20];
	char	modem_ICCID[20];
	
	// Modem CPU
	char	rpt_model_name[10];
	char	rpt_type[10];
	char	rpt_maker[10];
	u16		heartbeat_time;
	char	heartbeat_ctrl;
	char	auto_reset_time;
	char	rpt_reset;
	char	modem_reset;
	char	fw_download;
	u32		fw_download_result;
	char	fw_download_server_ip_addr[20];
	char	fw_download_file_path[50];
	char	fw_name[20];
	char	fw_download_server_user_id[20];
	char	fw_download_server_password[20];
	char	boot_time[20];
	char	system_time[20];
	signed	char	modem_RSSI;
	char	modem_block_error_rate;
	char	modem_PSC;
	char	modem_RSCP;
	char	modem_EC_Io;
	char	modem_ru_alarm_mask;		
};	

struct modem_info{
	// Modem
	char	modem_IMEI[25];
	
	char 	target_local_ip_addr1[20];
	char	target_local_ip_addr2[20];
	char	GW_phone_number_or_ip_addr[20];
	char	GW_user_id[20];
	char	GW_password[20];
	
	// Modem CPU
	u16		heartbeat_time;
	char	heartbeat_ctrl;
	char	auto_reset_time;
	char	rpt_reset;
	char	modem_reset;
	char	fw_download;

	char	fw_download_server_ip_addr[20];
	char	fw_download_file_path[20];
	char	sau_fw_name[20];
	char	dau_fw_name[20];
	char	mu_fw_name[20];
	char	fw_download_server_user_id[20];
	char	fw_download_server_password[20];

	char	modem_ru_alarm_mask;	
};
*/

