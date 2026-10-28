/* psa.h  */
#ifndef __PSA_H__
#define __PSA_H__

/* register map */
#define 		PSA_VERSION			0x00
#define 		PSA_CONIFIG			0x02
/* register map */
#define 		PSA_CFG0			0x00
#define 		PSA_CFG1			0x04
#define 		PSA_CFG2			0x08
#define 		PSA_ADDR			0x08
#define 		PSA_WDATA			0x0C
#define 		PSA_RDATA			0x10
#define 		PSA_CFG4			0x10                /* 4 */
#define 		PSA_CFG5			0x14                /* 5 */
#define 		PSA_CFG6			0x18                /* 6 */
#define 		PSA_CFG7			0x1C                /* 7 */

#define 		PSA_CFG8			0x10
#define 		PSA_CFG9			0x12
#define 		PSA_CFG10			0x14
#define 		PSA_CFG11			0x16
#define 		PSA_CFG12			0x18
#define 		PSA_CFG13			0x1A

#define 		PSA_UDC0_BASE		0x20
#define 		PSA_UDC1_BASE		0x30
#define 		PSA_UDC2_BASE		0x40
#define 		PSA_UDC3_BASE		0x50
#define 		PSA_UDC4_BASE		0x60
#define 		PSA_UDC5_BASE		0x70

#define 		XLOCKED				0x0003				/* tx_link_ready 		*/
#define 		RLOCKED				0x000C				/* rr_is_lockedtodata	*/
//#define 		TX_BUSY				0x0030				/* tx_cal_busy  		*/
//#define 		RX_BUSY				0x00C0				/* rx_cal_busy 			*/
#define 		X_READY				0x0300				/* tx_ready 			*/
#define 		R_READY				0x0C00				/* rx_ready 			*/

#define 		PSA_BUF_SIZE		12000

typedef struct _struct_shm_psa {
	unsigned char init;
	unsigned char mode;
	char	psa_buf[6][PSA_BUF_SIZE]; 
} struct_shm_psa;

/* dfe_struct */
struct psa_struct {
    unsigned int    psa_reg_base;
    unsigned int    psa_reg_size;

    unsigned int    psa_ram_base;
    unsigned int    psa_ram_size;


	int				dl_center_freq;
	int				dl_start_freq;
	int				dl_stop_freq;

	int				ul_center_freq;
	int				ul_start_freq;
	int				ul_stop_freq;

	int				fc;
	int				rfc;
	int				fstart;
	int				fstop;
	int				span;
	int				inc;
	int				rbw;

	int				fcount;

	int				mode;
	int				trace;
	int				tmin;
	int				tmax;
	
	unsigned char   init[2];

	/* 저장해야 할 파라미터 */
	short			offset[12];
};

struct psa_info {
	short			offset[12];
};


/* exported function */
extern struct psa_struct *getPsaDev(void);

/* defined @led.c */
extern void setRunLedOnOff(int onoff);
extern void setAlmLedOnOff(int onoff);

extern void confPsaChpwr(unsigned char Channel, short Xstart, short Xend);
extern void confPsaSisoChpwr(unsigned char Channel, short Xstart, short Xend);
extern void confPsaMimoChpwr(unsigned char Channel, short Xstart, short Xend);
extern int getPsaData(char *buf, int path);
extern int getPsa(char *buf);
extern int getPsaSiso(char *buf);
extern int getPsaMimo(char *buf);

#endif

