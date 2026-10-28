#include "FreeRTOS.h"
#include "semphr.h"
#include "xil_exception.h"
#include "xparameters.h"
#include "xqspips.h"
#include "xscugic.h"
#include <stdio.h>


#include "cal.h"
#include "config.h"
#include "flash.h"
#include "printf.h"
#include "repeater.h"
#include "tbl.h"
#include "uart.h"

#include "protocol_private.h"

#define QSPI_DEVICE_ID XPAR_XQSPIPS_0_DEVICE_ID
#define QSPI_INT_IRQ_ID XPAR_XQSPIPS_0_INTR

static XQspiPs QspiInstance;
// static XQspiPs flashSpi;

volatile static int TransferInProgress;
static int ErrorCount;

static u8 ReadBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 4];
static u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

static xSemaphoreHandle xFlashMutex;

static void FlashLock() {
  if (xFlashMutex == NULL) {
    xFlashMutex = xSemaphoreCreateMutex();
  }
  xSemaphoreTake(xFlashMutex, portMAX_DELAY);
}

static void FlashUnlock() { xSemaphoreGive(xFlashMutex); }

extern struct tbl_temp_info_struct tinfo;
extern struct hpa_info_struct hinfo;
extern struct tbl_cal_info_struct cinfo;

int init_flash(void) 
{
  int Status;
  XQspiPs_Config *ConfigPtr;

  /*
   * Initialize the QSPI driver so that it's ready to use.
   */
  ConfigPtr = XQspiPs_LookupConfig(QSPI_DEVICE_ID);
  if (ConfigPtr == NULL) {
    return XST_DEVICE_NOT_FOUND;
  }

  Status = XQspiPs_CfgInitialize(&QspiInstance, ConfigPtr, ConfigPtr->BaseAddress);
  if (Status != XST_SUCCESS) {
    printf("QSPI LookupConfig failed!\n\r");
    return XST_FAILURE;
  }

  /* Perform a self-test to check hardware build */
  Status = XQspiPs_SelfTest(&QspiInstance);
  if (Status != XST_SUCCESS) {
    printf("QSPI SelfTest failed! Status = %d\n\r", Status);
    return XST_FAILURE;
  }

  /*
   * Connect the QSPI driver to the interrupt subsystem.
   */
  Status = xPortInstallInterruptHandler(QSPI_INT_IRQ_ID, (XInterruptHandler)XQspiPs_InterruptHandler,(void *)&QspiInstance);
  if (Status != pdPASS) { // FreeRTOSe?�� pdPASS e��?i��?
    printf("Failed to install QSPI interrupt handler\n\r");
    return XST_FAILURE;
  }

  vPortEnableInterrupt(QSPI_INT_IRQ_ID);

  /*
   * Setup the handler for the QSPI that will be called from the interrupt
   * context when a transfer of data completes or an error occurs.
   */
  XQspiPs_SetStatusHandler(&QspiInstance, &QspiInstance,(XQspiPs_StatusHandler)SpiHandler);

  /*
   * Set Manual Start and Manual Chip select options.
   */
  Status = XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_MANUAL_START_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /* Set the operating clock frequency using the clock divider */
  Status = XQspiPs_SetClkPrescaler(&QspiInstance, XQSPIPS_CLK_PRESCALE_8);
  if (Status != XST_SUCCESS) {
    printf("SetClkPrescaler failed!\n\r");
    return XST_FAILURE;
  }

  /* Assert the FLASH chip select */
  Status = XQspiPs_SetSlaveSelect(&QspiInstance);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /* Diagnostic: Read Flash ID to verify communication */
  u8 id[3];
  Status = SpiFlashReadID(&QspiInstance, id);
  if (Status == XST_SUCCESS) {
    printf("Flash initialized. FlashID=0x%02x 0x%02x 0x%02x\n\r", id[0], id[1],id[2]);
  } else {
    printf("Flash initialized but ID read FAILED!\n\r");
  }

  return XST_SUCCESS;
}

int SpiFlashWriteEnable(XQspiPs *SpiPtr) ////spifWriteEnable
{
  int Status;
  u8 WriteEnableCmd = COMMAND_WRITE_ENABLE;

  /*
   * Wait while the Flash is busy.
   */
  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Initiate the Transfer.
   */
  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, &WriteEnableCmd, NULL, 1);

  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Wait till the Transfer is complete.
   */
  while (TransferInProgress);

  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

int SpiFlashWrite(XQspiPs *SpiPtr, u32 Addr, u32 ByteCount, u8 WriteCmd,u8 *data) 
{
  u32 Index;
  int Status;

  /*
   * ***** Set Extended Address *****
   */
  SpiFlashWriteEnable(SpiPtr);

  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  WriteBuffer[0] = COMMAND_EXTENDED_ADDRESS_WRITE;
  WriteBuffer[1] = (u8)(Addr >> 24);

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, NULL, 2);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  /*
   * ***** Write Data *****
   */
  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  SpiFlashWriteEnable(SpiPtr);

  /*
   * Prepare the WriteBuffer.
   */
  WriteBuffer[0] = WriteCmd;
  WriteBuffer[1] = (u8)(Addr >> 16);
  WriteBuffer[2] = (u8)(Addr >> 8);
  WriteBuffer[3] = (u8)Addr;

  for (Index = 0; Index < ByteCount; Index++) {
    WriteBuffer[Index + 4] = data[Index];
  }

  /*
   * Initiate the Transfer.
   */
  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, NULL, ByteCount + 4);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

int SpiFlashRead(XQspiPs *SpiPtr, u32 Addr, u32 ByteCount, u8 ReadCmd, u8 *buf) 
{
  int Status;

  /*
   * ***** Set Extended Address *****
   */
  SpiFlashWriteEnable(SpiPtr);

  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  WriteBuffer[0] = COMMAND_EXTENDED_ADDRESS_WRITE;
  WriteBuffer[1] = (u8)(Addr >> 24);

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, NULL, 2);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress) ;
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  /*
   * ***** Read Data *****
   */
  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Prepare the WriteBuffer.
   */
  WriteBuffer[0] = ReadCmd;
  WriteBuffer[1] = (u8)(Addr >> 16);
  WriteBuffer[2] = (u8)(Addr >> 8);
  WriteBuffer[3] = (u8)Addr;

  /*
   * Initiate the Transfer.
   */
  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, ReadBuffer, ByteCount + 4);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  /* Copy received data back to user buffer */
  for (int i = 0; i < ByteCount; i++) {
    buf[i] = ReadBuffer[i + 4];
  }

  return XST_SUCCESS;
}

int SpiFlashBulkErase(XQspiPs *SpiPtr) //// spifChipErase
{
  int Status;
  u8 BulkEraseCmd = COMMAND_BULK_ERASE;

  Status = SpiFlashWriteEnable(SpiPtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, &BulkEraseCmd, NULL, 1);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress) ;
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

int SpiFlashSectorErase(XQspiPs *SpiPtr, u32 Addr) 
{
  int Status;

  SpiFlashWriteEnable(SpiPtr);

  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  WriteBuffer[0] = COMMAND_EXTENDED_ADDRESS_WRITE;
  WriteBuffer[1] = (u8)(Addr >> 24);

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, NULL, 2);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  Status = SpiFlashWriteEnable(SpiPtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  WriteBuffer[0] = COMMAND_SECTOR_ERASE;
  WriteBuffer[1] = (u8)(Addr >> 16);
  WriteBuffer[2] = (u8)(Addr >> 8);
  WriteBuffer[3] = (u8)Addr;

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, NULL, 4);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

int SpiFlashSubSectorErase(XQspiPs *SpiPtr, u32 Addr) /// spifBlockErase 4KB
{
  int Status;

  SpiFlashWriteEnable(SpiPtr);

  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  WriteBuffer[0] = COMMAND_EXTENDED_ADDRESS_WRITE;
  WriteBuffer[1] = (u8)(Addr >> 24);

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, NULL, 2);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  Status = SpiFlashWriteEnable(SpiPtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  Status = SpiFlashWaitForFlashReady();
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  WriteBuffer[0] = COMMAND_SUBSECTOR_ERASE;
  WriteBuffer[1] = (u8)(Addr >> 16);
  WriteBuffer[2] = (u8)(Addr >> 8);
  WriteBuffer[3] = (u8)Addr;

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, WriteBuffer, NULL, 4);
  //	printf("1 SpiFlashSubSectorErase[%x][%d]\n\r",Addr,Status);

  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

int SpiFlashGetStatus(XQspiPs *SpiPtr) 
{
  int Status;
  u8 ReadStatusCmd[] = {COMMAND_STATUSREG_READ, 0};

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, ReadStatusCmd, ReadBuffer, 2);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

int SpiFlashGlobalUnprotect(void) 
{
  int Status;
  u8 StatusReg;
  u8 WriteStatusCmd[2];

  Status = SpiFlashGetStatus(&QspiInstance);
  if (Status != XST_SUCCESS) {
    return Status;
  }

  StatusReg = ReadBuffer[1];

  // If already unprotected, just return
  if ((StatusReg & 0x5C) == 0) {
    return XST_SUCCESS;
  }

  SpiFlashWriteEnable(&QspiInstance);

  WriteStatusCmd[0] = COMMAND_WRITE_STATUS;
  WriteStatusCmd[1] = StatusReg & ~(0x5C); // Clear BP0-BP3

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(&QspiInstance, WriteStatusCmd, NULL, 2);
  if (Status != XST_SUCCESS) {
    return Status;
  }

  while (TransferInProgress);

  return SpiFlashWaitForFlashReady();
}

int SpiFlashGlobalProtect(void) 
{
  int Status;
  u8 StatusReg;
  u8 WriteStatusCmd[2];

  Status = SpiFlashGetStatus(&QspiInstance);
  if (Status != XST_SUCCESS) {
    return Status;
  }

  StatusReg = ReadBuffer[1];

  SpiFlashWriteEnable(&QspiInstance);

  WriteStatusCmd[0] = COMMAND_WRITE_STATUS;
  WriteStatusCmd[1] = StatusReg | 0x5C; // Set BP0-BP3

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(&QspiInstance, WriteStatusCmd, NULL, 2);
  if (Status != XST_SUCCESS) {
    return Status;
  }

  while (TransferInProgress);

  return SpiFlashWaitForFlashReady();
}

int SpiFlashWaitForFlashReady(void) /// spifWaitForReady
{
  int Status;
  u8 StatusReg;
  volatile int timeout = 500000;

  while (timeout > 0) {
    Status = SpiFlashGetStatus(&QspiInstance);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }

    StatusReg = ReadBuffer[1];
    if ((StatusReg & 0x01) == 0) {
      break;
    }
    timeout--;
  }

  if (timeout == 0) {
    printf("SpiFlashWaitForFlashReady timeout\n");
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

void SpiHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount) 
{
  TransferInProgress = FALSE;

  if (StatusEvent != XST_SPI_TRANSFER_DONE) {
    ErrorCount++;
  }
}

int SpiFlashReadID(XQspiPs *SpiPtr, u8 *id) 
{
  int Status;
  u8 ReadIdCmd = COMMAND_READ_ID;

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(SpiPtr, &ReadIdCmd, ReadBuffer, 4);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  id[0] = ReadBuffer[1];
  id[1] = ReadBuffer[2];
  id[2] = ReadBuffer[3];

  return XST_SUCCESS;
}

/* ====================== ������ FlashReadID ====================== */
int FlashReadID(void) 
{
  int Status;

  u8 ReadIDCmd[4] = {COMMAND_READ_ID, 0x00, 0x00, 0x00}; // dummy bytes
  //	printf("0 FlashReadID \n\r");

  TransferInProgress = TRUE;
  Status = XQspiPs_Transfer(&QspiInstance, ReadIDCmd, ReadBuffer, 4); // Manufacturer + Memory Type + Capacity
  if (Status != XST_SUCCESS) {
    printf("1 FlashReadID Error \n\r");

    return XST_FAILURE;
  }

  while (TransferInProgress);
  if (ErrorCount != 0) {
    ErrorCount = 0;
    return XST_FAILURE;
  }

  printf("Flash ID = 0x%02X 0x%02X 0x%02X\r\n", ReadBuffer[1], ReadBuffer[2], ReadBuffer[3]);

  return XST_SUCCESS;
}

#if 1
int QSPI_Read(uint32_t addr, void *buf, uint32_t len) {
  int rc;
  FlashLock();
  rc = SpiFlashRead(&QspiInstance, addr, len, COMMAND_RANDOM_READ, buf);
  FlashUnlock();
  return rc;

#if 0
    uint8_t Cmd[5];
    Cmd[0] = 0x6B;                    // Quad Output Read (4-byte addr mode i�ҡ�i����)
    Cmd[1] = (addr >> 24) & 0xFF;
    Cmd[2] = (addr >> 16) & 0xFF;
    Cmd[3] = (addr >> 8)  & 0xFF;
    Cmd[4] = addr & 0xFF;

    return XQspiPs_PolledTransfer(&QspiInstance, Cmd, buf, len + 5);  // Command + Dummy + Data
#endif
}

int QSPI_Erase(uint32_t addr) {
  // Sector Erase (4KB) e??e?�� Bulk Erase ? i?��i?��e����e?�� 64KB/256KB Block Erase
  // e�Ҩ�i?�� i?��e����i?��e?�� e��?e?��i?? i??i���� i??i?�� Erase (i?��e����i??i?��e?�� Block
  // e?��i��?e���� e���i??)
  //    uint8_t Cmd[5] = {0xD8, (addr>>24)&0xFF, (addr>>16)&0xFF,
  //    (addr>>8)&0xFF, addr&0xFF}; uint8_t Cmd[4] = {COMMAND_SUBSECTOR_ERASE,
  //    (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF};

  //    uint8_t WREN = 0x06;

  //    XQspiPs_PolledTransfer(&QspiInstance, &WREN, NULL, 1);
  //    return XQspiPs_PolledTransfer(&QspiInstance, Cmd, NULL, 5);
  SpiFlashSectorErase(&QspiInstance, addr);
}

int QSPI_Write(uint32_t addr, const void *buf, uint32_t len) 
{
  int rc;
  FlashLock();
  SpiFlashGlobalUnprotect();
  SpiFlashSubSectorErase(&QspiInstance, addr);

  SpiFlashWriteEnable(&QspiInstance);

  rc = SpiFlashWrite(&QspiInstance, addr, len, COMMAND_PAGE_PROGRAM, (u8 *)buf);
  SpiFlashGlobalProtect();
  FlashUnlock();
  return rc;
}

//	SpiFlashWrite(&QspiInstance, &QspiInstance, len, COMMAND_PAGE_PROGRAM,
//buf);

#if 0
	// Page Program (�ǹ������� 256Byte ���� Loop ����)
    // ���⼭�� ���� ����
    uint8_t WREN = 0x06;
    XQspiPs_PolledTransfer(&QspiInstance, &WREN, NULL, 1);

    uint8_t Cmd[5] = {0x32, (addr>>24)&0xFF, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF}; // Quad Page Program
    // �����δ� Command + Data�� �Բ� �����ؾ� �� (�����ϴ� �ʿ� �� ����ȭ)

    return XQspiPs_PolledTransfer(&QspiInstance, Cmd, (u8*)buf, len + 5);
#endif


int QSPI_Write_Byte(uint32_t addr, uint8_t data, uint32_t len) 
{
  QSPI_Erase(addr); // Sector Erase
  usleep(40000);
  return QSPI_Write(addr, &data, len);
}

#endif

/***********************************************************************************************
 * ************ ********************************************
 * ************         Info Structure
 * ********************************************
 * ************ ********************************************
 * *********************************************************************************************
 */
void FlashSubSectorErase() {
  SpiFlashSubSectorErase(&QspiInstance, SPIF_FILE_SAVE_BASE);
}

u8 rx_info_buf[SUBSECTOR_SIZE];
u8 tx_info_buf[SUBSECTOR_SIZE];
u8 info_buf[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

void FlashTest() {
  SpiFlashWriteEnable(&QspiInstance);
  for (int k = 0; k < PAGE_SIZE; k++) {
    tx_info_buf[k] = k;
  }

  SpiFlashWrite(&QspiInstance, SPIF_FILE_SAVE_BASE, 256, COMMAND_PAGE_PROGRAM,tx_info_buf);
  SpiFlashRead(&QspiInstance, SPIF_FILE_SAVE_BASE, 256, COMMAND_RANDOM_READ,rx_info_buf);

  for (int k = 0; k < PAGE_SIZE; k++) {
    printf("[%d] ", rx_info_buf[k]);
  }
  printf("\n\r");
}




/***********************************************************************************************
 * ************ ********************************************
 * ************         	Download
 * ********************************************
 * ************ ********************************************
 * *********************************************************************************************
 */

#define APP_FLASH_BASE SPIF_APPX_IMAGE_BASE
u32 flash_pos = SPIF_APPX_IMAGE_BASE;

u8 syncword_sector[256];

void __FlashErase(int _Addr, unsigned int _size) {

  int rc, i, addr;
  int size;

  //	addr = _Addr;
  size = _size; /// 64KByte

  for (i = 0, addr = _Addr; i < size; i++) {

    rc = SpiFlashSectorErase(&QspiInstance, addr);

    if (rc < 0) {
      printf("1 SpiFlashSectorErase(0x%lx) returned %d\n", addr, rc);
      break;
    }
    addr += 0x10000;
  }
}

u8 info_buf_X[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];
u8 info_buf_Y[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

void Download_load(int _Addr, unsigned int size, u8 *buf) {

  SemaphoreHandle_t xFlashMutex;
  xFlashMutex = xSemaphoreCreateMutex();

  int addr;
  int iRemain;
  int iWLen, iOffset;
  addr = _Addr;

  printf("1 Download_load(0x%lx) [%d]\n\r", _Addr, size);
  iRemain = size;
  iOffset = 0;

  //	if (xSemaphoreTake(xFlashMutex, portMAX_DELAY) == pdTRUE) {

  while (iRemain > 0) 
  {
    if (iRemain > 256)
      iWLen = 256;
    else
      iWLen = iRemain;

    SpiFlashRead(&QspiInstance, addr + iOffset, PAGE_SIZE, COMMAND_RANDOM_READ, buf + iOffset);

    iRemain -= iWLen;
    iOffset += iWLen;
  }
  //		xSemaphoreGive(xFlashMutex);
  //	}

  printf("read ok\n\r");
}

void Download_write(u32 FlashAddr, unsigned int size, u8 *buf, u8 mode) {

	int rc, i, addr;

	int iRemain;
	int iWLen, iOffset;
	unsigned char aucWBuf[256];
	unsigned short xCrc_gen;
	unsigned char footer[12]; /* 12 bytes total */

  	printf("2 Download_write addr(0x%lx)size[%d]\n", FlashAddr, size);
	FlashLock();
	SpiFlashGlobalUnprotect();

	for (i = 0, addr = FlashAddr; i < (SPIF_APP_IMAGE_SIZE / 0x10000); i++) {
		rc = SpiFlashSectorErase(&QspiInstance, addr);
		if (rc < 0) {
		//     printf("1 SpiFlashSectorErase(0x%lx) returned %d\n", addr, rc);
		break;
		}
		addr += 0x10000;
	}

	iRemain = size;
	iOffset = 0;

	addr = FlashAddr;

	while (iRemain > 0) 
	{
		if (iRemain > 256)
		  iWLen = 256;
		else
		  iWLen = iRemain;

		SpiFlashWriteEnable(&QspiInstance);

		memcpy(aucWBuf, buf + iOffset, iWLen);

		SpiFlashWrite(&QspiInstance, addr + iOffset, PAGE_SIZE,COMMAND_PAGE_PROGRAM, aucWBuf);

		iRemain -= iWLen;
		iOffset += iWLen;

		//		printf("3 flash_pos(0x%lx)iRemain(%d) iOffset
		//%d\n",flash_pos,iRemain,iOffset);
	}
	if(mode == 1)
	{
		xCrc_gen = Generate_CRC((char *)buf, size);
		footer[0] = ((size >> 0) & 0xff);
		footer[1] = ((size >> 8) & 0xff);
		footer[2] = ((size >> 16) & 0xff);
		footer[3] = ((size >> 24) & 0xff);

		footer[4] = ((xCrc_gen >> 0) & 0xff);/// CRC: 2 bytes
		footer[5] = ((xCrc_gen >> 8) & 0xff);/// CRC: 2 bytes
		footer[6] = 0;///Version: 2 bytes
		footer[7] = 0;///Version: 2 bytes
		footer[8] = 0x01; ///Version: 2 bytes
		footer[9] = 0x00; /// CRC: 2 bytes
		footer[10] = 0x00;
		footer[11] = 0x00;

		SpiFlashWrite(&QspiInstance, (unsigned long)(addr + SPIF_APP_IMAGE_SIZE - 12), 12, COMMAND_PAGE_PROGRAM, footer);
	}
	SpiFlashGlobalProtect();
	FlashUnlock();

	printf("x write ok(0x%lx)size[%d]xCrc_gen[0x%x]\n\r", addr, size, xCrc_gen);
}


void FPGA_Download_write(u32 FlashAddr, unsigned int size, u8 *buf, u8 mode) {

	int rc, i, addr;

	int iRemain;
	int iWLen, iOffset;
	unsigned char aucWBuf[256];
	unsigned short xCrc_gen;
	unsigned char footer[12]; /* 12 bytes total */

  	printf("2 FPGA Download_write addr(0x%lx)size[%d]\n", FlashAddr, size);
	FlashLock();
	SpiFlashGlobalUnprotect();

	for (i = 0, addr = FlashAddr; i < (SPIF_PL_IMAGE_SIZE / 0x10000); i++) {
		rc = SpiFlashSectorErase(&QspiInstance, addr);
		if (rc < 0) {
		//     printf("1 SpiFlashSectorErase(0x%lx) returned %d\n", addr, rc);
			rc = SpiFlashSectorErase(&QspiInstance, addr);
		break;
		}
		addr += 0x10000;
		usleep(1000);
	}

	iRemain = size;
	iOffset = 0;

	addr = FlashAddr;

	while (iRemain > 0) 
	{
		if (iRemain > 256)
		  iWLen = 256;
		else
		  iWLen = iRemain;

		SpiFlashWriteEnable(&QspiInstance);

		memcpy(aucWBuf, buf + iOffset, iWLen);

		SpiFlashWrite(&QspiInstance, addr + iOffset, PAGE_SIZE,COMMAND_PAGE_PROGRAM, aucWBuf);

		iRemain -= iWLen;
		iOffset += iWLen;

		//		printf("3 flash_pos(0x%lx)iRemain(%d) iOffset
		//%d\n",flash_pos,iRemain,iOffset);
	}

	xCrc_gen = Generate_CRC((char *)buf, size);

	SpiFlashGlobalProtect();
	FlashUnlock();
//	xCrc_gen = Generate_CRC((char *)buf, size);

	printf("x write ok(0x%lx)size[%d]xCrc_gen[0x%x]\n\r", addr, size, xCrc_gen);
}

void Serial_Download_write(u32 FlashAddr, unsigned int size, u8 *buf, u8 type) {

	int rc, i, addr;

	int iRemain;
	int iWLen, iOffset;
	unsigned char aucWBuf[256];
	unsigned short xCrc_gen;
	unsigned char footer[12]; /* 12 bytes total */
	u32 flashsize = 0;
	unsigned int tmp_size;

  	printf("2 Download_write addr(0x%lx)size[%d]\n", FlashAddr, size);
	FlashLock();
	SpiFlashGlobalUnprotect();

	if(type == FILETYPE_FW_MODEM)
	{
		flashsize = SPIF_PL_IMAGE_SIZE;
	}
	else if(type == FILETYPE_FW_MB)
	{
		flashsize = SPIF_APP_IMAGE_SIZE;
	}
	else if(type == FILETYPE_FPGA)
	{
		flashsize = SPIF_PL_IMAGE_SIZE;
	}
	else if(type == FILETYPE_PL_Y)
	{
		flashsize = SPIF_PL_IMAGE_SIZE;
	}
	else if(type == FILETYPE_uMon)
	{
		flashsize = SPIF_APP_IMAGE_SIZE;
	}
	else if(type == FILETYPE_ALL)
	{
		if(size < SPIF_UMON_IMAGE_SIZE)
		{
			flashsize = SPIF_UMON_IMAGE_SIZE;
			FlashAddr = SPIF_UMON_IMAGE_BASE;
		}
		else if(size <= SPIF_APP_IMAGE_SIZE)
		{
			flashsize = SPIF_APP_IMAGE_SIZE;
			FlashAddr = SPIF_APPZ_IMAGE_BASE;
		}
		else if(size <= SPIF_PL_IMAGE_SIZE)
		{
			flashsize = SPIF_PL_IMAGE_SIZE;
			FlashAddr = SPIF_PLZ_IMAGE_BASE;
		}
		else if(size == ((SPIF_APP_IMAGE_SIZE*2)+(SPIF_PL_IMAGE_SIZE*2)))
		{
			flashsize = ((SPIF_APP_IMAGE_SIZE*2)+(SPIF_PL_IMAGE_SIZE*2));
			FlashAddr = SPIF_APP_IMAGE_SIZE;
		}
	}

	for (i = 0, addr = FlashAddr; i < (flashsize / 0x10000); i++) {	//0x060000
		rc = SpiFlashSectorErase(&QspiInstance, addr);
		if (rc < 0) {
		//     printf("1 SpiFlashSectorErase(0x%lx) returned %d\n", addr, rc);
		break;
		}
		addr += 0x10000;
	}

	iRemain = size;
	iOffset = 0;

	addr = FlashAddr;

	while (iRemain > 0) 
	{
		if (iRemain > 256)
		  iWLen = 256;
		else
		  iWLen = iRemain;

		SpiFlashWriteEnable(&QspiInstance);

		memcpy(aucWBuf, buf + iOffset, iWLen);

		SpiFlashWrite(&QspiInstance, addr + iOffset, PAGE_SIZE,COMMAND_PAGE_PROGRAM, aucWBuf);

		iRemain -= iWLen;
		iOffset += iWLen;

		//		printf("3 flash_pos(0x%lx)iRemain(%d) iOffset
		//%d\n",flash_pos,iRemain,iOffset);
	}

	SpiFlashGlobalProtect();
	FlashUnlock();

	printf("u write ok(0x%lx)size[%d]\n\r", FlashAddr, size);
}

void GUIDownload_complte() {
  flash_pos = APP_FLASH_BASE;

  SpiFlashWriteEnable(&QspiInstance);
  SpiFlashWrite(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_PAGE_PROGRAM,syncword_sector); // 2 : previous data frame size (buf[0]~[1] = curFrame)

  printf("5 GUIDownload_complte \n");
}

/***********************************************************************************************
 * ************ ********************************************
 * ************         	Xmodem
 * ********************************************
 * ************ ********************************************
 * *********************************************************************************************
 */

#define USE_CRC
static unsigned char Down[Xmodem1K_Packet_Size];

int SerialDownload(void) {
  unsigned long i, j;
  unsigned char ch, Control = SendReqChar;
  volatile unsigned long delayCnt = 0;
  unsigned short CheckVal = 0, RxCheckVal;
  unsigned long DataSize, Size = 0;
  u32 flash_pos = APP_FLASH_BASE;
#if DEBUG_PRINT
  printf("\n Please X-modem setting\n");
#endif
  while (1) {
    uartWrite(UART_CLI, Control);

    if (Control == ACK) {
      while (UART0_empty()) ;
      ch = uartRead(UART_CLI);
    } else {
      delayCnt = 0;
      while (UART0_empty()) {
        if (delayCnt++ > 500000)
          break;
      }
      if (delayCnt < 500000) {
        ch = uartRead(UART_CLI);
      }
    }

    if (ch == EOT) {
      uartWrite(UART_CLI, ACK);

      break;
    }

    if (ch == CAN) {
#if DEBUG_PRINT
      printf("\n!!!  Transfer Send File Canceled !!!\n");
#endif
      return 0;
    }

    if ((ch == SOH) || (ch == STX)) {
      DataSize = Xmodem1K_Packet_Size;

      // Block number ignore
      while (UART0_empty()) ;
      ch = uartRead(UART_CLI);
      while (UART0_empty()) ;
      ch = uartRead(UART_CLI);

      // Get Rx Data
      for (i = 0; i < DataSize; i++) {
        while (UART0_empty());
        ch = uartRead(UART_CLI);
        Down[i] = ch;
        Size++;
      }

      // get Data fault Check Data
      while (UART0_empty())  ;
      ch = uartRead(UART_CLI);
      RxCheckVal = ch << 8;

      while (UART0_empty()) ;
      ch = uartRead(UART_CLI);
      RxCheckVal |= ch;

#ifdef USE_CRC // CRC Calcuration
      for (i = 0; i < DataSize; i++)
        CheckVal = updatecrc16(CheckVal, Down[i]);
#else // CheckSum Calcuration
      for (i = 0; i < DataSize; i++)
        CheckVal += Down[i];
#endif
#if 0
			if (CheckVal == RxCheckVal)
			{

				if (flash_pos == APP_FLASH_BASE) {
					SpiFlashSectorErase(&QspiInstance, flash_pos);
				}
				else if (!(flash_pos % 0x10000)) {
					SpiFlashSectorErase(&QspiInstance, flash_pos);
				}

				for(int j = 0; j < 4 ; j++){

					SpiFlashWriteEnable(&QspiInstance);
					SpiFlashWrite(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_PAGE_PROGRAM, &Down[256*j]);
					flash_pos += 256;
				}

				Control = ACK;		 // Request Next Block
			}
			else
			{
				Control = NAK;

			}
			CheckVal = 0;
#endif
    }
  }

  return Size;
}

extern xTaskHandle xRepeaterHandle;

int dn_ser(int argc, char *argv[]) {
  //	if(argc < 2){
  //		printf("xmodem -option (-F : FPGA, -a : Application)
  //section\n");
  //
  //		return -1;
  //	}

  int size = 0;

  if ((size = SerialDownload()) > 0) {
#if DEBUG_PRINT
    printf("\nDownload OK\n");
    printf("Download Size : %dByte\n", size);
#endif
  }
#if DEBUG_PRINT
  else
    printf("\nDownload Fail\n");
#endif
  return 1;
}

unsigned short updatecrc16(unsigned short crc, unsigned char ser_data) {
  // Update the CRC for transmitted and received data using
  // the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1).

  crc = (unsigned char)(crc >> 8) | (crc << 8);
  crc ^= ser_data;
  crc ^= (unsigned char)(crc & 0xff) >> 4;
  crc ^= (crc << 8) << 4;
  crc ^= ((crc & 0xff) << 4) << 1;

  return crc;
}

// #define APP_MEMORY_BASE				0x00000050
// #define APP_SOFTWARE_SIZE   		0x3F000
//
// u8 appData_buf[2 + READ_WRITE_EXTRA_BYTES];
//
// static void spiFlash_f2m(u32 flashAddress, u32 memoryAddress, u32 size){
//	uint32_t flash_pos;
//	uint8_t *ram;
//
//	flash_pos = flashAddress;
//	ram = (uint8_t *)memoryAddress;
//
//	for(int i = 0; i < size; i++){
//		SpiFlashRead(&QspiInstance, flash_pos, 1, COMMAND_RANDOM_READ,
//appData_buf);
//		*(ram + i) = *(appData_buf + READ_WRITE_EXTRA_BYTES);
//		flash_pos++;
//	}
// }
//
// void ExecApp(){
//	void (*Jump_To_Application)();
//
//	printf("ExecApp\n\n");
//
//	spiFlash_f2m(APP_FLASH_BASE, APP_MEMORY_BASE, APP_SOFTWARE_SIZE);
//
//	printf("Jump to Application!!\n");
//
//	Jump_To_Application = (void (*)())APP_MEMORY_BASE;
//
//	Jump_To_Application();
// }

#define UMON_BASE_ADDRESS 0x80000000
#define RS_CP2102N_INT_IRQ_ID XPAR_INTC_0_UARTLITE_0_VEC_ID

void cmdReboot() {
  void (*Jump_To_uMON)();

  Jump_To_uMON = (void (*)())UMON_BASE_ADDRESS;

  Jump_To_uMON();
}

/***********************************************************************************************
 * ************ ********************************************
 * ************         	Table
 * ********************************************
 * ************ ********************************************
 * *********************************************************************************************
 */

#define TEMP_TBL_FLASH_ADDRESS SPIF_TEMP_TABLE_BASE;
#define HPA_TBL_FLASH_ADDRESS SPIF_AMP_ONOFF_BASE;
#define CAL_TBL_FLASH_ADDRESS SPIF_EQUAL_TABLE_BASE;

u8 tbl_buf[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];

void saveTempTbl(struct tbl_temp_info_struct *tinfo) {
  FlashLock();
  SpiFlashGlobalUnprotect();
  uint32_t flash_pos;
  uint8_t *tbl_pos;
  int size;
  uint8_t sector_num = 1;

  flash_pos = TEMP_TBL_FLASH_ADDRESS;
  tbl_pos = (u8 *)tinfo;

  size = sizeof(struct tbl_temp_info_struct);
  printf("0 size[%d]\n", size);
#if 0
  for (;;) {
    if (size < SUBSECTOR_SIZE * sector_num) {
      break;
    }
    sector_num++;
  }
#endif
//for (int i = 0; i < sector_num; i++) 
  {
    SpiFlashSubSectorErase(&QspiInstance, flash_pos);

    for (int j = 0; j < SUBSECTOR_SIZE / PAGE_SIZE; j++) {

      SpiFlashWriteEnable(&QspiInstance);

      if (size > PAGE_SIZE) {
        for (int k = 0; k < PAGE_SIZE; k++) {
          *(tbl_buf + k) = *(tbl_pos + k);
        }

        SpiFlashWrite(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_PAGE_PROGRAM,tbl_buf);
        flash_pos += PAGE_SIZE;
        tbl_pos += PAGE_SIZE;
        size -= PAGE_SIZE;
      } else {
        for (int k = 0; k < size; k++) {
          *(tbl_buf + k) = *(tbl_pos + k);
        }
        SpiFlashWrite(&QspiInstance, flash_pos, size, COMMAND_PAGE_PROGRAM, tbl_buf);
        break;
      }
    }
  }
  SpiFlashGlobalProtect();
  FlashUnlock();
}

void loadTempTbl(struct tbl_temp_info_struct *tinfo) {
  uint32_t flash_pos;
  uint8_t *tbl_pos;
  int size;
  flash_pos = TEMP_TBL_FLASH_ADDRESS;
  tbl_pos = (u8 *)tinfo;

  size = sizeof(struct tbl_temp_info_struct);

  for (;;) {
    if (size > PAGE_SIZE) {
      SpiFlashRead(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_RANDOM_READ, tbl_buf);

      for (int i = 0; i < PAGE_SIZE; i++) {
        *(tbl_pos + i) = *(tbl_buf + i);
      }

      flash_pos += PAGE_SIZE;
      tbl_pos += PAGE_SIZE;
      size -= PAGE_SIZE;
    } else {
      SpiFlashRead(&QspiInstance, flash_pos, size, COMMAND_RANDOM_READ,tbl_buf);
      for (int i = 0; i < size; i++) {
        *(tbl_pos + i) = *(tbl_buf + i);
      }
      break;
    }
  }
}

void saveHpaTbl(struct hpa_info_struct *tinfo) {
  FlashLock();
  SpiFlashGlobalUnprotect();
  uint32_t flash_pos;
  uint8_t *tbl_pos;
  int size;
  uint8_t sector_num = 1;

  flash_pos = HPA_TBL_FLASH_ADDRESS;
  tbl_pos = (u8 *)tinfo;

  size = sizeof(struct hpa_info_struct);
#if 0
  for (;;) {
    if (size < SUBSECTOR_SIZE * sector_num) {
      break;
    }
    sector_num++;
  }
#endif
//for (int i = 0; i < sector_num; i++) 
  {
    SpiFlashSubSectorErase(&QspiInstance, flash_pos);

    for (int j = 0; j < SUBSECTOR_SIZE / PAGE_SIZE; j++) {

      SpiFlashWriteEnable(&QspiInstance);

      if (size > PAGE_SIZE) {
        for (int k = 0; k < PAGE_SIZE; k++) {
          *(tbl_buf + k) = *(tbl_pos + k);
        }

        SpiFlashWrite(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_PAGE_PROGRAM,tbl_buf);
        flash_pos += PAGE_SIZE;
        tbl_pos += PAGE_SIZE;
        size -= PAGE_SIZE;
      } else {
        for (int k = 0; k < size; k++) {
          *(tbl_buf + k) = *(tbl_pos + k);
        }
        SpiFlashWrite(&QspiInstance, flash_pos, size, COMMAND_PAGE_PROGRAM,tbl_buf);
        break;
      }
    }
  }
  SpiFlashGlobalProtect();
  FlashUnlock();
}

void loadHpaTbl(struct hpa_info_struct *tinfo) {
  uint32_t flash_pos;
  uint8_t *tbl_pos;
  int size;
  flash_pos = HPA_TBL_FLASH_ADDRESS;
  tbl_pos = (u8 *)tinfo;

  size = sizeof(struct hpa_info_struct);

  for (;;) {
    if (size > PAGE_SIZE) {
      SpiFlashRead(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_RANDOM_READ,tbl_buf);

      for (int i = 0; i < PAGE_SIZE; i++) {
        *(tbl_pos + i) = *(tbl_buf + i);
      }

      flash_pos += PAGE_SIZE;
      tbl_pos += PAGE_SIZE;
      size -= PAGE_SIZE;
    } else {
      SpiFlashRead(&QspiInstance, flash_pos, size, COMMAND_RANDOM_READ, tbl_buf);

      for (int i = 0; i < size; i++) {
        *(tbl_pos + i) = *(tbl_buf + i);
      }
      break;
    }
  }
}

void saveCalTbl(struct tbl_cal_info_struct *tinfo) {
  FlashLock();
  SpiFlashGlobalUnprotect();
  uint32_t flash_pos;
  uint8_t *tbl_pos;
  int size;
  uint8_t sector_num = 1;

  flash_pos = CAL_TBL_FLASH_ADDRESS;
  tbl_pos = (u8 *)tinfo;

  size = sizeof(struct tbl_cal_info_struct);
#if 0
  for (;;) {
    if (size < SUBSECTOR_SIZE * sector_num) {
      break;
    }
    sector_num++;
  }
#endif
//for (int i = 0; i < sector_num; i++) 
  {
    SpiFlashSubSectorErase(&QspiInstance, flash_pos);

    for (int j = 0; j < SUBSECTOR_SIZE / PAGE_SIZE; j++) {

      SpiFlashWriteEnable(&QspiInstance);

      if (size > PAGE_SIZE) {
        for (int k = 0; k < PAGE_SIZE; k++) {
          *(tbl_buf + k) = *(tbl_pos + k);
        }

        SpiFlashWrite(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_PAGE_PROGRAM,tbl_buf);
        flash_pos += PAGE_SIZE;
        tbl_pos += PAGE_SIZE;
        size -= PAGE_SIZE;
      } else {
        for (int k = 0; k < size; k++) {
          *(tbl_buf + k) = *(tbl_pos + k);
        }
        SpiFlashWrite(&QspiInstance, flash_pos, size, COMMAND_PAGE_PROGRAM,tbl_buf);
        break;
      }
    }
  }
  SpiFlashGlobalProtect();
  FlashUnlock();
}

void loadCalTbl(struct tbl_cal_info_struct *tinfo) {
  uint32_t flash_pos;
  uint8_t *tbl_pos;
  int size;
  flash_pos = CAL_TBL_FLASH_ADDRESS;
  tbl_pos = (u8 *)tinfo;

  size = sizeof(struct tbl_cal_info_struct);

  for (;;) {
    if (size > PAGE_SIZE) {
      SpiFlashRead(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_RANDOM_READ,tbl_buf);

      for (int i = 0; i < PAGE_SIZE; i++) {
        *(tbl_pos + i) = *(tbl_buf + i);
      }

      flash_pos += PAGE_SIZE;
      tbl_pos += PAGE_SIZE;
      size -= PAGE_SIZE;
    } else {
      SpiFlashRead(&QspiInstance, flash_pos, size, COMMAND_RANDOM_READ, tbl_buf);
      for (int i = 0; i < size; i++) {
        *(tbl_pos + i) = *(tbl_buf + i);
      }
      break;
    }
  }
}
//extern struct ics_info 				pinfo;
extern struct ics_info 				iinfo;

#if 1
void saveICSTbl(struct ics_info *pinfo) {
	FlashLock();
	SpiFlashGlobalUnprotect();
	uint32_t flash_pos;
	uint8_t *tbl_pos;
	int size;
	uint8_t sector_num = 1;

	flash_pos = SPIF_ICS_INFO_BASE;
	tbl_pos = (u8 *)pinfo;

	size = sizeof(struct tbl_cal_info_struct);
#if 0
	for (;;) {
	if (size < SUBSECTOR_SIZE * sector_num) {
	break;
	}
	sector_num++;
	}
#endif
	//for (int i = 0; i < sector_num; i++) 
	{
		SpiFlashSubSectorErase(&QspiInstance, flash_pos);

		for (int j = 0; j < SUBSECTOR_SIZE / PAGE_SIZE; j++) {

			SpiFlashWriteEnable(&QspiInstance);

			if (size > PAGE_SIZE) {
			for (int k = 0; k < PAGE_SIZE; k++) {
			*(tbl_buf + k) = *(tbl_pos + k);
			}

			SpiFlashWrite(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_PAGE_PROGRAM,tbl_buf);
			flash_pos += PAGE_SIZE;
			tbl_pos += PAGE_SIZE;
			size -= PAGE_SIZE;
			} else {
				for (int k = 0; k < size; k++) {
				*(tbl_buf + k) = *(tbl_pos + k);
				}
				SpiFlashWrite(&QspiInstance, flash_pos, size, COMMAND_PAGE_PROGRAM,tbl_buf);
				break;
			}
		}
	}
	SpiFlashGlobalProtect();
	FlashUnlock();
}

void loadICSTbl(struct ics_info *pinfo) {
	uint32_t flash_pos;
	uint8_t *tbl_pos;
	int size;
	flash_pos = SPIF_ICS_INFO_BASE;
	tbl_pos = (u8 *)pinfo;

	size = sizeof(struct tbl_cal_info_struct);

	for (;;) {
		if (size > PAGE_SIZE) {
			SpiFlashRead(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_RANDOM_READ,tbl_buf);

		for (int i = 0; i < PAGE_SIZE; i++) {
		*(tbl_pos + i) = *(tbl_buf + i);
		}

			flash_pos += PAGE_SIZE;
			tbl_pos += PAGE_SIZE;
			size -= PAGE_SIZE;
		} else {
			SpiFlashRead(&QspiInstance, flash_pos, size, COMMAND_RANDOM_READ, tbl_buf);
			for (int i = 0; i < size; i++) {
			*(tbl_pos + i) = *(tbl_buf + i);
			}
			break;
		}
	}
}

#endif

void loadInfoTbl(void) {
  unsigned char pl_settings = 0xff;
  unsigned char app_settings = 0xff;
  int rc_read;

  rc_read = QSPI_Read(SPIF_PL_INFO_BASE, (char *)&pl_settings, 1);
  rc_read = QSPI_Read(SPIF_APP_INFO_BASE, (char *)&app_settings, 1);
  printf("0 pl_settings: 0x%02x,pl_settings: 0x%02x\n", pl_settings,app_settings);
}


void BootInfoTbl(unsigned char pl_val,unsigned char app_val){

	u8 buf[10]= {0,};

	unsigned char pl_settings = 0xff;
	unsigned char app_settings = 0xff;
	int rc_read;

	printf("2 pl_settings: 0x%02x,pl_settings: 0x%02x\n", pl_val,app_val);

	buf[0] = pl_val;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = app_val;
	buf[5] = 0x00;
	buf[6] = 0x00;
	buf[7] = 0x00;
	
	QSPI_Write(SPIF_PL_INFO_BASE,buf,8);

	rc_read = QSPI_Read(SPIF_PL_INFO_BASE, (char *)&pl_settings, 1);
	rc_read = QSPI_Read(SPIF_APP_INFO_BASE, (char *)&app_settings, 1);
	printf("3 pl_settings: 0x%02x,pl_settings: 0x%02x\n", pl_settings,app_settings);

}


int getBootInfo(char *pl_mode, char *app_mode)
{          
	unsigned char pl_settings = 0xff;
	unsigned char app_settings = 0xff;
	int rc_read;

	rc_read = QSPI_Read(SPIF_PL_INFO_BASE, (char *)&pl_settings, 1);
	rc_read = QSPI_Read(SPIF_APP_INFO_BASE, (char *)&app_settings, 1);
	printf("0 pl_settings: 0x%02x,app_settings: 0x%02x\n", pl_settings,app_settings);

	*pl_mode = pl_settings;
	*app_mode = app_settings;

    return rc_read;
}   



void CrcInfoTbl(int addr, unsigned int size,unsigned short xCrc_gen){

	u8 buf[15]= {0,};

	unsigned char pl_settings = 0xff;
	unsigned char app_settings = 0xff;
	int rc_read;

	printf("1 pl_settings: 0x%02x,size[%d],pl_settings: 0x%02x\n", addr,size,xCrc_gen);


	buf[0] = ((size >> 0) & 0xff);
	buf[1] = ((size >> 8) & 0xff);
	buf[2] = ((size >> 16) & 0xff);
	buf[3] = ((size >> 24) & 0xff);

	buf[4] = ((xCrc_gen >> 0) & 0xff);/// CRC: 2 bytes
	buf[5] = ((xCrc_gen >> 8) & 0xff);/// CRC: 2 bytes
	buf[6] = 0;///Version: 2 bytes
	buf[7] = 0;///Version: 2 bytes
	buf[8] = 0x01; ///Version: 2 bytes
	buf[9] = 0x00; /// CRC: 2 bytes
	buf[10] = 0x00;
	buf[11] = 0x00;

 	QSPI_Write((unsigned long)(addr + SPIF_APP_IMAGE_SIZE - 12),buf,12);
}



#if 0
void CrcInfoTbl(int Addr, nsigned int size,unsigned short xCrc_gen){
	u8 buf[15]= {0,};

#if 1
	buf[0] = ((size >> 0) & 0xff);
	buf[1] = ((size >> 8) & 0xff);
	buf[2] = ((size >> 16) & 0xff);
	buf[3] = ((size >> 24) & 0xff);

	buf[4] = ((xCrc_gen >> 0) & 0xff);/// CRC: 2 bytes
	buf[5] = ((xCrc_gen >> 8) & 0xff);/// CRC: 2 bytes
	buf[6] = 0;///Version: 2 bytes
	buf[7] = 0;///Version: 2 bytes
	buf[8] = 0x01; ///Version: 2 bytes
	buf[9] = 0x00; /// CRC: 2 bytes
	buf[10] = 0x00;
	buf[11] = 0x00;



	SpiFlashWrite(&QspiInstance, (unsigned long)(Addr + SPIF_APP_IMAGE_SIZE - 12), 12, COMMAND_PAGE_PROGRAM, buf);
#endif
}

#endif

void saveinfo(struct repeater_info *pinfo) {


    FlashLock();
    SpiFlashGlobalUnprotect();

    uint32_t flash_pos;
    uint8_t *tbl_pos;
    int size;

    flash_pos = SPIF_FILE_SAVE_BASE;
    tbl_pos = (uint8_t *)pinfo;

    size = sizeof(struct repeater_info);

    /* Erase 4 KB subsector */
    SpiFlashSubSectorErase(&QspiInstance, flash_pos);

    for (int j = 0; j < SUBSECTOR_SIZE / PAGE_SIZE; j++) {

        if (size <= 0)
            break;

        SpiFlashWriteEnable(&QspiInstance);

        if (size > PAGE_SIZE) {

            for (int k = 0; k < PAGE_SIZE; k++) {
                *(info_buf + k) = *(tbl_pos + k);
            }

            SpiFlashWrite(&QspiInstance,
                          flash_pos,
                          PAGE_SIZE,
                          COMMAND_PAGE_PROGRAM,
						  info_buf);

            flash_pos += PAGE_SIZE;
            tbl_pos += PAGE_SIZE;
            size -= PAGE_SIZE;
        }
        else {

            for (int k = 0; k < size; k++) {
                *(info_buf + k) = *(tbl_pos + k);
            }

            SpiFlashWrite(&QspiInstance,
                          flash_pos,
                          size,
                          COMMAND_PAGE_PROGRAM,
						  info_buf);

            break;
        }
    }

    SpiFlashGlobalProtect();
    FlashUnlock();

}

void loadinfo(struct repeater_info *pinfo) {

  uint32_t flash_pos;
  uint8_t *info_pos;
  int size;

  FlashLock();

  flash_pos = SPIF_FILE_SAVE_BASE;
  info_pos = (u8 *)pinfo;

  size = sizeof(struct repeater_info);

  for (;;) {
    if (size > PAGE_SIZE) {
      SpiFlashRead(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_RANDOM_READ,info_buf);

      for (int i = 0; i < PAGE_SIZE; i++) {
        *(info_pos + i) = *(info_buf + i);
      }

      flash_pos += PAGE_SIZE;
      info_pos += PAGE_SIZE;
      size -= PAGE_SIZE;
    } else {
      SpiFlashRead(&QspiInstance, flash_pos, size, COMMAND_RANDOM_READ,info_buf);

      for (int i = 0; i < size; i++) {
        *(info_pos + i) = *(info_buf + i);
      }
      break;
    }
  }

  FlashUnlock();
}

void save_main_info(struct main_ctrl_mu_struct *pinfo) {


    FlashLock();
    SpiFlashGlobalUnprotect();

    uint32_t flash_pos;
    uint8_t *tbl_pos;
    int size;

    flash_pos = SPIF_FILE_SAVE_BASE;
    tbl_pos = (uint8_t *)pinfo;

    size = sizeof(struct main_ctrl_mu_struct);

    /* Erase 4 KB subsector */
    SpiFlashSubSectorErase(&QspiInstance, flash_pos);

    for (int j = 0; j < SUBSECTOR_SIZE / PAGE_SIZE; j++) {

        if (size <= 0)
            break;

        SpiFlashWriteEnable(&QspiInstance);

        if (size > PAGE_SIZE) {

            for (int k = 0; k < PAGE_SIZE; k++) {
                *(info_buf + k) = *(tbl_pos + k);
            }

            SpiFlashWrite(&QspiInstance,
                          flash_pos,
                          PAGE_SIZE,
                          COMMAND_PAGE_PROGRAM,
						  info_buf);

            flash_pos += PAGE_SIZE;
            tbl_pos += PAGE_SIZE;
            size -= PAGE_SIZE;
        }
        else {

            for (int k = 0; k < size; k++) {
                *(info_buf + k) = *(tbl_pos + k);
            }

            SpiFlashWrite(&QspiInstance,
                          flash_pos,
                          size,
                          COMMAND_PAGE_PROGRAM,
						  info_buf);

            break;
        }
    }

    SpiFlashGlobalProtect();
    FlashUnlock();

}
void load_main_info(struct main_ctrl_mu_struct *pinfo) {

  uint32_t flash_pos;
  uint8_t *info_pos;
  int size;

  uint32_t check_val = 0;

  FlashLock();
  flash_pos = SPIF_FILE_SAVE_BASE;

//  SpiFlashRead(&QspiInstance, flash_pos, 4, COMMAND_RANDOM_READ, (u8 *)&check_val);
//  if (check_val == 0xFFFFFFFF) {
//      FlashUnlock();
//      return 0;
//  }


  info_pos = (uint8_t *)pinfo;

  size = sizeof(struct main_ctrl_mu_struct);

  for (;;) {
    if (size > PAGE_SIZE) {
      SpiFlashRead(&QspiInstance, flash_pos, PAGE_SIZE, COMMAND_RANDOM_READ,info_buf);

      for (int i = 0; i < PAGE_SIZE; i++) {
        *(info_pos + i) = *(info_buf + i);
      }

      flash_pos += PAGE_SIZE;
      info_pos += PAGE_SIZE;
      size -= PAGE_SIZE;
    } else {
      SpiFlashRead(&QspiInstance, flash_pos, size, COMMAND_RANDOM_READ,info_buf);

      for (int i = 0; i < size; i++) {
        *(info_pos + i) = *(info_buf + i);
      }
      break;
    }
  }

  FlashUnlock();

}
