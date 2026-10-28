#ifndef __REPEATER_H__
#define __REPEATER_H__

#include <stdint.h>
#include <stddefs.h>

#define FIRMWARE_MAIN_VERSION	0x00
#define FIRMWARE_SUB_VERSION	0x02	/// 20261014


#define FPGA_MAIN_VERSION	0x00
#define FPGA_SUB_VERSION	0x03 /// 20261014
#define FPGA_VERSION_REG    0x04


#define MAX_BAND				4

//#define BAND0					0
//#define BAND1					1
//#define BAND2					2
//#define BAND3					3


#define MAX_SB					4

#define DL						0
#define UL						1


typedef unsigned char		BOOLEAN;
typedef unsigned char		INT8U;						// Unsigned  8 bit quantity
typedef signed   char		INT8S;						// Signed    8 bit quantity
typedef unsigned short int	INT16U;						// Unsigned 16 bit quantity
typedef signed short int	INT16S;						// Signed   16 bit quantity
typedef unsigned int		INT32U;						// Unsigned 32 bit quantity
typedef signed   int		INT32S;						// Signed   32 bit quantity

#define 					BR_NUM			4 				// 4 branches 
#define 					RU_BR_NUM		4 				// max 4 RU each branch

struct repeater_info {
	int16_t				tr_pwm;
	int16_t				ir_pwm_br[MAX_BAND];
};

struct repeater_adc_struct {
	unsigned short DL_DET_High_Band;
	unsigned short DL_DET_Low_Band;
	unsigned short UL_DET_High_Band;
	unsigned short UL_DET_Low_Band;
	unsigned short DC_DET;
};
struct repeater_table_struct {
	unsigned char  Table_Types;
	unsigned char  Table_Size;

	unsigned short  Actual_Table_Value[2500];

};
struct repeater_table_query_struct {
	unsigned char Current_Band_in_Use;
	unsigned char type;
};
struct repeater_table_control_struct {

	unsigned char Current_Band_in_Use;
	unsigned char  control_Varibale_Mask;
	unsigned char Table_types;
	unsigned char Table_Size;
	unsigned short Table_Value[2500];
};

struct repeater_ags_query_struct {
	unsigned char		AGS_Band_Current;
};
struct repeater_ags_struct {
	unsigned char 		Progress;
	unsigned char 		isAgs;
	unsigned short		Dau1;
	unsigned short 		Dau2;
	unsigned short		Dau3;
	unsigned int 		Br1;
	unsigned int 		Br2;
	unsigned int 		Br3;
	unsigned int 		Br4;
	unsigned short 		Low_Band_Att;
	unsigned short 		High_Band_Att;
	unsigned short		LB_DL_Att_Pwr;
	unsigned short		HB_DL_Att_Pwr;
	
};

struct  msg_install {
    char dau[3];				/* 0 : MNO0,1 : MNO1, 2 : MNO2 */
	char sau_pri;
	char sau_pri_mu;
	char sau_pri_ru[BR_NUM][RU_BR_NUM];

	char sau_s1;
	char sau_s1_mu;
	char sau_s1_ru[BR_NUM][RU_BR_NUM];

	char sau_s2;
	char sau_s2_mu;
	char sau_s2_ru[BR_NUM][RU_BR_NUM];

	char sau_s3;
	char sau_s3_mu;
	char sau_s3_ru[BR_NUM][RU_BR_NUM];

	char sau_s4;
	char sau_s4_mu;
	char sau_s4_ru[BR_NUM][RU_BR_NUM];


//    char    			sau_tree[5];				/* 0: Pri,1:Sec 0,2:Sec 1,3:Sec 2,4:Sec 3 */
//    char    			mu_tree[5];					/* 0: Pri,1:Sec 0,2:Sec 1,3:Sec 2,4:Sec 3 */
//    char    			ru[5][16];					/* 93Byte */
};


///// Tree Info
struct tree_Info
{
	unsigned char dau0;
	unsigned char dau1;
	unsigned char dau2;
	unsigned char sau_pri;
	unsigned char sau_pri_mu;
	unsigned char sau_pri_ru[16];
	unsigned char sau_sec0;
	unsigned char sau_sec0_mu;
	unsigned char sau_sec0_ru[16];
	unsigned char sau_sec1;
	unsigned char sau_sec1_mu;
	unsigned char sau_sec1_ru[16];
	unsigned char sau_sec2;
	unsigned char sau_sec2_mu;
	unsigned char sau_sec2_ru[16];
	unsigned char sau_sec3;
	unsigned char sau_sec3_mu;
	unsigned char sau_sec3_ru[16];

};
////////




struct repeater_mu {
	char				repeater_model_name[10];
	char				repeater_type[10];
	char				repeater_maker[10];
	short				repeater_ID;
	char				MU_ID;
	short				boot_initilization;
	bool				is_booted;
	bool				Topology_Set;
	bool				AGS_onoff;
	bool				ISO_onoff;	
	unsigned char		repeater_sn[20];
	unsigned short		firmware_version;
	unsigned short		fpga_version;							//0x01 : ver 0.1

	short				temperature;  
	// 0: BR1 (0-3bit: ru1-ru4)
	// 1: BR2 (0-3bit: ru5-ru8)
	// 2, BR3 (0-3bit: ru9-ru12) 
	// 3: BR4 (0-3bit: ru13-ru16)
	short				ru_con_status[BR_NUM]; 
	short				dc_volt;
	bool				br_onoff[BR_NUM];		 // BRx_OnOff
	short				ru_nu;					 // Number_of_RUs
	short				ru_br_nu[BR_NUM];		 // Number_of_BRx
	short				ru_alarm_nu;			 // Number_of_RU_Alarm
	/* 9.High Temperature Alarm State,(R/A)*/
	unsigned char		temperatureAlarm;
	bool				br_pwr_alarm[BR_NUM];	 // BRx_PWR_Alarm
//	bool				ru_link_fail[BR_NUM * RU_BR_NUM];			 // RUx_x_LINK_Fail
	bool				ru_link_fail[BR_NUM][RU_BR_NUM];			// RUx_x_LINK_Fail
	short				RU_Iso[16];
	char				band_in_use[MAX_BAND];
	int16_t 			temp_high;
	short				temp_offset;

	/* .Alarm Duration,(R/W)*/
	unsigned short		alarmDuration;
	int16_t 			tr_pwm;
	short				tr_sel;
	int16_t 			ir_pwm_br[MAX_BAND];
	short				ir_pwm_sel[MAX_BAND];
	int16_t 			tr_txcount;
	int16_t 			tr_rxcount;
	short				ir_br_txcount[MAX_BAND];
	short				ir_br_rxcount[MAX_BAND];
	char				psu_status;
	short				led_status[20];
	short				led_test;
	char				reserved[20];
};

struct repeater_ru {
	char				repeater_model_name[10];
	char				repeater_type[10];
	char				repeater_maker[10];
	short				repeater_ID;
	char				RU_ID;
	short				boot_initilization;
	bool				is_booted;
	bool				Topology_Set;
	bool				AGS_onoff;
	bool				ISO_onoff;	
	unsigned char		repeater_sn[20];
	unsigned short		firmware_version;
	unsigned short		fpga_version;							//0x01 : ver 0.1
	short				temperature;  
	short				dl_output_pwr_700M[MAX_SB];
	short				dl_output_pwr_800M[MAX_SB];
	short				dl_output_pwr_1700M[MAX_SB];
	short				dl_output_pwr_2100M[MAX_SB];
	short				RU_Iso[MAX_SB];
	unsigned char 		dl_amp[MAX_BAND];
	unsigned char 		ul_amp[MAX_BAND];
	unsigned char		AGS_Status;
	short				AGS_Level_Lowband;
	short				AGS_Level_Highband;
	short				DL_Output_Power_Lowband;
	short				DL_Output_Power_Highband;
	short				DL_Output_Power_Lowband_Offset;
	short				DL_Output_Power_Highband_Offset;
	short				AGS_Level_ATT_Lowband;
	short				AGS_Level_ATT_Highband;
	short				Distance;
	short				DL_ATT_Gain_Offset_Lowband;
	short				DL_ATT_Gain_Offset_Highband;
	short				DL_ADC_Lowband;
	short				DL_ADC_Highband;
	short				DC_DET_ADC;
	unsigned char		IR_LINK_Fail;
	unsigned char		High_Temperature_Alarm;
	short				Temp_High;
	short				Temp_Offset;
	unsigned char		BAND_Use[MAX_BAND];
	short				Alarm_Duration;
	int16_t 			tr_pwm;
	unsigned char		tr_sel;
	int16_t 			ir_pwm;
	unsigned char		ir_sel;
	unsigned char		LED_Status;
	unsigned char		LED_Test;

	short				TR_TxCount;
	short				TR_RxCount;
	short				IR_TxCount;
	short				IR_RxCount;
	char				reserved[12];
};


struct repeater_dau {
	char				repeater_model_name[10];
	char				repeater_type[10];
	char				repeater_maker[10];
	short				repeater_ID;
	char				RU_ID;
	short				boot_initilization;
	bool				is_booted;
//	bool				Topology_Set;
//	bool				AGS_onoff;
//	bool				ISO_onoff;	
	unsigned char		repeater_sn[20];
	unsigned short		firmware_version;
	unsigned short		fpga_version;							//0x01 : ver 0.1
	short				temperature;  
	short				dl_Input_pwr[MAX_SB];  
	short				DL_ALC_Level[MAX_SB];  
	short				DL_ALC_Offset[MAX_SB];  
	unsigned char		DL_ALC_OnOff[MAX_SB];  
	short				UL_Output_power[MAX_SB];  
	unsigned char		UL_AGC_OnOff[MAX_SB];  
	short				UL_AGC_Offset[MAX_SB];  
	unsigned char 		dl_amp[MAX_BAND];
	unsigned char 		ul_amp[MAX_BAND];
	unsigned short		dl_Att[MAX_BAND];							//0x01 : ver 0.1
	unsigned short		ul_Att[MAX_BAND+1];							//0x01 : ver 0.1
	unsigned short		dl_Att_Offset[MAX_BAND];							//0x01 : ver 0.1
	unsigned short		ul_Att_Offset[MAX_BAND+1];							//0x01 : ver 0.1
	short				AGS_Level_ATT_Lowband;
	short				AGS_Level_ATT_Highband;
	short				UL_Output_Power_Lowband;
	short				UL_Output_Power_Highband;
	short				UL_800M_ADC;
	short				UL_2100M_ADC;
	short				DC_DET_ADC;
	short				Temp_High;
	short				Temp_Offset;
	unsigned char		High_Temperature_Alarm;
	unsigned char		BAND_Use[MAX_BAND];
	short		dl_Temp_Att[MAX_BAND];							//0x01 : ver 0.1
	short		ul_Temp_Att[MAX_BAND+1];							//0x01 : ver 0.1
	short				Alarm_Duration;
	int16_t 			ir_pwm;
	short				ir_sel;
	short				IR_TxCount;
	short				IR_RxCount;
	unsigned char		LED_Status[8];
	unsigned char		LED_Test;
	char				reserved[8];
};


struct repeater_struct {

	struct repeater_mu Mu;
	struct msg_install Tree;

	/* System common */

	char        		repeater_model_name[10];
	char  				repeater_type[10];
	char  				repeater_maker[10];
	short 				repeater_ID;
	char				MU_ID;
	short 				boot_initilization;
	bool 				is_booted;
	bool 				Topology_Set;
	bool 				AGS_onoff;
	bool 				ISO_onoff;	
	unsigned char		repeater_sn[20];
	unsigned short		firmware_version;
	unsigned short  	fpga_version;                    		//0x01 : ver 0.1

	short				temperature;  
	// 0: BR1 (0-3bit: ru1-ru4)
	// 1: BR2 (0-3bit: ru5-ru8)
	// 2, BR3 (0-3bit: ru9-ru12) 
	// 3: BR4 (0-3bit: ru13-ru16)
	short				ru_con_status[BR_NUM]; 
	short 				dc_volt;
	bool				br_onoff[BR_NUM];        // BRx_OnOff
	short 				ru_nu;                   // Number_of_RUs
	short 				ru_br_nu[BR_NUM];        // Number_of_BRx
	short 				ru_alarm_nu;             // Number_of_RU_Alarm
	/* 9.High Temperature Alarm State,(R/A)*/
	unsigned char 		temperatureAlarm;
	bool				br_pwr_alarm[BR_NUM];    // BRx_PWR_Alarm
//	bool				ru_link_fail[BR_NUM * RU_BR_NUM];            // RUx_x_LINK_Fail
	bool				ru_link_fail[BR_NUM][RU_BR_NUM];            // RUx_x_LINK_Fail
	short				RU_Iso[16];
	char				band_in_use[MAX_BAND];
	int16_t				temp_high;
	short				temp_offset;

	/* .Alarm Duration,(R/W)*/
	unsigned short 		alarmDuration;
	int16_t				tr_pwm;
	short				tr_sel;
	int16_t				ir_pwm_br[MAX_BAND];
	short				ir_pwm_sel[MAX_BAND];
	int16_t				tr_txcount;
	int16_t				tr_rxcount;
	short				ir_br_txcount[MAX_BAND];
	short				ir_br_rxcount[MAX_BAND];
	char 				psu_status;
	short				led_status[20];
	short				led_test;
	char 				reserved[20];
	//bool				all_br_call_onoff;       // All_BR_Cal_OnOff
	//short				psu_status;
	
	// short				temp_offset;
	
	unsigned char tree_status[93];

	unsigned long       realFileDataSize;
	unsigned char pl_mode;
	unsigned char app_mode;
	unsigned short crc_gen;
	char		   filetype;
///////////////////////////////////////////////
	char				boot_init;	

	char				ru_insert[16];
	u16 				ru_tx_cnt[16];
	u16 				ru_rx_cnt[16];
	u16 				ru_link_cnt[16];
//////////////////////////////////////////////
	
	struct repeater_ru Ru[BR_NUM][RU_BR_NUM];
	unsigned char ID_port;
	unsigned char RU_id;
	unsigned char br_insert[4];

	char				ru_id;
	char				tr_ru_id;
	char				ir_ru_id;

	char				tr_buff[4];
	char				ir_buff[4];
	char				children_ru_number[4];
	char				ir_children_ru_number;
	char				curr_ir_children_ru_number;
	char				TOPOLOGY_cnt;

		
	u8					lmt_bypass;
	u16					ir_port_cnt;

};

// ----- end of repeater_struct
struct main_ctrl_mu_struct{
//	unsigned char 			mask[6];
//	unsigned char 			Reserved1;
	bool 					boot_initilization;
	bool 					Topology_Set;
	bool 					AGS_onoff;
	bool 					ISO_onoff;
	unsigned char			repeater_sn[20];
	bool 					br_onoff[BR_NUM];
	bool					band_in_use[MAX_BAND];
	int16_t					temp_high;
	short					temp_offset;
	unsigned short 			alarmDuration;
	int16_t					tr_pwm;
	short					tr_sel;
	int16_t				ir_pwm_br[MAX_BAND];
	short				ir_pwm_sel[MAX_BAND];
	short				led_test;
	char 				Reserved2[28];
};

struct main_sau_struct {
	/* --- Header --- */
	char        		repeater_model_name[10];
	char  				repeater_type[10];
	char  				repeater_maker[10];
	short 				repeater_ID;
	char				MU_ID;
	short 				boot_initilization;
	bool 				is_booted;
	bool 				Topology_Set;
	bool 				AGS_onoff;
	bool 				ISO_onoff;	
	unsigned char 		Topology_Progress;
	unsigned char		AGS_Progress;
	unsigned char		ISO_Progress;	
	bool 				RU_Install;
	bool 				PVT_OnOff;

	unsigned char		repeater_sn[20];
	unsigned short		firmware_version;
	unsigned short  	fpga_version;                    	
	unsigned int  		MNO_insert[4];
	unsigned int 		MNO_type[4];
	unsigned char 		MNO_EN[4];

	/* ================= 700M BAND ================= */
	unsigned char 		Type_700M[4];				// 4 Bytes
	unsigned char 		BW_700M[4];					// 4 Bytes
	short 				DL_Input_power_700M[4];		// 8 Bytes (4 x 2)
	short 				DL_Output_power_700M[4];	// 8 Bytes
	short 				DL_Gain_700M[4];			// 8 Bytes
	short 				DL_ATT1_700M[4];			// 8 Bytes
	short 				DL_ATT2_700M[4];			// 8 Bytes
	short 				DL_ALC_Level_700M[4];		// 8 Bytes
	short 				DL_ALC_Offset_700M[4];		// 8 Bytes
	unsigned char 		DL_ALC_OnOff_700M;			// 1 Byte
	unsigned char 		DL_Path_OnOff_700M[4];		// 4 Bytes
	unsigned char 		DL_Amp_OnOff_700M[4];		// 4 Bytes
	short 				DL_Isolation_700M[4];		// 8 Bytes
	unsigned char 		UL_AGC_OnOff_700M[4];		// 4 Bytes
	short 				UL_AGC_Offset_700M[4];		// 8 Bytes
	short 				UL_Input_power_700M[4];		// 8 Bytes
	short 				UL_Output_power_700M[4];	// 8 Bytes
	short 				UL_Gain_700M[4];			// 8 Bytes
	short 				UL_ATT_700M[4];				// 8 Bytes
	short 				UL_ALC_Level_700M[4];		// 8 Bytes
	short 				UL_ALC_Offset_700M[4];		// 8 Bytes
	unsigned char 		UL_ALC_OnOff_700M;			// 1 Byte
	unsigned char 		UL_Pam_OnOff_700M[4];		// 4 Bytes
	unsigned char 		UL_Amp_OnOff_700M[4];		// 4 Bytes
	short 				UL_Sleep_Level_700M;		// 2 Bytes
	unsigned char 		UL_Sleep_OnOff_700M;		// 1 Byte
	unsigned char 		Sleep_Mode_Status_700M;		// 1 Byte
	unsigned char 		Shutdown_OnOff_700M;		// 1 Byte

	/* ================= 800M BAND ================= */
	unsigned char 		Type_800M[4];				// 4 Bytes
	unsigned char 		BW_800M[4];					// 4 Bytes
	short 				DL_Input_power_800M[4];		// 8 Bytes
	short 				DL_Output_power_800M[4];	// 8 Bytes
	short 				DL_Gain_800M[4];			// 8 Bytes
	short 				DL_ATT1_800M[4];			// 8 Bytes
	short 				DL_ATT2_800M[4];			// 8 Bytes
	short 				DL_ALC_Level_800M[4];		// 8 Bytes
	short 				DL_ALC_Offset_800M[4];		// 8 Bytes
	unsigned char 		DL_ALC_OnOff_800M;			// 1 Byte
	unsigned char 		DL_Path_OnOff_800M[4];		// 4 Bytes
	unsigned char 		DL_Amp_OnOff_800M[4];		// 4 Bytes
	short 				DL_Isolation_800M[4];		// 8 Bytes
	unsigned char 		UL_AGC_OnOff_800M[4];		// 4 Bytes
	short 				UL_AGC_Offset_800M[4];		// 8 Bytes
	short 				UL_Input_power_800M[4];		// 8 Bytes
	short 				UL_Output_power_800M[4];	// 8 Bytes
	short 				UL_Gain_800M[4];			// 8 Bytes
	short 				UL_ATT_800M[4];				// 8 Bytes
	short 				UL_ALC_Level_800M[4];		// 8 Bytes
	short 				UL_ALC_Offset_800M[4];		// 8 Bytes
	unsigned char 		UL_ALC_OnOff_800M;			// 1 Byte
	unsigned char 		UL_Pam_OnOff_800M[4];		// 4 Bytes
	unsigned char 		UL_Amp_OnOff_800M[4];		// 4 Bytes
	short 				UL_Sleep_Level_800M;		// 2 Bytes
	unsigned char 		UL_Sleep_OnOff_800M;		// 1 Byte
	unsigned char 		Sleep_Mode_Status_800M;		// 1 Byte
	unsigned char 		Shutdown_OnOff_800M;		// 1 Byte

	/* ================= 1900M BAND ================= */
	unsigned char 		Type_1900M[4];				// 4 Bytes
	unsigned char 		BW_1900M[4];				// 4 Bytes
	short 				DL_Input_power_1900M[4];	// 8 Bytes
	short 				DL_Output_power_1900M[4];	// 8 Bytes
	short 				DL_Gain_1900M[4];			// 8 Bytes
	short 				DL_ATT1_1900M[4];			// 8 Bytes
	short 				DL_ATT2_1900M[4];			// 8 Bytes
	short 				DL_ALC_Level_1900M[4];		// 8 Bytes
	short 				DL_ALC_Offset_1900M[4];		// 8 Bytes
	unsigned char 		DL_ALC_OnOff_1900M;			// 1 Byte
	unsigned char 		DL_Path_OnOff_1900M[4];		// 4 Bytes
	unsigned char 		DL_Amp_OnOff_1900M[4];		// 4 Bytes
	short 				DL_Isolation_1900M[4];		// 8 Bytes
	unsigned char 		UL_AGC_OnOff_1900M[4];		// 4 Bytes
	short 				UL_AGC_Offset_1900M[4];		// 8 Bytes
	short 				UL_Input_power_1900M[4];	// 8 Bytes
	short 				UL_Output_power_1900M[4];	// 8 Bytes
	short 				UL_Gain_1900M[4];			// 8 Bytes
	short 				UL_ATT_1900M[4];			// 8 Bytes
	short 				UL_ALC_Level_1900M[4];		// 8 Bytes
	short 				UL_ALC_Offset_1900M[4];		// 8 Bytes
	unsigned char 		UL_ALC_OnOff_1900M;			// 1 Byte
	unsigned char 		UL_Pam_OnOff_1900M[4];		// 4 Bytes
	unsigned char 		UL_Amp_OnOff_1900M[4];		// 4 Bytes
	short 				UL_Sleep_Level_1900M;		// 2 Bytes
	unsigned char 		UL_Sleep_OnOff_1900M;		// 1 Byte
	unsigned char 		Sleep_Mode_Status_1900M;	// 1 Byte
	unsigned char 		Shutdown_OnOff_1900M;		// 1 Byte

	/* ================= 2100M BAND ================= */
	unsigned char 		Type_2100M[4];				// 4 Bytes
	unsigned char 		BW_2100M[4];				// 4 Bytes
	short 				DL_Input_power_2100M[4];	// 8 Bytes
	short 				DL_Output_power_2100M[4];	// 8 Bytes
	short 				DL_Gain_2100M[4];			// 8 Bytes
	short 				DL_ATT1_2100M[4];			// 8 Bytes
	short 				DL_ATT2_2100M[4];			// 8 Bytes
	short 				DL_ALC_Level_2100M[4];		// 8 Bytes
	short 				DL_ALC_Offset_2100M[4];		// 8 Bytes
	unsigned char 		DL_ALC_OnOff_2100M;			// 1 Byte
	unsigned char 		DL_Path_OnOff_2100M[4];		// 4 Bytes
	unsigned char 		DL_Amp_OnOff_2100M[4];		// 4 Bytes
	short 				DL_Isolation_2100M[4];		// 8 Bytes
	unsigned char 		UL_AGC_OnOff_2100M[4];		// 4 Bytes
	short 				UL_AGC_Offset_2100M[4];		// 8 Bytes
	short 				UL_Input_power_2100M[4];	// 8 Bytes
	short 				UL_Output_power_2100M[4];	// 8 Bytes
	short 				UL_Gain_2100M[4];			// 8 Bytes
	short 				UL_ATT_2100M[4];			// 8 Bytes
	short 				UL_ALC_Level_2100M[4];		// 8 Bytes
	short 				UL_ALC_Offset_2100M[4];		// 8 Bytes
	unsigned char 		UL_ALC_OnOff_2100M;			// 1 Byte
	unsigned char 		UL_Pam_OnOff_2100M[4];		// 4 Bytes
	unsigned char 		UL_Amp_OnOff_2100M[4];		// 4 Bytes
	short 				UL_Sleep_Level_2100M;		// 2 Bytes
	unsigned char 		UL_Sleep_OnOff_2100M;		// 1 Byte
	unsigned char 		Sleep_Mode_Status_2100M;	// 1 Byte
	unsigned char 		Shutdown_OnOff_2100M;		// 1 Byte

	/* ================================== */
	unsigned char 		Band_SCAN_OnOff;			// 1 Byte
	unsigned char 		CW_OnOff;					// 1 Byte
	short 				Temp_High;					// 2 Bytes
	short 				DL_Input_Low_Level[4];		// 8 Bytes
	short 				DL_Input_Over_Level[4];		// 8 Bytes
	short 				DL_Output_Over_SD[4];		// 8 Bytes
	short 				Low_Isolation_Level[4];		// 8 Bytes
	short 				Isolation_SD_Level[4];		// 8 Bytes
	short 				UL_Input_Over_Level[4];		// 8 Bytes
	short 				UL_Output_Over_SD_Level[4];	// 8 Bytes
	short 				RU_Isolation_Value[16];		// 32 Bytes
	unsigned char 		SFP_Insert[4];				// 4 Bytes
	short 				PD_Power[4];				// 8 Bytes
	short 				LD_Power[4];				// 8 Bytes
	unsigned char 		Total_Alarm;				// 1 Byte
	unsigned char 		Band_Use_700M;				// 1 Byte
	unsigned char 		Band_Use_800M;				// 1 Byte
	unsigned char 		Band_Use_1700M;				// 1 Byte
	unsigned char 		Band_Use_2100M;				// 1 Byte
	unsigned char 		External_Alarm_Type[5];		// 5 Bytes (Ext 1 ~ 5)

	/* ================= SITE INFO ================= */
	char 				Site_Name[30];				// 30 Bytes
	char 				Address[30];				// 30 Bytes
	char 				Latitude[30];				// 30 Bytes
	char 				Longitude[30];				// 30 Bytes
	char 				Install_Company[30];		// 30 Bytes
	char 				Installer_Name[30];			// 30 Bytes
	char 				Phone[30];					// 30 Bytes
	char 				Email[30];					// 30 Bytes
	char 				Reserved1[10];				// 10 Bytes

	/* ================= CONTROL & PWM / LED ================= */
	unsigned short 		Alarm_Duration;				// 2 Bytes
	short 				Temp_Offset;				// 2 Bytes
	unsigned short 		VC_PWM;						// 2 Bytes
	unsigned short 		MNO0_PWM;					// 2 Bytes
	unsigned char 		MNO0_SEL;					// 1 Byte
	unsigned short 		MNO1_PWM;					// 2 Bytes
	unsigned char 		MNO1_SEL;					// 1 Byte
	unsigned short 		MNO2_PWM;					// 2 Bytes
	unsigned char 		MNO2_SEL;					// 1 Byte
	unsigned short 		IR_PWM;						// 2 Bytes
	unsigned char 		IR_SEL;						// 1 Byte
	unsigned char 		LED_Status[7];				// 7 Bytes
	unsigned char 		LED_Test;					// 1 Byte
	char 				Reserved2[26];				// 26 Bytes
};
struct main_ru_struct {
	/* --- Header  --- */
	char        		repeater_model_name[10];	// 10 Bytes
	char  				repeater_type[10];			// 10 Bytes
	char  				repeater_maker[10];			// 10 Bytes
	short 				repeater_ID;				// 2 Bytes
	unsigned char		RU_ID;						// 1 Byte
	unsigned char 		boot_initialization;		// 1 Byte
	bool 				is_booted;					// 1 Byte
	bool 				topology_set;				// 1 Byte
	bool 				AGS_onoff;					// 1 Byte
	bool 				ISO_onoff;					// 1 Byte
	unsigned char		repeater_sn[20];			// 20 Bytes
	unsigned short		firmware_version;			// 2 Bytes
	unsigned short  	fpga_version;				// 2 Bytes
	short				temperature;				// 2 Bytes
	/* --- DL Output Power & Isolation --- */
	short 				DL_Output_Power_700M[4];	// 8 Bytes (4 x 2)
	short 				DL_Output_Power_800M[4];	// 8 Bytes
	short 				DL_Output_Power_1700M[4];	// 8 Bytes
	short 				DL_Output_Power_2100M[4];	// 8 Bytes
	short 				ISO_Value[4];				// 8 Bytes
	/* --- DL / UL Amp On/Off --- */
	unsigned char 		DL_Amp_OnOff_700M;			// 1 Byte
	unsigned char 		DL_Amp_OnOff_800M;			// 1 Byte
	unsigned char 		DL_Amp_OnOff_1700M;			// 1 Byte
	unsigned char 		DL_Amp_OnOff_2100M;			// 1 Byte
	unsigned char 		UL_Amp_OnOff_700M;			// 1 Byte
	unsigned char 		UL_Amp_OnOff_800M;			// 1 Byte
	unsigned char 		UL_Amp_OnOff_1700M;			// 1 Byte
	unsigned char 		UL_Amp_OnOff_2100M;			// 1 Byte
	/* --- AGS &  Output Power --- */
	unsigned char 		AGS_Status;					// 1 Byte
	short 				AGS_Level_Lowband;			// 2 Bytes
	short 				AGS_Level_Highband;			// 2 Bytes
	short 				DL_Output_Power_Lowband;	// 2 Bytes
	short 				DL_Output_Power_Highband;	// 2 Bytes
	short 				DL_Output_Power_Offset_Lowband;	 // 2 Bytes
	short 				DL_Output_Power_Offset_Highband; // 2 Bytes
	short 				DL_AGS_ATT_Lowband;			// 2 Bytes
	short 				DL_AGS_ATT_Highband;		// 2 Bytes
	unsigned char 		Distance;					// 1 Byte
	short 				DL_ATT_Gain_Offset_Lowband;	// 2 Bytes
	short 				UL_ATT_Gain_Offset_Highband;// 2 Bytes
	/* --- ADC  --- */
	short 				DL_ADC_Lowband;				// 2 Bytes
	short 				DL_ADC_Highband;			// 2 Bytes
	short 				DC_DET_ADC;					// 2 Bytes
	unsigned char 		IR_LINK_Fail;				// 1 Byte
	unsigned char 		High_Temperature_Alarm;		// 1 Byte
	short 				Temp_High;					// 2 Bytes
	short 				Temp_Offset;				// 2 Bytes
	unsigned char 		BAND_Use_700M;				// 1 Byte
	unsigned char 		BAND_Use_800M;				// 1 Byte
	unsigned char 		BAND_Use_1700M;				// 1 Byte
	unsigned char 		BAND_Use_2100M;				// 1 Byte
	unsigned short 		Alarm_Duration;				// 2 Bytes
	unsigned short 		TR_PWM;						// 2 Bytes
	unsigned char 		TR_SEL;						// 1 Byte
	unsigned short 		IR_PWM;						// 2 Bytes
	unsigned char 		IR_SEL;						// 1 Byte
	unsigned char 		LED_Status;					// 1 Byte
	unsigned char 		LED_Test;					// 1 Byte
	unsigned short 		TR_TxCount;					// 2 Bytes
	unsigned short 		TR_RxCount;					// 2 Bytes
	unsigned short 		IR_TxCount;					// 2 Bytes
	unsigned short 		IR_RxCount;					// 2 Bytes
	char 				Reserved[12];				// 12 Bytes
};
struct ru_manager_struct {
	bool 					ru_connected[16];
	struct main_ru_struct 	ru_list[16];
};
struct fpga_desc {
    char    		name[32];    		/* Bank Name                        	*/
    unsigned int 	start;         		/* Start Address of this fpga bank		*/
    unsigned int	end;         		/* End Address of this fpga bank		*/
    unsigned int	size;        		/* Size of Configuration Image			*/
};



#endif
