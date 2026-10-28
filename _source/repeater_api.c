// #include <stdlib.h>
// #include <string.h>
#include "config.h"
#include "fpga.h"
#include "genlib.h"
#include "ics.h"
#include "protocol_private.h"
#include "repeater.h"
#include <math.h>

// #include "info_structure.h"
#include "flash.h"
#include "monlib.h"
#include "tbl.h"
#include "tfs.h"
#include <stdarg.h>


#define BAND3_C1_EARFCN_CENTER 1500
#define BAND3_C2_EARFCN_CENTER 1850

#define REPEATER_INFO_FILE "repeater"

struct repeater_info pinf;
struct main_ctrl_mu_struct pinfo;
extern struct hpa_info_struct hinfo;

int loadRepeaterInfo(struct repeater_struct *pdev);
void saveRepeaterInfo(struct repeater_struct *pdev);
void saveRepeaterInfoDef(struct repeater_struct *pdev);
void DefaultBackup(struct repeater_struct *pdev);

extern void C2_PLL_Control(unsigned _Val);
extern unsigned char AmpoffFlag[MAX_BAND];

void setRptIcsOnOff(int band, char onoff) {
  struct ics_struct *idev;

  if (band == B3)
    idev = getIcsDev(B3);
  else
    idev = getIcsDev(B28);

  setIcsOnOff(idev, DL, onoff);
  setIcsOnOff(idev, UL, onoff);
}

//// C1 1500  1835 1557.1   277.9
//// C2 1850  1870 1583.9   286.1

// 1803 ~ 1880
// 1805 MHz ~ 1880 MHz
// (1200 - 1949)
// C1 1825.46 ~ 1844.54 (1405 ~ 1595) (1825.5 ~ 1844.5) 19M
// Rakuten : 1825.0 ~ 1845.0  1500 (1400 ~ 1599)
// C2 1860.46 ~ 1879.54 (1755 ~ 1945)
// Docomo : 1860.1 ~ 1879.9 1850 (1751 ~ 1949)

void setBandwidthType(struct repeater_struct *pdev, uint32_t _BW, short band) {
  struct ics_struct *idev;

	if (band == B28) {
		idev = getIcsDev(B28);
		if(_BW == 0) 
			setIcsDacOnOff(idev,DL,_Off);
		else setIcsDacOnOff(idev,DL,_On); 		
	} else {
		idev = getIcsDev(B3);
		setIcsFilter(idev, _BW);
	}

	printf("_BW [%d]: %d\n", band, _BW);
}

void setFilterType(struct repeater_struct *pdev, uint32_t _earfcn_val, short band) {
  extern int getB3FromEarfcn(int earfcn, int *rf_fc, int *rf_lo, int *bb_fc,int *bb_of, unsigned char *mno);
  extern void setLocalOsc(uchar band, unsigned int lo);

  //	int Center_Freg;
  struct ics_struct *idev;
  //	if(band == B3)
  {
    idev = getIcsDev(B3);

    static int earfcn;
    int rf_fc, rf_lo, bb_fc, bb_of;
    uchar mno;

    earfcn = _earfcn_val;
    if (getB3FromEarfcn(earfcn, &rf_fc, &rf_lo, &bb_fc, &bb_of, &mno) == -1) {
      printf("earfcn Out of Range : %d\n", earfcn);
      return 0;
    }
    setLocalOsc(B3, rf_lo * 100);
    setIcsMno(idev, mno);
    setIcsNco(idev, bb_of);

    if (mno == 3) {
      setIcsUsrGain(idev, DL, 300);
      setIcsUsrGain(idev, UL, 200);
    } else if (mno == 2) {
      setIcsUsrGain(idev, DL, 0);
      setIcsUsrGain(idev, UL, 100);
    } else {
      setIcsUsrGain(idev, DL, 0);
      setIcsUsrGain(idev, UL, 0);
    }
//	pdev->sys_dl_dnc_gain_offset[B3][1] = 300;
//	pdev->sys_ul_dnc_gain_offset[B3][1] = 200;

	printf("%10s%10s%10s%10s%10s%10s","EARFCN|","RF Fc|","RF Lo|","BB Fc|","BB Offset|","MNO|");
    printf("\n");
    printf("%9d|", earfcn);
    printf("%7d.%d|", rf_fc / 10, abs(rf_fc) % 10);
    printf("%7d.%d|", rf_lo / 10, abs(rf_lo) % 10);
    printf("%7d.%d|", bb_fc / 10, abs(bb_fc) % 10);
    printf("%7d.%d|", bb_of / 10, abs(bb_of) % 10);
    printf("%9d|", mno);
    printf("\n");
  }
}

unsigned char getBandwidth_Band3(struct repeater_struct *pdev) {
  struct ics_struct *idev;

  idev = getIcsDev(B3);
  return (unsigned char)getIcsFilter(idev);
}

int loadRepeaterInfo(struct repeater_struct *pdev) {
	int i;

	loadinfo(&pinf);

	for (i = 0; i < MAX_BAND; i++) {
		pdev->ir_pwm_br[i] = pinf.ir_pwm_br[i];
	}

	pdev->tr_pwm = pinf.tr_pwm;
  return 0;
}

/*
 * saveRepeaterInfo(): Save the repeater information to the TFS file system.
 */
void saveRepeaterInfo(struct repeater_struct *pdev)
{
	int							i;
	struct repeater_info 		pinf;
	
	pinf.tr_pwm = pdev->tr_pwm;

	for (i=0; i<MAX_BAND; i++)
		{
		pinf.ir_pwm_br[i] = pdev->ir_pwm_br[i];
		}

	saveinfo(&pinf);
}

int loadMainInfo(struct repeater_struct *pdev) {

		load_main_info(&pinfo);

		pdev->boot_initilization = pinfo.boot_initilization;
		pdev->Topology_Set = pinfo.Topology_Set;
		pdev->AGS_onoff = pinfo.AGS_onoff;
		 pdev->ISO_onoff = pinfo.ISO_onoff;
		for (int i = 0;i<20;i++){
			pdev->repeater_sn[i] = pinfo.repeater_sn[i];
		}
		for(int i =0;i<4;i++){
			pdev->br_onoff[i] = pinfo.br_onoff[i];

		}
		for(int i =0;i<4;i++){
			pdev->band_in_use[i] = pinfo.band_in_use[i];

		}
		pdev->temp_high = pinfo.temp_high;
		pdev->temp_offset = pinfo.temp_offset;
		pdev->alarmDuration = pinfo.alarmDuration;
		pdev->tr_pwm = pinfo.tr_pwm;
		pdev->tr_sel = pinfo.tr_sel;
		for(int i = 0;i<4;i++){
			pdev->ir_pwm_br[i] = pinfo.ir_pwm_br[i];
		}
		for(int i = 0;i<4;i++){
			pdev->ir_pwm_sel[i] = pinfo.ir_pwm_sel[i];
		}
		pdev->led_test = pinfo.led_test;


  return 0;
}
void saveMainInfo(struct repeater_struct *pdev)
{
	pinfo.boot_initilization = pdev->boot_initilization;
	pinfo.Topology_Set = pdev->Topology_Set;
	pinfo.AGS_onoff = pdev->AGS_onoff;
	pinfo.ISO_onoff = pdev->ISO_onoff;
	for (int i = 0;i<20;i++){
		pinfo.repeater_sn[i] = pdev->repeater_sn[i];
	}
	for(int i =0;i<4;i++){
		pinfo.br_onoff[i] = pdev->br_onoff[i];

	}
	for(int i =0;i<4;i++){
		pinfo.band_in_use[i] = pdev->band_in_use[i];

	}
	pinfo.temp_high = pdev->temp_high;
	pinfo.temp_offset = pdev->temp_offset;
	pinfo.alarmDuration = pdev->alarmDuration;
	pinfo.tr_pwm = pdev->tr_pwm;
	pinfo.tr_sel = pdev->tr_sel;
	for(int i = 0;i<4;i++){
		pinfo.ir_pwm_br[i] = pdev->ir_pwm_br[i];
	}
	for(int i = 0;i<4;i++){
		pinfo.ir_pwm_sel[i] = pdev->ir_pwm_sel[i];
	}
	pinfo.led_test = pdev->led_test;


	save_main_info(&pinfo);
}
/*
 * saveRepeaterInfoDef()
 * FilesystemÃ¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â±Âº Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Æ’Â±Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã©â‚¬Â¾Ã‘Æ’Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã©â‚¬Â¾Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å’â€“Ã¦â‚¬Â¨Ã«ÂºÂ£Ì�Æ’ï¿½Ã¥ï¿½Â Ì�ï¿½Å’ÃªÂ¶Â¨Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã§â€�Â±Ã¯Â¿Â½Ã¯Â¦Â«Ã«ï¿½Â¯Ì�â€°Â»Ã¯Â¿Â½Ì�â€šâ€¢ Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã§â€˜Å“ÄƒÆ’Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¨Â«Â­Ã¯Â¿Â½ Ã¯Â¿Â½Ã«â€¡Â¦Ã§Å’Â¿Ã«â€”Â«Ì�Å“ÂªÃ¥ï¿½Â Ì�ï¿½Ë†Ã«Â®Â¡
 * Ã§Â¹ï¿½Ã«Â²Â¿Ì�â€šÂ´Ã­Æ’ÂªÃ¥ï¿½Â Ì�ï¿½Ë†Ã¢â‚¬Â¦Ã¨â€¡Â¾Ì�â€“ï¿½Ã«Å“ï¿½Ã¥ï¿½Â Ã¯Â¿Â½ Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¡Ã¯Â¿Â½Ã«Â§Å¸Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¯Â¿Â½Ì�Â Â§Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¨Â¸Â°Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Â¼Â¨Ã¨Â½â€¦Ã«Âªâ€¦Ã¯Â¿Â½Ã¢Ë†Â½Ì�Ëœâ„¢Ã¦Â²â€¦Ì�â€¹Â¨Ã«Â¤â€ Ã¯Â¿Â½Ã«Â£â€ Ã¥ï¿½Â Ì�ï¿½Â¬ÃªÂµÂ¦Ã¨Â«Â­Ã¯Â¿Â½ Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Â£ï¿½Ã¯Â¿Â½Ì�â€šâ€¢Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¯Â¿Â½ÃªÂ¶â€¹Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã§â€�Â±Ì�ï¿½Â²Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¥Â â€°Ã¯Â¿Â½ .
 */


void DefaultBackup(struct repeater_struct *pdev)
{
	
}



void saveRepeaterInfoDef(struct repeater_struct *pdev)
{


}

/*
 * loadSnInfo()
 * FilesystemÃ¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«â€žÂ°Ã¥ï¿½Â Ì�ï¿½Â©Ã«Â²â‚¬Ã¯Â¿Â½ÃªÂ±Â Ã¥ï¿½Â Ì�ï¿½â€žÃ«ï¿½Â¡Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¥ï¿½Â Ì�ï¿½Ë†Ì�ï¿½Â»Ã¯Â¿Â½Ã«Å“ï¿½Ã¥ï¿½Â Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¥Â â€°Ã¯Â¿Â½Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½ÃªÂ½â€�Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â´Ã­Ë†Â£Ã§Å’Â·Ã«ÂªÆ’Ì�ï¿½â€¢Ã¥ï¿½Â Ì�ï¿½Å’Ã«Â§Â¶Ã¥ï¿½Â Ì�ï¿½Ë†Ì�ï¿½Â»Ã¯Â¿Â½Ã«Å“ï¿½Ã¥ï¿½Â Ã¯Â¿Â½.
 */
int loadSnInfo(struct repeater_struct *pdev)
{
	
  return (1);
}

/*
 * saveSnInfo()
 * FilesystemÃ¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½ repeater_sn
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¥Â â€°Ã¢â€™Â´Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Æ’Â·Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½.
 */
void saveSnInfo(struct repeater_struct *pdev)
{
	
}

/*
 * saveRepeaterInfoDef()
 * FilesystemÃ¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¥ï¿½Â Ì�ï¿½Ë†Ì�ï¿½Â»Ã¯Â¿Â½Ã«Å“ï¿½Ã¥ï¿½Â Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«â€žÂ°Ã¥ï¿½Â Ì�ï¿½Â©Ã«Â²â‚¬Ã¯Â¿Â½ÃªÂ±Â Ã¥ï¿½Â Ì�ï¿½â€žÃ«ï¿½Â¡Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¥ï¿½Â Ì�ï¿½Ë†Ì�ï¿½Â»Ã¯Â¿Â½Ã«Å“ï¿½Ã¥ï¿½Â Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«ÂªËœÃ¯Â¿Â½Ì�â€ Â»Ã¦Â´Â¹Ã¯Â¼Â¥Ã¯Â¿Â½Ã¥ï¿½Â Ì�ï¿½Å’Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â§ï¿½Ì�â‚¬Â¥Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´ÃªÂ¿Â¨Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Â§Â¶Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¥Â â€°Ã¢â€™Â´Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â¥Ã¯Â¿Â½Ã«Æ’Â·Ã¯Â¿Â½Ã«Å“ï¿½Ã¯Â¿Â½Ã«Å¸Â©Ã¦ï¿½Â´Ã¯Â¿Â½
 * .
 */
void saveSnInfoDef(struct repeater_struct *pdev) {
 
}


uint8_t ISAscII(char Data)
{
	if('0' <= Data && Data <= '9')	return 1;
	else							return 0;
}

uint8_t DataConv ( uint8_t HighLow, uint8_t Value )
{
	uint8_t Digit10;
	uint8_t Digit1;

  Digit1 = (Value & 0x0f) + 0x30;
  Digit10 = ((Value >> 4) & 0x0f) + 0x30;

	if(Digit1 >= 0x3a) 	Digit1 += 0x07;
	if(Digit10 >= 0x3a) Digit10 += 0x07;

	if(HighLow == 1) return( Digit10 );
	if(HighLow == 0) return( Digit1 );
	return(0);
}


unsigned int ascii_to_hex(char* str, size_t size, uint8_t* hex)
{
    unsigned int i, h, high, low;
	int k = 0;
	
    for (h = 0, i = 0; i < size; i += 1, ++h) {

		if('0' <= str[i] && str[i] <= '9')
		{
		    high = (str[i] > '9') ? str[i] - 'A' + 10 : str[i] - '0';
			if(str[i] == 0x3a) high  = 0x3a;
			k++;
		}
		else high = str[i];

		hex[h] = (high);
    }
	return k;
//    return h;
}




void Def_Backup(struct repeater_struct *pdev)
{
	saveRepeaterInfoDef(pdev);
	saveRepeaterInfo(pdev);
	saveTemp();
	SaveAmpInfo(pdev);
	saveCalInfo();
	printf("Def_Backup \n");
}



void __DefaultBackup(struct repeater_struct *pdev)
{
	

}


void _saveRepeater(struct repeater_struct *pdev)
{
	DefaultBackup(pdev);
	saveRepeaterInfo(pdev);
}
