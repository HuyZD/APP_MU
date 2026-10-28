/*	repeater_led.c
 *	Repeater API
 */
//#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "repeater.h"
#include "protocol_private.h"

#if INCLUDE_HW_LED
#define INIT_PERIOD			10



void run_led_alarm(struct repeater_struct *pdev)
{
	unsigned short 		input_level = 0x0;
	unsigned short 		output_level = 0x0;
	unsigned short 		iso_level = 0x0;
	static	char		alarm = 0, alarm_trg = 0;
	static  char		green_alarm = 0, green_alarm_trg = 0;
	static  int         init_cnt = 0;
	static  int         install_cnt = 0;  
	static  char		trg = 0;
	static char			init_sts = 1;
	char				normal_status = 0;				
	int 				i;
	static int         ncnt = 0;
	static int         band = 0;
	int         _band = 0;

	if(init_sts){

		setLedPwrGrnOnOff(trg);
		setLedB1GrnOnOff(trg);
		setLedB3GrnOnOff(trg);
		setLedN78GrnOnOff(trg);

		trg = !trg;
		init_cnt++;   // 초기 부팅중 (initialize)
#if DEBUG_PRINT
		printf("0 init_sts[%d][%d]\n",init_sts,init_cnt);
#endif
			
		if(init_cnt >INIT_PERIOD){       
//            if(normal_status)
			{   //test용 원래 normal_status = 1
            	init_cnt = 0;
           	 	init_sts = 0;
			}
		}
		init_sts = 0;
	}
	else{
//		printf("1 init_sts[%d][%d][%d]>[%d]\n",init_sts,band,ncnt,trg); 	
		
		/*Power LED*/	
		setLedPwrGrnOnOff(1);
		setLedPwrRedOnOff(0);
		{
			trg = !trg;

			{
				_band = B3;

				setLedB1GrnOnOff(0);
				setLedB1RedOnOff(0);

				setLedN78GrnOnOff(1);
				setLedN78RedOnOff(0);

				if(pdev->sys_dl_alarm_total[_band])
				{
					setLedB3GrnOnOff(0);
					setLedB3RedOnOff(trg);
				}
				else
				{
					setLedB3GrnOnOff(1);
					setLedB3RedOnOff(0);
				}

			}
			{
				_band = B1;

				setLedB3GrnOnOff(0);
				setLedB3RedOnOff(0);


				if(pdev->sys_dl_alarm_total[_band])
				{
					setLedB1GrnOnOff(0);
					setLedB1RedOnOff(trg);
				}
				else
				{
					setLedB1GrnOnOff(1);
					setLedB1RedOnOff(0);
				}


				setLedN78GrnOnOff(0);
				setLedN78RedOnOff(0);
				
			}
			{
				_band = N78;


				setLedB3GrnOnOff(0);
				setLedB3RedOnOff(0);

				setLedB1GrnOnOff(0);
				setLedB1RedOnOff(0);
				
				
				if(pdev->sys_dl_alarm_total[_band])
				{
					setLedN78GrnOnOff(0);
					setLedN78RedOnOff(trg);
				}
				else
				{
					setLedN78GrnOnOff(1);
					setLedN78RedOnOff(0);
				}

			}				
			{
				if((band == 0)) ///B3
				{
					_band = B3;
					if(pdev->sys_dl_alarm_total[_band])
					{
						setLedB3GrnOnOff(0);
						setLedB3RedOnOff(trg);
					}
					else
					{
						setLedB3GrnOnOff(1);
						setLedB3RedOnOff(0);
					}


					setLedB1GrnOnOff(0);
					setLedB1RedOnOff(0);
					
					setLedN78GrnOnOff(0);
					setLedN78RedOnOff(0);
					//band 
					
				
					if(ncnt++ > 4)
					{
						ncnt = 0;
						band = 1;
					}
				}
				else if((band == 1)) ///B1
				{
					//band 
					_band = B1;

					setLedB3GrnOnOff(0);
					setLedB3RedOnOff(0);
					

					if(pdev->sys_dl_alarm_total[_band])
					{
						setLedB1GrnOnOff(0);
						setLedB1RedOnOff(trg);
					}
					else
					{
						setLedB1GrnOnOff(1);
						setLedB1RedOnOff(0);
					}
					
					setLedN78GrnOnOff(0);
					setLedN78RedOnOff(0);


					if(ncnt++ > 4)
					{
						ncnt = 0;
						band = 2;
					}				
				}
				else if((band == 2)) ///N78
				{
					//band 
					_band = N78;

					setLedB3GrnOnOff(0);
					setLedB3RedOnOff(0);

					setLedB1GrnOnOff(0);
					setLedB1RedOnOff(0);
					
					if(pdev->sys_dl_alarm_total[_band])
					{
						setLedN78GrnOnOff(0);
						setLedN78RedOnOff(trg);
					}
					else
					{
						setLedN78GrnOnOff(1);
						setLedN78RedOnOff(0);
					}


					if(ncnt++ > 4)
					{
						ncnt = 0;
						band = 0;
					}	
				}

			}
		}
	}	
}	


void run_led_init(struct repeater_struct *pdev)
{

	static  int         init_cnt = 0;
	static  char		trg = 0;
	static char			init_sts = 1;


	if(init_sts){
		setLedUserRedOnOff(trg);

		trg = !trg;
		init_cnt++;

		//if(init_cnt >INIT_PERIOD){
		if(init_cnt >40){
            init_cnt = 0;
           	init_sts = 0;
		}
		
	}
}

#endif
