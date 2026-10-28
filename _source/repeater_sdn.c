//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"
#include "tbl.h"
#include "protocol_private.h"

#define	SDN_NORMAL				0
#define SDN_RETRY1				1
#define	SDN_RETRY2_START		2 
#define	SDN_RETRY2				3
#define	SDN_RETRY3_START		4
#define	SDN_RETRY3				5
#define	SDN_RETRY4_START		6	
//#define SDN_RETRY4				7
//#define	SDN_RETRY5_START		8
//#define	SDN_RETRY5  			9
#define SHUTDOWN				6		//// 10   6
#define SDN_PERIOD				1					/* 1->1��, 2->0.5�� */


struct sdn_fsm {
//	unsigned int	state;
	signed int		state;	
	int				status;
	unsigned int	timer;
	unsigned int	recovery_timer;
	unsigned int 	OffReason;
	short			dsp_outpwr;		///DL
	short			rf_outpwr;		///UL
	short			iso_level;
	short			dsp_sdnlvl;
	short			rf_sdnlvl;
	short			iso_sdnlvl;
	unsigned char	rfmask;	
	unsigned char	isomask;	

};

static struct sdn_fsm sdn_fsm[4][2] = 				/* 2Path (������/������) Finite State Machine */
{	
	{
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		},
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		},
	},
	/////1
	{
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		},
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		},
	},
	
	/////2
	{
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		},
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			
		},
	},
	
	/////3
	{
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		},
		{
			SDN_NORMAL,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		},
	},
	
	/////4	
	
};			

void init_sdn(struct sdn_fsm *psdn)
{
	psdn->state	=	SDN_NORMAL;
	psdn->timer	=	0;
	psdn->recovery_timer	=	0;
}


uchar sdnStateCtrl(uchar band,uchar path,signed int val)   //path : DL/UL
{
	uchar state = 0;
	
//	sdn_fsm[0][path].state = val;
	sdn_fsm[path][band].state = val;

//	printf("sdnStateCtrl[%d][%d]>[%d]\n",path,val,sdn_fsm[path][band].state);
	
	return (state);
}


uchar sdnStatus(uchar path, uchar sb)
{
	uchar	retval;
	
	switch (sdn_fsm[path][sb].state) {
	case SDN_NORMAL : 
	case SDN_RETRY1 : 
	case SDN_RETRY2 : 
	case SDN_RETRY3 :
//	case SDN_RETRY4 :
		retval = 0;
		break;
	case SDN_RETRY2_START : 
	case SDN_RETRY3_START : 
//	case SDN_RETRY4_START : 
		retval = 1;
		break;
	case SHUTDOWN : 
		retval = 2;
		break;
	default:
		break;
	}
	
	return (retval);
}

/* runShutFsm()
 * 
 * RF Shutdown �� ������ ���� �����Ѵ�.											
 * 1. Output power �� SD Level ���� ���� ��Ȳ�� 10���̻� ���ӽ� Shutdown ��Ų��. DSP��°� RF ����� ������ ���ƾ� �Ѵ�. 	
 * 		DSP		RF
 * SDN	0		0	�̰���
 *		0		1	���Ǻ� ���� RF�� dsp_sdnlvl+3dB�� �Ѵ°�쿡��. 
 *		1		0	SDN����
 *		1		1	SDN����
 * 2. pll alarm �Ǵ� temp alarm �� �� �� 10���̻� ���� �� shutdown ��Ų��.   
 * 3. Shutdown�� �Ǹ� 30���Ŀ� �����ϰ� �ٽ� shutdown �˻縦 �Ѵ�.				
 * 4. 2ȸ Shutdown�� �Ǹ� 30���Ŀ� �����ϰ� �ٽ� shutdown �˻縦 �Ѵ�.							 		    			
 * 5. 3ȸ Shutdown�� �Ǹ� ���� shutdown �ȴ�.										
 * 6. ���� shutdown�� �Ǹ� ��� ������ ON/OFF �ϱ� ������ RF ����� OFF ���¸� �����Ѵ�
 * 7. shutdown�� �����ǰ� 5�а� �������ϸ�, shutdown ���ӹ߻�Ƚ���� 0�� �ȴ�.
 *	retval
 *		0 : hold
 *		1 : off
 *		2 : on
 *

 */



//// Alarm Check 10Sec
////  After 30 seconds of Shutdown
static int runShutFsm(struct sdn_fsm *psdn, char hpa_onoff, uchar pll_alarm, uchar temp_alarm, uchar location_Alarm)
{
	struct repeater_struct	*prpt;

	int retval=0;
	
	int SDN_Timer;
	int SDN_RETRY3_Timer;
	int recovery_Timer;
	static uint8_t	OffReason;
	static uint8_t	OffTime;

	prpt = getRptDev();

	SDN_Timer 			= SDN_PERIOD*10;		///10Sec
	SDN_RETRY3_Timer 	= SDN_PERIOD*30;	///30Sec
	recovery_Timer 		= SDN_PERIOD*10;		///10Sec

#if DEBUG_PRINT
//	printf("psdn->state[%d]>[%d][%d]\n",psdn->state,psdn->timer,psdn->recovery_timer);
#endif

	switch(psdn->state) {
	case SDN_NORMAL: 				   				/* ���� ���� */
		if(!hpa_onoff) {                			/* hpa ���� off�� Nomal ���·� ����*/			
			psdn->state = SDN_NORMAL;
			break;
		}		
//		else if(psdn->dsp_outpwr >= psdn->dsp_sdnlvl || psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level <= psdn->iso_sdnlvl || pll_alarm==1 ) {
		else if(((psdn->rf_outpwr >= psdn->rf_sdnlvl)&&(psdn->rfmask == _On))|| ((psdn->iso_level <= psdn->iso_sdnlvl)&&(psdn->isomask == _On)) || pll_alarm==1 ) {
			psdn->state = SDN_RETRY1;
			break;
		}
		psdn->timer = 0;
		psdn->recovery_timer = 0;
		break;

	case SDN_RETRY1:									/* 10�ʰ� ���ӿ���  �˻� �� , ù��° Shut down */
		if(!hpa_onoff) {
			retval 		= 3;							/*hpa ���� off�� Nomal ���·� ����*/	
			psdn->state = SDN_NORMAL;
			psdn->recovery_timer = 0;
			break;			
		}													
//		else if(psdn->dsp_outpwr >= psdn->dsp_sdnlvl || psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level <= psdn->iso_sdnlvl || pll_alarm==1 ) {
//		else if((psdn->rf_outpwr >= psdn->rf_sdnlvl) || psdn->iso_level <= psdn->iso_sdnlvl || pll_alarm==1 ) {
		else if(((psdn->rf_outpwr >= psdn->rf_sdnlvl)&&(psdn->rfmask == _On))|| ((psdn->iso_level <= psdn->iso_sdnlvl)&&(psdn->isomask == _On)) || pll_alarm==1) {
			if(psdn->timer >= SDN_Timer) {		
				retval 		= 1;
				psdn->state = SDN_RETRY2_START;
				psdn->timer = 0;
				///
#if DEBUG_PRINT
//				printf("1 rf_outpwr[%d][%d]>[%d]: pll_alarm[%d] \n",psdn->rf_outpwr,psdn->rf_sdnlvl,psdn->state,pll_alarm);
#endif
				break;
			}
			psdn->recovery_timer = 0;
			psdn->timer++;
		} else {
			if(psdn->recovery_timer >= recovery_Timer) { 		// �������� ���� �Ǿ��� ���,
				retval 		= 2;								// 5�� �� ���� �����̸� Shut down Ƚ�� �ʱ�ȭ
				psdn->state = SDN_NORMAL;
				psdn->recovery_timer = 0;
 				break;
			}
			psdn->timer = 0;
			psdn->recovery_timer++;
		}
		break;

	case SDN_RETRY2_START:										//retry1�� shutdown �� ���� �ܰ�. 30�� ���� HPA ON �� �ٽ� �˻�
		if (psdn->timer >= SDN_RETRY3_Timer) {
			retval 		= 2;
			psdn->state = SDN_RETRY2;
			psdn->timer = 0;
			break;
		}
		psdn->timer++;
		break;

	case SDN_RETRY2:
		if(!hpa_onoff) {
			retval 		= 3;
			psdn->state = SDN_NORMAL;
			psdn->recovery_timer = 0;
			break;			
		}		  												// �ι�° ���� 10�ʰ� ���ӿ��� �˻���, �ι�° Shut dwon		
//		else if(psdn->dsp_outpwr >= psdn->dsp_sdnlvl || psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level <= psdn->iso_sdnlvl || pll_alarm==1) {	
//		else if(psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level <= psdn->iso_sdnlvl || pll_alarm==1) {	
		else if(psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level < psdn->iso_sdnlvl || pll_alarm==1) {	

#if DEBUG_PRINT
//			printf("2 rf_outpwr[%d][%d]>[%d]: pll_alarm[%d] \n",psdn->rf_outpwr,psdn->rf_sdnlvl,psdn->state,pll_alarm);
#endif


			if(psdn->timer >= SDN_Timer){					// 10 �ʰ� ���ӿ���  �˻�
				retval 		= 1;
				psdn->state = SDN_RETRY3_START;
				psdn->timer = 0;
				
				break;
			}
			psdn->recovery_timer = 0;
			psdn->timer++;
			//printf("SDN_RETRY2_RUN \n");
		} else {
			if(psdn->recovery_timer >= recovery_Timer) { 		// �������� ���� �Ǿ��� ���,
				retval 		= 2;								// 5�� �� ���� �����̸� Shut down Ƚ�� �ʱ�ȭ
				psdn->state = SDN_NORMAL;
				psdn->recovery_timer = 0;
				
//				OffReason = CASE_User;
				break;
			}
			psdn->timer = 0;
			psdn->recovery_timer++;
			//printf("SDN_RETRY2_RECOVERY \n");
		}
		break;

	case SDN_RETRY3_START:											/* retry1�� shutdown �� ���� �ܰ�. 30��  ���� HPA ON �� �ٽ� �˻� */
		if (psdn->timer >= SDN_RETRY3_Timer) {
			retval 		= 2;
			psdn->state = SDN_RETRY3;
			psdn->timer = 0;

			break;
		}			
		psdn->timer++;
		break;

	case SDN_RETRY3:  							// ����° ���� 10�ʰ� ��������, ����°(����) Shut down
		if(!hpa_onoff) {
			retval 		= 3;
			psdn->state = SDN_NORMAL;
			psdn->recovery_timer = 0;
			
			break;			
		}											
//		else if(psdn->dsp_outpwr >= psdn->dsp_sdnlvl || psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level <= psdn->iso_sdnlvl || pll_alarm==1 || temp_alarm==1) {
//		else if(psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level <= psdn->iso_sdnlvl || pll_alarm==1 ) {
		else if(psdn->rf_outpwr >= psdn->rf_sdnlvl || psdn->iso_level < psdn->iso_sdnlvl || pll_alarm==1 ) {

#if DEBUG_PRINT
//		printf("3 rf_outpwr[%d][%d]>[%d]: pll_alarm[%d] \n",psdn->rf_outpwr,psdn->rf_sdnlvl,psdn->state,pll_alarm);
#endif

			if(psdn->timer >= SDN_Timer){	// 10 �ʰ� ���ӿ���  �˻�
				retval 		= 1;
				psdn->state = SHUTDOWN;//
				psdn->timer = 0;
				
				if(psdn->rf_outpwr >= psdn->rf_sdnlvl)
				{
					psdn->OffReason = 1;

				}

				else if(psdn->iso_level <= psdn->iso_sdnlvl)
				{
					psdn->OffReason = 2;

				}
				else if(pll_alarm)
				{
					psdn->OffReason = 3;
				}
			
				psdn->OffReason = OffReason;

				break;
			}
			psdn->recovery_timer = 0;                                    
			psdn->timer++;
		} else {
			if(psdn->recovery_timer >= recovery_Timer) { 			// �������� ���� �Ǿ��� ���,
				retval 		= 2;									// 5�� �� ���� �����̸� Shut down Ƚ�� �ʱ�ȭ
				psdn->state = SDN_NORMAL;
				psdn->recovery_timer = 0;
//				OffReason = CASE_User;
				break;
			}
			psdn->timer = 0;
			psdn->recovery_timer++;
		}
		break;

	case SHUTDOWN:

//		if(psdn->OffReason == 4)
//		printf("3 dsp_outpwr[%d][%d]rf_outpwr[%d][%d]>[%d]: pll_alarm[%d] \n",psdn->dsp_outpwr,psdn->dsp_sdnlvl,psdn->rf_outpwr,psdn->rf_sdnlvl,psdn->state,pll_alarm);

		retval 		= -1;
		
		break;
	default:
		break;
	}
	return (retval);
}

//static int dl_init_sdn = 0;
//static int ul_init_sdn = 0;

ushort 				dl_shutdown_alarm[4]={0,0,0,0};
ushort 				ul_shutdown_alarm[4]={0,0,0,0};

#define TEMP_SHUTDOWN_LEVEL		90

/* runShutDown() : 5���� �ѹ��� ���� - �� 0.5�ʿ� �ѹ�.
 */

//confFwdHpaOn(2, prpt->sys_dl_hpa_onoff[2]);
void ShutDown_AmpOnOff(int band, int val)
{


 }

 
 /////1. PLL Unlock, Output OverPower, ISO ShutDown  locationAlarm	--> complete shutdown
 //// Temp SD, OAM , PLMN 
void runShutDown(struct repeater_struct *pdev)
{


}


void sdn_init(uchar band)
{
	init_sdn(&sdn_fsm[DL][band]);
	init_sdn(&sdn_fsm[UL][band]);
}

