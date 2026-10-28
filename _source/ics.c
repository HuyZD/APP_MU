//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"

/* 4개의 밴드를 지원한다. 각 밴드는 최대 2개의 SB(Service Block)를 가지고 있다.  */
static struct ics_struct ics_dev[MAX_BAND];

struct ics_struct *getIcsDev(int band)
{
	return (&ics_dev[band]);
}

void showIcsDev(void)
{
	struct ics_struct 	*pdev;
	
    //
	//pdev = getIcsDev(0);
    //
	//if (isIcsSbAlive()) {
  	//	getIcsBandType(pdev, &pdev->band_info.index, &pdev->band_info.band, &pdev->band_info.type, &pdev->band_info.desc, &pdev->band_info.firsel, &pdev->band_info.fa, &pdev->band_info.nco,&pdev->band_info.rname);
	//}
	//printf("%8s|%8s|%8s|%8s|%10s|\n"," ","index","band","type","desc");
   	//for (band=0;band<2;band++) {
    //	pdev = getIcsDev(0);
	//		if (isBandAlive(pdev,sb)) {
    //			pdev->status = 1;
    //			getBandType(pdev, sb, &pdev->band_info.index, 
    //									&pdev->band_info.band, 
    //									&pdev->band_info.type, 
    //									&pdev->band_info.desc, 
    //									&pdev->band_info.firsel, 
    //									&pdev->band_info.fa, 
    //									&pdev->band_info.nco);
    //			printf("%8s|",sb_name[sb]);
    //			printf("%8d|",pdev->band_info.index);
    //			printf("%8s|",pdev->band_info.band);
    //			printf("%7s%c|"," ", pdev->band_info.type);
    //			printf("%10s|",pdev->band_info.desc);
    //			//printf("desc  -> %s\n",pdev->band_info.firsel);
    //			//printf("desc  -> %s\n",pdev->band_info.fa);
    //			//printf("desc  -> %s\n",pdev->band_info.nco);
    //			printf("\n");
	//		}
    //}
}

/* RF_DUO/Speed_Duo/ICS_Duo 
 * ------------------------------------------------------------
 * SB0	: 0x000000
 * SB1	: 0x010000
 * SB15	: 0x0F0000 -> PSA
 * ------------------------------------------------------------
 */
static unsigned int nco_ram_base[MAX_BAND] = {
	0x43C00000,0x43C10000
};
static unsigned int ics_reg_base[MAX_BAND][2] = {
	{0x43C08000,0x43C08080},				/* B28 : 700M */
	{0x43C18000,0x43C18080},				/* B3  : 1.8G */
};
static unsigned int ics_pwr_base[MAX_BAND][2] = {
	{0x43C08100,0x43C08140},
	{0x43C18100,0x43C18140},
};
static unsigned int pre_reg_base[MAX_BAND][2] = {
	{0x43C0A000,0x43C0B000},
	{0x43C1A000,0x43C1B000},
};
static unsigned int coe_reg_base[MAX_BAND][2] = {
	{0x43C0C000,0x43C0D000},
	{0x43C1C000,0x43C1D000},
};
static unsigned int lpc_reg_base[MAX_BAND][2] = {
	{0x43C0E000,0x43C0F000},
	{0x43C1E000,0x43C1F000},
};

char *band_name[MAX_BAND] = {"BAND_NB70","BAND_NB17"};

extern struct repeater_struct *getRptDev(void);
extern struct repeater_struct rpt_dev;

/* run_ics()
 * repeater() 함수에 100ms 마다 호출되야 한다. 
 */
void run_ics(void)
{
	int 				band,path = 0;
	struct ics_struct 	*pdev;
	struct repeater_struct						*prpt;
	static int cnt = 0;
	static short  Curr_pwr;
	static short  Curr_pwr_Sum;

//	getAdcPwr(&dl_rssi, &ul_rssi);
//	getDacPwr(&dl_tssi, &ul_tssi);
	
	prpt = getRptDev();


	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (isIcsBandAlive(pdev)) {

			for (path=0;path<2;path++) {

				/* ---------------------------------------------------------------------
				 *  PATH(DL,UL) POWER 수집.
				 * 	현재  업데이트
				 * -------------------------------------------------------------------*/			
				/* DDC 파워 업데이트 */                                      
				pdev->ddc_pwr[path] 		= getIcsDdcPwr(pdev,path);
				pdev->ddc_pkk[path] 		= getIcsDdcPkk(pdev,path);
				                        	                                     
				/* LMS 파워 업데이트 */     	                                 
				pdev->lms_pwr[path] 		= getIcsLmsPwr(pdev,path);
				pdev->lms_pkk[path] 		= getIcsLmsPkk(pdev,path);
				                        	
				/* ALC 파워 업데이트 */     	                                 
				pdev->att_pwr[path] 		= getIcsAttPwr(pdev,path);
				pdev->att_pkk[path] 		= getIcsAttPkk(pdev,path);
                                        	
				/* ALC gain 업데이트 */    	                                  
				pdev->alc_gain[path]  		= getIcsAlcGain(pdev,path);
				                        	
				/* DUC 파워 업데이트 */     	                                 
				pdev->usr_pwr[path]	 		= getIcsUsrPwr(pdev,path);
				pdev->usr_pkk[path]			= getIcsUsrPkk(pdev,path);
    		
				/* AGC gain 업데이트 */ 
				pdev->man_gain[path] 		= getIcsManGain(pdev,path);
				pdev->ref_gain[path] 		= getIcsRefGain(pdev,path);
				
				pdev->agc_gain[path] 		= getIcsRefGain(pdev,path);
				
				pdev->iso[path] 			= getIcsIso(pdev,path);
				pdev->ilc_gain[path] 		= getIcsIlcGain(pdev,path);
				
				pdev->papr[path] 			= getIcsPapr(pdev,path);
				
				/* ALC step 업데이트 */ 
				pdev->alc_setp[path] 		= getIcsAlcSetp(pdev,path);
				pdev->ilc_onoff[path]		= getIcsIlcOnOff(pdev,path);
				pdev->ics_onoff[path]		= getIcsOnOff(pdev,path);
			
				pdev->alc_onoff[path]		= getIcsAlcOnOff(pdev,path);
				pdev->agc_onoff[path]		= getIcsAgcOnOff(pdev,path);
			}

		}
	}

}
    

/* init_ics()
 * 
 */
void init_ics(void)
{
	int 			index;

	struct ics_struct 	*pdev;

	unsigned int 		path;
	int					band,band_type;

	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		for (path=0;path<2;path++) {
			pdev->ics_reg_base[path] 	= ics_reg_base[band][path];
			pdev->ics_reg_size 	= 32;
			
			pdev->ics_pwr_base[path] 	= ics_pwr_base[band][path];
			pdev->ics_pwr_size 	= 16;

			pdev->pre_reg_base[path] 	= pre_reg_base[band][path];
			pdev->pre_reg_size 	= 16;

			pdev->coe_reg_base[path] 	= coe_reg_base[band][path];
			pdev->coe_reg_size 	= 16;
    	
			pdev->lpc_reg_base[path] 	= lpc_reg_base[band][path];
			pdev->lpc_reg_size 	= 16;
		}
    	pdev->nco_ram_base 	= nco_ram_base[band];
    	pdev->nco_ram_size 	= 1536;
		printf("%08x\n",pdev->nco_ram_base);
	}

	/* *********************************************************************************
	 * FPGA를 탐색하여 지원되는 Band와 SB를 찾아낸다. 그리고 이를 각 ics_dev에 등록한다.
	 *  2차 FPGA 정보 획득. 
	 *  1. 각 밴드 타입. 
	 ***********************************************************************************/
	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		pdev->bd_name = (char *)0;
		pdev->bd_id = band;
    	pdev->supported = 0;			

		if (isIcsBandAlive(pdev)) {
			pdev->bd_name = band_name[band];
			pdev->supported = 1;
  			getIcsBandType(pdev,&pdev->band_info.index, 
  								&pdev->band_info.band, 
  								&pdev->band_info.type, 
  								&pdev->band_info.desc, 
  								&pdev->band_info.firsel, 
  								&pdev->band_info.fa, 
  								&pdev->band_info.nco,
  								&pdev->band_info.rname);
		}
	}
	printf("%8s|"," ");
	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (!pdev->supported) {
			continue;
		}
  		printf("%10s|",pdev->bd_name);
  	}
	printf("\n");

	printf("%8s|","index");
	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (!pdev->supported) {
			continue;
		}
		printf("%10d|",pdev->band_info.index);
  	}
	printf("\n");

	printf("%8s|","band");
	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (!pdev->supported) {
			continue;
		}
		printf("%10s|",pdev->band_info.band);
  	}
	printf("\n");
	  
	printf("%8s|","type");
  	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (!pdev->supported) {
			continue;
		}
		printf("%9s%c|"," ", pdev->band_info.type);
  	}
	printf("\n");
    
	printf("%8s|","desc");
  	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (!pdev->supported) {
			continue;
		}
		printf("%10s|",pdev->band_info.desc);
  		//printf("desc  -> %s\n",pdev->band_info.firsel);
  		//printf("desc  -> %s\n",pdev->band_info.fa);
  		//printf("desc  -> %s\n",pdev->band_info.nco);
  	}
	printf("\n");
	/* *********************************************************************************
	 * //////////////// 파일에서 읽어들여야 하는 것들 //////////////////
	 * 각 ics_dev마다 파일이 생성된다. ics_dev.support=1인 SB에 대해서 파일을 생성한다. 
	 ***********************************************************************************/
	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (pdev->supported) {
			//if (loadIcsInfo(pdev) < 0) {
			//	saveIcsInfoDef(pdev);
			//	printf("create a new ics file[%s_]..\n",pdev->bd_name, pdev->sb_name);
			//}
			//else {
			//	printf("load %s_%s file done..\n",pdev->bd_name, pdev->sb_name);
			//}
		}
	}

	//Equalizer Setting
	{
		pdev = getIcsDev(0);
		if (pdev->band_info.type != 'e') {
//			loadCalCoeForce(pdev,UL,0,0);
			loadCalInfo(1); /// 0 Default Value
							/// 1 Table
		}
	}
	
#if 0
	{
		unsigned char			pl_mode,app_mode;
		getBootInfo((char *)&pl_mode, (char *)&app_mode);
		printf("pl_mode[%d],app_mode[%d]\n",pl_mode, app_mode);
		//Equalizer Setting
		if(pl_mode != 1)
		{
			pdev = getIcsDev(0);
			loadCalCoeForce(pdev,UL,0,0);
		}
	}
#endif
//	/*보드가 살면 우선적으로 FPGA에 Default로 되어있는 dly1 값 불러온다. */
//	if (isIcsSbAlive()) {
//		pdev->delay1 = getIcsDelay1(pdev);
//		if (strcmp(pdev->band_info.band, "900M") == 0){
//			if(strstr(pdev->band_info.desc, "WCDMA1FA")){
//				pdev->delay1 = 16;
//				//monrc파일에 DLY1_BAND21_W1FA이 있으면 이 값으로 한다. 
//				if(mon_getenv("DLY1_BAND21_W1FA"))
//					pdev->delay1 = strtoul(mon_getenv("DLY1_BAND21_W1FA"),0,0);    
//			} 
//			if(strstr(pdev->band_info.desc, "WCDMA2FA")){
//				pdev->delay1 = 17;
//				if(mon_getenv("DLY1_BAND21_W2FA"))
//					pdev->delay1 = strtoul(mon_getenv("DLY1_BAND21_W2FA"),0,0);    
//			} 
//		}
//	}
//	
//
//  	for (band=0;band<MAX_BAND;band++) {
//			pdev = getIcsDev(band,sb);
//			if (!pdev->supported) {
//				continue;
//			}
//			for (path=0;path<2;path++) {
//				setIcsAlcOnOff(pdev,path,pdev->alc_onoff[path]);
//				setIcsAlcSetp(pdev,path,pdev->alc_setp[path]);
//				//setIcsAlcSetp(pdev,path,-1500);
//    			//printf("alc setp[%d] : %d \n", path,getIcsAlcSetp(pdev,path));
//				setIcsUsrGain(pdev,path,pdev->usr_gain[path]);				
//				//setIcsFaThrLvl(pdev,path, pdev->fa_lvl[path]);
//				setIcsFaThrLvl(pdev,path, -1000);
//    			//printf("fa lvl[%d] : %d \n", path,getIcsFaThrLvl(pdev,path));
//				
//				pdev->lms_tget_def[path] = getLmsTgetDef(pdev,path);
//				pdev->lms_tget_run[path] = getLmsTgetRun(pdev,path);
//				pdev->lms_step_def[path] = getLmsStepDef(pdev,path);
//				pdev->lms_step_run[path] = getLmsStepRun(pdev,path);
//				pdev->lpc_step_def[path] = getLpcStepDef(pdev,path);
//				pdev->lpc_step_run[path] = getLpcStepRun(pdev,path);  			
//    	}
//    } 
//
////	for (path=0;path<2;path++) {
////		for (i=0;i<NUMB_FA;i++) {
////		////////////////////////// ICS 설정 /////////////////////////////
////			setIcsAttGain(pdev, path, i, pdev->att_gain[path][i]);
////			///setIcsUsrGain(pdev, path, i, pdev->usr_gain[path]);
////		}
////		//setIcsAlcOnOff(pdev, pdev->alc_onoff);
////		setIcsAgcOnOff(pdev, path, pdev->agc_onoff[path]);
////  }
////	//////////////// FPGA 에서 읽어들여야 하는 것들  ////////////////
////	pdev->unit_tap 		= getIcsUnitTap(pdev);
////	pdev->numb_tap 		= getIcsNumbTap(pdev);
////	//pdev->delay1		= getIcsDelay1(pdev);
////	pdev->delay2		= getIcsDelay2(pdev);
////	setIcsAgcSetp(pdev, pdev->agc_setp);
//
////	//////////////// 기본 설정 ////////////////
////	setIcsDelay1(pdev,pdev->delay1);
//
////	setIcsFreq(pdev, 0, pdev->nco_freq[0]);
////	setIcsFreq(pdev, 1, pdev->nco_freq[1]);
////	setIcsFreq(pdev, 2, pdev->nco_freq[2]);
////	setIcsFreq(pdev, 3, pdev->nco_freq[3]);
//
//  	for (band=0;band<MAX_BAND;band++) {
//			pdev = getIcsDev(band,sb);
//			if (!pdev->supported) {
//				continue;
//			}
//			setIcsNco(pdev,pdev->fc);
//    }
//
//  	for (band=0;band<MAX_BAND;band++) {
//  		for (sb=0;sb<1;sb++) {
//			pdev = getIcsDev(band,sb);
//			if (!pdev->supported) {
//				continue;
//			}
//			//setIcsAgcSetp(pdev, DL, pdev->agc_setp[DL]);
//			//setIcsAgcSetp(pdev, UL, pdev->agc_setp[UL]);
//			if(band < 3) {
//				setIcsAgcSetp(pdev, DL, -3400);
//				setIcsAgcSetp(pdev, UL, -2400);
//			} else {
//				//if(strcmp(pdev->band_info.desc,"   NR100M") == 0)	{ 
//					printf("NR Bandwidth:%s\n",pdev->band_info.desc);
//					setIcsAgcSetp(pdev, DL, -2400);
//					setIcsAgcSetp(pdev, UL, -2400);
//				//} else {
//				//	printf("NR Bandwidth:%s\n",pdev->band_info.desc);
//				//	setIcsAgcSetp(pdev, DL, -2000);
//				//	setIcsAgcSetp(pdev, UL, -2400);
//				//}
//			}
//				
//    	}
//    }
//
//	for (band=0;band<MAX_BAND;band++) {
//		pdev = getIcsDev(band,0);
//
//		pdev->ilc_onoff[0]		= getIcsIlcOnOff(pdev,band);
//		pdev->ics_onoff[0]		= getIcsOnOff(pdev,band);
//
//		setIcsIlcTget(pdev,-2900);
//		setIcsIlcXth(pdev,2000);
//		setIcsIlcOnOff(pdev,DL,1);
//		setIcsIlcOnOff(pdev,UL,1);
//		
//	}
//
//	pdev = getIcsDev(3,0);
//	/* 동기모듈 시험용 SG 로 부터 Maker 신호를 입력 받아, Tdd swtiching을 할 경우*/
//	//setMonTsyncOut(pdev, 0);	// 1 :Out En
//	//setTddMode(pdev, 1);		// 1 :Exten Mark Input mode,
//
//	/* 중계기 Demod에서 동기를 생성해서 사용할 경우와 TP로 생성된  T-sync 신호를 보기 위해서 내보기위 한성*/
//	setMonTsyncOut(pdev, 1);	// 1 :Out En
//	setTddMode(pdev, 0);		// 1 :Exten Mark Input mode,
//
////	//////////////// ILC 설정.  ////////////////
////	pdev = getIcsDev(3,0);
////	setIcsIlcTget(pdev,-2900);
////	setIcsIlcXth(pdev,2050);
////	setIcsIlcOnOff(pdev,DL,1);
////	setIcsIlcOnOff(pdev,UL,1);

	for (band=0;band<MAX_BAND;band++) {
		pdev = getIcsDev(band);
		if (!pdev->supported) {
			continue;
		}
		//if(band == 1) {
		//	setIcsNco(pdev,-200);
		//	printf("ICS nco set\n");
		//}
	}
	printf("ICS Initialized..!!\n");
}

