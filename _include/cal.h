#ifndef _cal_h
#define _cal_h

//#include "stddefs.h"   

//#define MAX_BAND				2
#define BAND0					0
#define BAND1					1
#define BAND2					2
#define BAND3					3

#define DL						0
#define UL						1

#define CAL_DELAY				0x08
#define CAL_ALC_GAIN			0x34
                            	
#define ICS_ON      			0x00010000
#define AGC_ON      			0x00020000
#define ALC_ON      			0x00040000
#define DAC_ON      			0x00080000

#define ICS_VERSION				0x00

#define CAL_CONFIG0				0x00
#define CAL_CONFIG1				0x04
#define CAL_AGC_SETP			0x18

#define CAL_LMS_NOW				0x1C
#define CAL_LMS_STEP			0x38

#define CAL_DELAY1				0x08
#define CAL_DELAY2				0x08

#define CAL_DDC_PWR				0x00
#define CAL_LMS_PWR				0x08
#define CAL_MAN_PWR				0x10
#define CAL_REF_PWR				0x18		//#define LTE_DUC_PWR				0x18

#define CAL_MGC_GAIN			0x28		//	Main Path의 AGC 이득, Main Path란 PN_CODE로 부터 직접 들어 오는 신호다. 
#define CAL_RGC_GAIN			0x28		//	Ref Path의 AGC 이득, Ref Path란 ADC로 부터 들어 오는 신호다. 

#define CAL_MAN_GAIN			0x2C		//	LMS Block의 Man. Error=Man-Ref*hn. 
#define CAL_REF_GAIN			0x2C		//	LMS Block의 Ref. Congugate=Man^. 

//struct band_info {
//	int 			index;
//	char			*band;
//	char			type;
//	char			*desc;
//	unsigned short	firsel;
//	unsigned char	fa;
//	unsigned char	nco;
//};

struct cal_file {
	unsigned char	type;
	unsigned char	nco;
	unsigned char	size;
	unsigned char	rsvd;
	short			hi[2][16];  ///DL ,UL
	short			hq[2][16];
};

struct tbl_cal_info_struct{
	struct cal_file tbl_ul_cal_struct_B28;
//	struct cal_file tbl_dl_cal_struct_B3;
//	struct cal_file tbl_ul_cal_struct_B3;
//	struct cal_file tbl_dl_cal_struct_B28;
//	struct cal_file tbl_ul_cal_struct_B28;
};



struct cal_struct {
	/* Public Parameter */
	//short				iso[2];
	//short				papr[2];

	/* 저장해야 할 파라미터 */
	unsigned char		agc_onoff[2];
		            	
    short	    		att_gain[2];
    short	    		alc_setp[2];
    short	    		man_setp[2];
    //short	    		ref_setp[2];
    short	    		lim_setp[2];
    short	    		usr_gain[2];
                    	
    short	    		delay[4];
    short	    		delay1;
    short	    		delay2;
                    	
	short				lms_tget_run[2];
	short				lms_tget_def[2];
	short				lms_step_run[2];
	short				lms_step_def[2];

	/* Public Parameter */
    unsigned int    	cal_reg_base[2];
    unsigned int    	cal_reg_size;
                    	
    unsigned int    	cal_pwr_base[2];
    unsigned int    	cal_pwr_size;
                    	
    unsigned int    	cal_coe_base[2]; 
    unsigned int    	cal_coe_size;

    unsigned int    	psa_reg_base;
	unsigned int    	psa_reg_size;
	//struct band_info 	band_info;
};

extern struct cal_struct *getCalDev(int band);
//extern short cal_power(int val);
//extern char *showfloat(char *buf, short val);

extern void setCalOnOff(struct cal_struct *pdev, int path, int onoff);
extern void setCalAgcOnOff(struct cal_struct *pdev, int path, int onoff);
extern void setCalAgcSetp(struct cal_struct *pdev, int path, int set);
extern int getCalAgcSetp(struct cal_struct *pdev, int path);
extern int getCalDdcPwr(struct cal_struct *pdev, int path);
extern int getCalLmsPwr(struct cal_struct *pdev, int path);
extern int getCalManPwr(struct cal_struct *pdev, int path);
extern int getCalRefPwr(struct cal_struct *pdev, int path);
extern void setCalDelay1(struct cal_struct *pdev, int delay);
extern int getCalDelay1(struct cal_struct *pdev);
extern void setCalDelay2(struct cal_struct *pdev, int delay);
extern int getCalDelay2(struct cal_struct *pdev);
extern int getCalErrGain(struct cal_struct *pdev, int path);
extern int getCalMgcGain(struct cal_struct *pdev, int path);
extern int getCalRgcGain(struct cal_struct *pdev, int path);
extern int loadCalCoeDef(struct ics_struct *pdev);
extern int loadCalCoe(struct cal_struct *pdev);
extern void showCalCoe(struct cal_struct *pdev);
extern int getCalCoe(struct cal_struct *pdev, struct cal_file *pfile);

#endif
