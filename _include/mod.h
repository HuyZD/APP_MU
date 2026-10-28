#ifndef _mod_h
#define _mod_h

/* ics register map */


#define MOD_STATUS				0x00 
#define MOD_CONFIG0				0x04
#define MOD_CONFIG1				0x04
#define	MOD_CONFIG2				0x30

#define MOD_CELL_INF0			0x08
#define MOD_CELL_INF1			0x0C
#define MOD_CELL_INF2			0x10

#define MOD_MIB_INF0			0x20
#define MOD_MIB_INF1			0x24

#define MOD_START_TIME			0x2C
#define MOD_NCO_CNF   			0x34
#define MOD_HW_DLY   			0x1C

#define MOD_TDD_CONFIG0			0x40
#define MOD_TDD_CONFIG1			0x44
#define MOD_TDD_CONFIG2			0x48
#define MOD_TDD_CONFIG3			0x4C

#define MOD_PSS_INF0			0x50     // corr
#define MOD_PSS_INF1			0x5C     // max
#define MOD_PSS_INF2			0x60     // threshold

#define MOD_SSS_INF0			0x64
#define MOD_SSS_INF1			0x68
#define MOD_SSS_INF2			0x6C

#define MOD_AGC_SETP			0x14    
#define MOD_AGC_GAIN			0x18  

#define MOD_SYNC_CNT			0x70 

#define MOD_FAIL_CNT0			0x74
#define MOD_FAIL_CNT1		 	0x78
#define MOD_FAIL_CNT2 			0x7C

//#define MOD_SSB_CNF			0x7A


struct mod_struct {

    /////////////////////////////////////////////
    //	Register Base Address 
    /////////////////////////////////////////////
    
    unsigned int    mod_reg_base;
    unsigned int    mod_reg_size;
   
    unsigned int    dem_reg_base;
    unsigned int    dem_reg_size;
     
    int				fc;
     
    // Control Info
    unsigned char   version;
    unsigned char   start;
    unsigned char   mode;
    unsigned int    gain;
    unsigned int	nco_val;
    int				agc_setp;
    
    // Cell Info      
    unsigned char   SearchSuccess;
    unsigned char   SearchDone;      
    
    unsigned int    detectStatus;
    unsigned int    decodeStatus;
      
    unsigned char   pssSuccess;
    unsigned char   sssSuccess;    
    unsigned char   mibSuccess;
    
    unsigned int    NCellID;
    unsigned char   NCellID2;
    
    unsigned int    frequencyOffset;
    unsigned int    timingOffset;
    
    unsigned int    psscorr0;
    unsigned int    psscorr1;
    unsigned int    psscorr2;
    unsigned int    pssthreshold;
    
    unsigned int    ssscorr;
    unsigned int    sssmaxcorr;
    unsigned int    sssthreshold;    
    
    // MIB Info
    unsigned char   ssbIndex;    
    unsigned int    sfn;
    unsigned int    scsCommon;
    unsigned int    Kssb;
    unsigned char   dmrsTypeAPos;
    unsigned int    sib1Config;
    unsigned char   cellBarred;
    unsigned char   intraFreqResel;
    unsigned char   hrf;

    // TDD Info
    unsigned char	sync_lock;
    unsigned int   	dlLen;    
    unsigned int    periodicity;
    unsigned int    dlOffset;
    unsigned int    ulOffset;
    unsigned int	hwDelay;
    
    // Others
    unsigned char   DEM_START;  
    unsigned int	cnfssbidx;
    unsigned int	cur_ssb_freq;

    unsigned int	ssb_arfcn1;
    unsigned int	ssb_arfcn2;
    	
};

/* 파일시스템에 저장되는 파일의 구조
 */
 
struct mod_info {

    // Control Info
    unsigned int	nco_val;
    int				agc_setp;
    
     // TDD Info
    unsigned int   	dlLen;    
    unsigned int    periodicity;
    unsigned int    dlOffset;
    unsigned int    ulOffset;
    unsigned int	hwDelay;   	

    unsigned int	ssb_arfcn1;
    unsigned int	ssb_arfcn2;

};


//extern void init_mod(void);

extern struct mod_struct *getModDev();

extern unsigned int getModMode(struct mod_struct *mdev);
extern unsigned int getModSearchSuccess(struct mod_struct *mdev);
extern unsigned int getModSearchDone(struct mod_struct *mdev);
extern unsigned int getModdetectStatus(struct mod_struct *mdev);
extern unsigned int getModdecodeStatus(struct mod_struct *mdev);
extern unsigned int getModNCellID(struct mod_struct *mdev);
extern unsigned int getModNCellID2(struct mod_struct *mdev);
extern unsigned int getModFreqOffset(struct mod_struct *mdev);
extern unsigned int getModTimingOffset(struct mod_struct *mdev);

extern unsigned int getModssbIndex(struct mod_struct *mdev);
extern unsigned int getModsfn(struct mod_struct *mdev);
extern unsigned int getModscsCommon(struct mod_struct *mdev);
extern unsigned int getModKssb(struct mod_struct *mdev);
extern unsigned int getModdmrsTypeAPos(struct mod_struct *mdev);
extern unsigned int getModsib1Config(struct mod_struct *mdev);
extern unsigned int getModcellBarred(struct mod_struct *mdev);
extern unsigned int getModintraFreqResel(struct mod_struct *mdev);
extern unsigned int getModhrf(struct mod_struct *mdev);


extern void setModDlLength(struct mod_struct *mdev, int val);
extern int getModDlLength(struct mod_struct *mdev);
extern void setModTddPeriod(struct mod_struct *mdev, int val);
extern int getModTddPeriod(struct mod_struct *mdev);


extern void setHoldSync(struct mod_struct *mdev, int onoff);
extern void setHoldTime(struct mod_struct *mdev, int time);
extern void setModDlOffset(struct mod_struct *mdev, int val);
extern int getModDlOffset(struct mod_struct *mdev);
extern void setModUlOffset(struct mod_struct *mdev, int val);
extern int getModUlOffset(struct mod_struct *mdev);
extern unsigned int getModSWStart(struct mod_struct *mdev);
extern void setModSWStart(struct mod_struct *mdev, int start);
extern unsigned int getModModectl(struct mod_struct *mdev);
extern void setModModectl(struct mod_struct *mdev, int mode);
extern unsigned int getModPSSCorr(struct mod_struct *mdev, char index);
extern unsigned int getModPSSMaxCorr(struct mod_struct *mdev);
extern unsigned int getModPSSThreshold(struct mod_struct *mdev);
extern unsigned int getModSSSCorr(struct mod_struct *mdev);
extern unsigned int getModSSSMaxCorr(struct mod_struct *mdev);
extern unsigned int getModSSSThreshold(struct mod_struct *mdev);
extern unsigned int getPssFailCount(struct mod_struct *mdev);
extern unsigned int getSssFailCount(struct mod_struct *mdev);
extern unsigned int getMibFailCount(struct mod_struct *mdev);

extern void run_mod(void);;


#endif
