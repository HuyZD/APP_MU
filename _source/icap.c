//#include <xparameters.h>
//#include <xstatus.h>
//#include <xil_types.h>
//#include <xil_assert.h>
//#include <xhwicap_i.h>
//#include <xhwicap_l.h>
//#include <xhwicap.h>
//
//
//#define HWICAP_DEVICEID         XPAR_HWICAP_0_DEVICE_ID
//
//#define HWICAP_BASEADDR         XPAR_HWICAP_0_BASEADDR
//
//#define HWICAP_EXAMPLE_BITSTREAM_LENGTH     8
//
//static XHwIcap HwIcap;
//
//static uint32_t ReadId[8] =
//{
//	0xFFFFFFFF, /* Dummy Word */
//	0xAA995566, /* Sync Word*/
//	0x20000000, /* Type 1 NO OP */
//	0x30020001, /* Write WBSTAR cmd */
//	0x01000000, /* Warm boot start address (Load the desired address) */
//	0x30008001, /* Write CMD */
//	0x0000000F, /* Write IPROG */
//	0x20000000, /* Type 1 NO OP  */
//};
//
//uint8_t isFallback(){
//	XHwIcap_Config *CfgPtr;
//	u32 ConfigRegData;
//
//	CfgPtr = XHwIcap_LookupConfig(HWICAP_DEVICEID);
//
//	XHwIcap_CfgInitialize(&HwIcap, CfgPtr, CfgPtr->BaseAddress);
//
//	HwIcap.WordsPerFrame = 93;
//	HwIcap.BytesPerFrame = 372;
//	HwIcap.IsReady = XIL_COMPONENT_IS_READY;
//
//	XHwIcap_Reset(&HwIcap);
//
//	XHwIcap_GetConfigReg(&HwIcap, XHI_BOOTSTS, (u32 *)&ConfigRegData);
//
//	return (ConfigRegData & 0x2 ? 1 : 0);
//}
//
//void set_IPROG(uint32_t iprog_address)
//{
//	u32 Index;
//
//	ReadId[4] = iprog_address; //Load the Warm Boot Address
//
//	for (Index = 0; Index < 8; Index++) {
//		XHwIcap_WriteReg(HWICAP_BASEADDR, XHI_WF_OFFSET, ReadId[Index]);
//	}
//
//	XHwIcap_WriteReg(HWICAP_BASEADDR, XHI_CR_OFFSET, XHI_CR_WRITE_MASK);
//}
//
////void cmdReboot()
////{
////	u32 Index;
////
////	ReadId[4] = 0x00800000;
////
////	for (Index = 0; Index < 8; Index++) {
////		XHwIcap_WriteReg(HWICAP_BASEADDR, XHI_WF_OFFSET, ReadId[Index]);
////	}
////
////	XHwIcap_WriteReg(HWICAP_BASEADDR, XHI_CR_OFFSET, XHI_CR_WRITE_MASK);
////}
//
//void cmdReboot()
//{
//	void (*Jump_To_Application)();
//
//	Jump_To_Application = (void (*)())0x80000000;
//
//	Jump_To_Application();
//}
