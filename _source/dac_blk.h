#ifndef __DAC_BLK_H__
#define __DAC_BLK_H__

#define INCLUDE_AD9783        	1
#define INCLUDE_AD9695        	0

struct dac_struct {
    char    			*dac_name;
    int    				dac_id;
    void     			(*init_dac)();
    void     			(*reset_dac)();
    int     			(*setClkPsc)();
    void     			(*showRegInfo)();
    void     			(*showDacInfo)();
    int     			(*txDac)();
	void 				(*writeDacReg)();
	ushort 				(*readDacReg)();
    void 				*dac_type;
};

extern int isDacAlive(int band);

//#if INCLUDE_AD9695
///* AD9695을 사용하면, adc_blk.h에 아래 ad9250 초기화 함수는 반드시 extern으로 공유되어야 한다. */
//extern void init_ad9695(struct adc_struct *padc);
//#endif

#endif
