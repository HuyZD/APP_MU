#ifndef _ics_h
#define _ics_h
#include "stddefs.h"   
/* ics register map */
//#define ICS_REG_BASE			0x8000
//#define ICS_REG_SIZE			128
#define NUMB_FA					4

#define AGC_SETP				0x14

#define TMP_ATT					0x36
#define AGC_GAIN				0x38

#define ICS_ALC_SETP			0x20
#define FA1_ALC_GAIN			0x28
#define FA2_ALC_GAIN			0x2A
#define FA3_ALC_GAIN			0x2C
#define FA4_ALC_GAIN			0x2E

#define FA1_ATT_GAIN			0x30
#define FA2_ATT_GAIN			0x32
#define FA3_ATT_GAIN			0x34
#define FA4_ATT_GAIN			0x36

#define ICS_TMP_GAIN			0x3A

#define FA1_DDC_PWR				0x00
#define FA2_DDC_PWR				0x04
#define FA3_DDC_PWR				0x08

#define FA1_LMS_PWR				0x10
#define FA2_LMS_PWR				0x14
#define FA3_LMS_PWR				0x18

#define FA1_ALC_PWR				0x20
#define FA2_ALC_PWR				0x24
#define FA3_ALC_PWR				0x28

#define FA1_DUC_PWR				0x30
#define FA2_DUC_PWR				0x34
#define FA3_DUC_PWR				0x38

#define	WIN_ON				0x0040

#define LAMDA				0x0080


#define ICS_SLEEP_PERIOD		0x3C

struct band_info {
	int 			index;
	char			*band;
	char			type;
	char			*desc;
	unsigned short	firsel;
	unsigned char	fa;
	unsigned char	nco;
	char			*rname; /*repater struct name*/
};
	
struct ics_struct {
	/* Public Parameter */
    char 			*bd_name;					/* band name : BAND_FB18, */
    char 			*sb_name;					/* service block name : SB0,SB1,SB2 */
    unsigned char	bd_id;
    unsigned char	sb_id;
    unsigned char 	supported;					/* '1'->FPGA가 지원, '0'->지원하지 않음.*/

	/* 실시간 모니터링 파라미터 */
   	short			alc_gain[2];	//Band별 FA
   	short			man_gain[2];	//Band별
   	short			ref_gain[2];	//Band별
   	short			agc_gain[2];	//Band별 사용 하지 않음 
   
	short			ddc_pwr[2];		//Band별
	short			lms_pwr[2];		//Band별
	short			fbk_pwr[2];		//Band별
	short			att_pwr[2];		//Band별
	short			usr_pwr[2];		//Band별
	
	short			ddc_pkk[2];
	short			lms_pkk[2];
	short			fbk_pkk[2];
	short			att_pkk[2];
	short			usr_pkk[2];
	short			ssb_alc_pkk;

   
	short			iso[2];

	short			papr[2];

	short			dl_papr[2];
	short			ul_papr[2];

	unsigned char	numb_tap;
	unsigned char	unit_tap;

	/* 저장해야 할 파라미터 */
	unsigned char	ics_onoff[2];
	unsigned char	agc_onoff[2];
	unsigned char	alc_onoff[2];

    short	    	agc_setp[2];
    short	    	att_gain[2];	// Band별 
    short	    	alc_setp[2];	// Band별 
    short	    	usr_gain[2];
    short	    	init_gain[2];
    
    short			ilc_gain[2];
    short	    	ilc_tget;
    short	    	ilc_xth;

	short			delay1[2];
	short			delay2[2];
	short			delay3[2];
	int				fc;
	short			lms_tget_run[2];
	short			lms_tget_def[2];
	short			lms_step_run[2];
	short			lms_step_def[2];
	short			lpc_step_run[2];
	short			lpc_step_def[2];
	
	char			path_onoff[2];

	char			band_sel;
    
    short			fa_lvl[2];


    short	    	init_att[2];

    /////////////////////////////////////////////
    //	Register Base Address 
    /////////////////////////////////////////////
    unsigned int    nco_ram_base;
    unsigned int    nco_ram_size;

    unsigned int    ics_reg_base[2];
    unsigned int    ics_reg_size;

    unsigned int    ics_pwr_base[2];
    unsigned int    ics_pwr_size;

    unsigned int    pre_reg_base[2];
    unsigned int    pre_reg_size;

    unsigned int    coe_reg_base[2];
    unsigned int    coe_reg_size;

    unsigned int    lpc_reg_base[2];
    unsigned int    lpc_reg_size;

#if CONFIG_PSR_LTE
	unsigned int	*virt_addr; //challi	
#endif	

    struct band_info 	band_info;
	unsigned char	ilc_onoff[2];
	
	short			alc_pwr_peak_Sum[2][4];
};

/* 파일시스템에 저장되는 파일의 구조
 */
struct ics_info {
	unsigned char	ics_onoff[2];
	unsigned char	agc_onoff[2];
	unsigned char	alc_onoff[2];

    short	    	agc_setp[2];
    short	    	att_gain[2];
    short	    	alc_setp[2];
    short	    	usr_gain[2];
    short	    	init_gain[2];

	short			fa_lvl[2];

    short	    	ilc_tget;
    short	    	ilc_xth;
	short			delay1[2];
	short			delay2[2];
	int				fc;

	/* Path On/Off
	*/
	char			path_onoff[2];

	/* Band Sel
	*/
	char	band_sel;
	
};


extern void init_ics(void);

extern void getGraphAfter(int path, char *buf);
extern int getGraphBefore(int path, char *buf);
extern void getFbDelay(short *ics_dly, short *lte_dly);

/* Power Monitor */
//extern int cal_power(int val);
//extern int cal_iso(int val1,int val2);
//extern int cal_papr(int peak,int mean);

/* Global Variable
 * dl_rssi : DownLink LTE+3G Total 입력 파워 
 * dl_tssi : DownLink LTE+3G Total 출력 파워 
 * ul_rssi : UpLink LTE+3G Total 입력 파워 
 * ul_tssi : UpLink LTE+3G Total 출력 파워 
 */
extern int dl_rssi,ul_rssi;
extern int dl_tssi,ul_tssi;
extern int dl_att_pwr,ul_att_pwr;

extern unsigned int getIcsVersion(void);

//extern int getAdcPwr(uchar path, signed short target1, signed short target2);
extern int getDacPwr(uchar path, double target1, double target2);
extern void getAttPwr(int *dl_att_pwr, int *ul_att_pwr);

/* Gain & Setp */
extern void setIcsAgcSetp(struct ics_struct *pdev, int path, int set);
extern void setIcsUsrGain(struct ics_struct *pdev, int path, short gain);
extern void setIcsTempAtt(struct ics_struct *pdev, short gain);

extern int getIcsAgcGain(struct ics_struct *pdev, int path);
extern int getIcsAlcSetp(struct ics_struct *pdev, int path);
extern int getIcsAlcGain(struct ics_struct *pdev, int path);
extern int getIcsAttGain(struct ics_struct *pdev, int path);
extern int getIcsUsrGain(struct ics_struct *pdev, int path);
extern int getIcsTempGain(struct ics_struct *pdev);

extern void setIcsDacOnOff(struct ics_struct *pdev, int path, int onoff);

/* FA Alloc & Sleep */
extern void setIcsFaMode(struct ics_struct *pdev, int path, int onoff);
extern void setIcsFaAlloc(struct ics_struct *pdev, unsigned short set);
extern int getIcsFaAlloc(struct ics_struct *pdev);
extern void setIcsFaThrLvl(struct ics_struct *pdev, int path, int target);
extern int getIcsFaThrLvl(struct ics_struct *pdev, int path);

extern void confFaAlloc(unsigned char fa_mode, unsigned char fa_stat, short fa_alloc_lvl, 
					short dnc_gain, short dnc_gain_offset, short dnc_attn);
 
/* ISO & ICS */
extern int getIcsIso(struct ics_struct *pdev, int path);
extern int getIcsIsoTotal(struct ics_struct *pdev, int path);

extern void setMuLvl(short lvl1, short lvl2, short lvl3);
extern void getMuLvl(short *lvl1, short *lvl2, short *lvl3);
//extern char *showfloat(char *buf, short val);

extern void run_ics(void);     

extern int loadIcsInfo(struct ics_struct *pdev);
extern void saveIcsInfo(struct ics_struct *pdev);
extern void saveIcsInfoDef(struct ics_struct *pdev);

extern void setIcsDelay(struct ics_struct *pdev, unsigned int delay);
extern unsigned int getIcsDelay(struct ics_struct *pdev);
extern void setLedRank(unsigned short rank);

/*LED 제어*/
extern void setIcsDlLed(struct ics_struct *pdev, int path, unsigned short level);
extern void setIcsIsoLed(struct ics_struct *pdev, int path, unsigned short level);

/*Sleep*/
extern void setIcsSleepOnOff(struct ics_struct *pdev, int onoff);
extern int getIcsSleepOnOff(struct ics_struct *pdev);
extern void setIcsSleepSetup(struct ics_struct *pdev, int setup);     // s
extern void setSleepTurnOnOffLvl(struct ics_struct *pdev, int target);
extern int isIcsSleeping(struct ics_struct *pdev);

/////////////////////////////////////////////////////////////////////////
//	Public 변수 & 함수
//	구현이 되어 있지 않더라도 Body는 반드시 NULL이라도 구현되어야 함. 
/////////////////////////////////////////////////////////////////////////
/* Ics Handle */
extern struct ics_struct *getIcsDev(int band);

extern int getBandType(struct ics_struct *pdev);
extern void setBandType(struct ics_struct *pdev, int type);

extern void setIcsBandOnOff(struct ics_struct *pdev, uchar dual, int path, int onoff);
extern int getIcsBandOnOff(struct ics_struct *pdev, uchar dual, int path);

extern void setIcsOnOff(struct ics_struct *pdev, int path, int onoff);
extern void setIcsAlcOnOff(struct ics_struct *pdev, int path, uchar onoff);
extern void setIcsAlcSetp(struct ics_struct *pdev, int path, int set);

extern void setIcsAgcOnOff(struct ics_struct *pdev, int path, int onoff);
extern void setIcsAgcSetp(struct ics_struct *pdev, int path, int set);
extern int getIcsAgcSetp(struct ics_struct *pdev, int path);

extern void setIcsAttGain(struct ics_struct *pdev, int path, short gain);

extern int loadIcsInfo(struct ics_struct *pdev);
extern void saveIcsInfo(struct ics_struct *pdev);
extern void saveIcsInfoDef(struct ics_struct *pdev);
//extern void confIcsSleepOn(unsigned char sleep_on, short sleep_lvl, 
//					short dnc_gain, short dnc_gain_offset, short dnc_attn);
extern void confFwdIcsOn(unsigned char onoff);
extern void confRvsIcsOn(unsigned char onoff);

//extern void setIcsSleepOnOff(struct ics_struct *pdev, int onoff);
//extern int getIcsSleepOnOff(struct ics_struct *pdev);
//extern void setIcsSleepLvl(struct ics_struct *pdev, int target);
//extern int getIcsSleepLvl(struct ics_struct *pdev);
//extern void setIcsSleepSetup(struct ics_struct *pdev, int setup);
//extern int getIcsSleepSetup(struct ics_struct *pdev);

extern void setIcsDelay1(struct ics_struct *pdev, int path, unsigned int delay);
extern unsigned int getIcsDelay1(struct ics_struct *pdev, int path);
extern void setIcsDelay2(struct ics_struct *pdev, int path, unsigned int delay);
extern unsigned int getIcsDelay2(struct ics_struct *pdev, int path);

extern int getIcsDdcPwr(struct ics_struct *pdev, int path);
extern int getIcsDdcPkk(struct ics_struct *pdev, int path);
extern int getIcsLmsPwr(struct ics_struct *pdev, int path);
extern int getIcsLmsPkk(struct ics_struct *pdev, int path);
extern int getIcsAttPwr(struct ics_struct *pdev, int path);
extern int getIcsAttPkk(struct ics_struct *pdev, int path);
extern int getIcsUsrPwr(struct ics_struct *pdev, int path);
extern int getIcsUsrPkk(struct ics_struct *pdev, int path);
extern int getSsbPwr(struct ics_struct *pdev);
extern int getSsbPkk(struct ics_struct *pdev);
extern int getCsiPkk(struct ics_struct *pdev);

extern uchar getIcsBandType(struct ics_struct *pdev, int *index, char **band,
						 char *type, char **desc, ushort *firsel, uchar *fa, uchar *nco, char **name);
 					 
extern uchar isIcsBandAlive(struct ics_struct *pdev);

extern void setIcsIlcOnOff(struct ics_struct *pdev, int path, int onoff);
extern int getIcsIlcGain(struct ics_struct *pdev, int path);
extern void setIcsIlcXth(struct ics_struct *pdev, short gain);
extern int getIcsIlcXth(struct ics_struct *pdev);
extern int getIcsIlcTatt(struct ics_struct *pdev, int path);
extern void setIcsIlcTget(struct ics_struct *pdev, short set);
extern int getIcsIlcSetp(struct ics_struct *pdev, int path);

extern void setIcsFcoType(struct ics_struct *pdev, short set);
extern short getIcsFcoType(struct ics_struct *pdev);

extern void setIcsFilter(struct ics_struct *pdev, unsigned short filter);


#if CONFIG_PSR_LTE
extern unsigned int 		tVirtAddr;
#endif

#endif
