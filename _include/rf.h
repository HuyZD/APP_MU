
#define		BAND0		0
#define		BAND1		1
#define		BAND2		2
#define		BAND3		3

extern int	CurrentBand;
extern char	*band_name[];

extern int isBandAlive(uchar band);

/* Band별 Attn은 3개를 가정. 
 * DL ALC ATT : Downlink RF DownConverter에 위치한 Atten
 * UL ALC ATT : Uplink RF DownConverter에 위치한 Atten
 * UL AGC ATT : Uplink RF UpConverter에 위치한 Atten (*Home Repeater 타입의 중계기에는 없음)
 */
extern void getRfAttnAll(uchar band, short *alc_d, short *alc_u, short *agc_u);

/* id : 0 -> alc_d
 * id : 1 -> alc_u
 * id : 2 -> agc_u
 */
extern short getRfAttnInd(uchar band, uchar id);

/*
 */
extern void setRfAttn(uchar band, uchar id, short data);

/* band  : 0->700M, 1->800M, 2->1.7G, 3-> 2.1G
 * path  : 0->DL, 1->UL
 * onoff : 0->off, 1->On
 */
extern void setHpaOnOff(uchar band, uchar path, uchar onoff);

/* RF PLL
 * local : LO=1667.5Mhz
 * ref   : 30.72Mhz
 */
extern void confPll(int band, unsigned int local, unsigned int ref, unsigned int Fres, int prescale, int verbose);
extern void init_pll(void);
extern int isPllLocked(int band);
