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

struct repeater_info pinfo;
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

	loadinfo(&pinfo);

	for (i = 0; i < MAX_BAND; i++) {
		pdev->ir_pwm_br[i] = pinfo.ir_pwm_br[i];
	}

	pdev->tr_pwm = pinfo.tr_pwm;
  return 0;
}

/*
 * saveRepeaterInfo(): Save the repeater information to the TFS file system.
 */
void saveRepeaterInfo(struct repeater_struct *pdev)
{
	int							i;
	struct repeater_info 		pinfo;
	
	pinfo.tr_pwm = pdev->tr_pwm;

	for (i=0; i<MAX_BAND; i++)
		{
		pinfo.ir_pwm_br[i] = pdev->ir_pwm_br[i];
		}

	saveinfo(&pinfo);
}

/*
 * saveRepeaterInfoDef()
 * Filesystemï¿½ëœ�ï¿½ëŸ¥ï¿½ë±º ï¿½ëœ�ï¿½ëŸ¥ï¿½ëƒ±ï¿½ëœ�ï¿½ëŸ©é€¾Ñƒëœ�ï¿½ëŸ©é€¾ï¿½
 * ï¿½ëŒ–æ€¨ëº£́ƒ�å� ́�Œê¶¨ï¿½ëœ�ï¿½ëŸ¥ç”±ï¿½ï¦«ë�¯́‰»ï¿½́‚• ï¿½ëœ�ï¿½ëŸ¥ç‘œăƒ¥ëœ�ï¿½ëŸ©è«­ï¿½ ï¿½ë‡¦çŒ¿ë—«́œªå� ́�ˆë®¡
 * ç¹�ë²¿́‚´íƒªå� ́�ˆâ€¦è‡¾́–�ëœ�å� ï¿½ ï¿½ëœ�ï¿½ëŸ¡ï¿½ë§Ÿï¿½ëœ�ï¿½ëŸ©ï¿½́ §ï¿½ëœ�ï¿½ëŸ©è¸°ï¿½
 * ï¿½ë¼¨è½…ëª…ï¿½âˆ½́˜™æ²…́‹¨ë¤†ï¿½ë£†å� ́�¬êµ¦è«­ï¿½ ï¿½ëœ�ï¿½ë£�ï¿½́‚•ï¿½ëœ�ï¿½ëŸ©ï¿½ê¶‹ï¿½ëœ�ï¿½ëŸ¥ç”±́�²ëœ�ï¿½ëŸ¥å ‰ï¿½ .
 */


void DefaultBackup(struct repeater_struct *pdev)
{
	
}



void saveRepeaterInfoDef(struct repeater_struct *pdev)
{


}

/*
 * loadSnInfo()
 * Filesystemï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï¿½ë„°å� ́�©ë²€ï¿½ê± å� ́�„ë�¡ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶å� ́�ˆ́�»ï¿½ëœ�å� ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥å ‰ï¿½ï¿½ëœ�ï¿½ëŸ¥ï¿½ê½”ï¿½ëœ�ï¿½ëŸ©æ�´ê¿´íˆ£çŒ·ëªƒ́�•å� ́�Œë§¶å� ́�ˆ́�»ï¿½ëœ�å� ï¿½.
 */
int loadSnInfo(struct repeater_struct *pdev)
{
	
  return (1);
}

/*
 * saveSnInfo()
 * Filesystemï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½ repeater_sn
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥å ‰â’´ëœ�ï¿½ëŸ¥ï¿½ëƒ·ï¿½ëœ�ï¿½ëŸ©æ�´ï¿½.
 */
void saveSnInfo(struct repeater_struct *pdev)
{
	
}

/*
 * saveRepeaterInfoDef()
 * Filesystemï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶å� ́�ˆ́�»ï¿½ëœ�å� ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï¿½ë„°å� ́�©ë²€ï¿½ê± å� ́�„ë�¡ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶å� ́�ˆ́�»ï¿½ëœ�å� ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï¿½ëª˜ï¿½́†»æ´¹ï¼¥ï¿½å� ́�Œë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ï¿½
 * ï¿½ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥ï§�́€¥ëœ�ï¿½ëŸ©æ�´ê¿¨ëœ�ï¿½ëŸ¥ï¿½ë§¶ï¿½ëœ�ï¿½ëŸ¥å ‰â’´ëœ�ï¿½ëŸ¥ï¿½ëƒ·ï¿½ëœ�ï¿½ëŸ©æ�´ï¿½
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





void updateMuInfo(struct repeater_struct *prpt)
{
	int	i,j;

//	prpt->ru_install[0].bit.b0 = 1;
//	prpt->_ru_install.bit.ru1 = 1;
	prpt->_ru_install.Data[0] = prpt->children_ru_number[0];
	prpt->_ru_install.Data[1] = prpt->children_ru_number[1];
	prpt->_ru_install.Data[2] = prpt->children_ru_number[2];
	prpt->_ru_install.Data[3] = prpt->children_ru_number[3];

	if(prpt->children_ru_number[0] == 1)
	{
		prpt->_ru_install.bit.ru1 = 1;
	}
	
	if(prpt->children_ru_number[0] == 2)
	{
		prpt->_ru_install.bit.ru1 = 1;
		prpt->_ru_install.bit.ru2 = 1;
	}
	
	if(prpt->children_ru_number[0] == 3)
	{
		prpt->_ru_install.bit.ru1 = 1;
		prpt->_ru_install.bit.ru2 = 1;
		prpt->_ru_install.bit.ru3 = 1;
	}
	
	if(prpt->children_ru_number[0] == 4)
	{
		prpt->_ru_install.bit.ru1 = 1;
		prpt->_ru_install.bit.ru2 = 1;
		prpt->_ru_install.bit.ru3 = 1;
		prpt->_ru_install.bit.ru4 = 1;
	}

}
