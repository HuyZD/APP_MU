//#include "xgpiops.h"
//#include "user.h"
//#include "xstatus.h"
//
//#define EMIO_GPIO_D	XPAR_XGPIOPS_0_DEVICE_ID
//XGpioPs EMIO_GPIO;
//XGpioPs_Config *ConfigPtr;
//
//u32 gpio_config(){
//	int Status;
//	ConfigPtr = XGpioPs_LookupConfig(EMIO_GPIO_D);
//
//	Status = XGpioPs_CfgInitialize(&EMIO_GPIO, ConfigPtr, ConfigPtr->BaseAddr);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	} else
//		return XST_SUCCESS;
//}
//
//u32 write_pin(u32 Pin, u32 Data){
//	XGpioPs_SetDirectionPin(&EMIO_GPIO, Pin, 1);
//	XGpioPs_SetOutputEnablePin(&EMIO_GPIO, Pin, 1);
//	XGpioPs_WritePin(&EMIO_GPIO, Pin, Data);
//	return Data;
//}
//
//u32 read_pin(u32 Pin){
//	return XGpioPs_ReadPin(&EMIO_GPIO, Pin);
//}
//
