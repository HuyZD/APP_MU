#define INCLUDE_AD9250        	0
#define INCLUDE_AD9695        	0
#define INCLUDE_ADS4246        	1

struct adc_struct {
    char    			*adc_name;
    int    				adc_id;
    void     			(*init_adc)();
    int     			(*reset_adc)();
    int     			(*setClkPsc)();
    void     			(*showRegInfo)();
    void     			(*showAdcInfo)();
    int     			(*rxAdc)();
    void 				*adc_type;
};



#if INCLUDE_AD9250
/* AD9250을 사용하면, adc_blk.h에 아래 ad9250 초기화 함수는 반드시 extern으로 공유되어야 한다. */
//extern void init_ad9250(struct adc_struct *padc);
#endif

#if INCLUDE_AD9695
/* AD9695을 사용하면, adc_blk.h에 아래 ad9250 초기화 함수는 반드시 extern으로 공유되어야 한다. */
//extern void init_ad9695(struct adc_struct *padc);
#endif
