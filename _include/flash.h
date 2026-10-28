/*
 * boot.h
 *
 *  Created on: 2023. 4. 14.
 *      Author: LJH
 */

#ifndef __FLASH_SPI_H__
#define __FLASH_SPI_H__

#include "xqspips.h"

#define USE_CRC

#define SendReqChar	'C'

#define SOH		0x01
#define STX		0x02
#define NAK		0x15
#define ACK		0x06
#define EOT		0x04
#define CAN		0x18

#define Xmodem_Packet_Size		128
#define Xmodem1K_Packet_Size	1024
#define Xmodem_Max_Retrans      12

/* Flash Commands */
#define COMMAND_PAGE_PROGRAM		0x02
#define COMMAND_RANDOM_READ			0x03
#define COMMAND_WRITE_DISABLE		0x04
#define COMMAND_STATUSREG_READ		0x05
#define COMMAND_WRITE_ENABLE		0x06
#define COMMAND_SECTOR_ERASE		0xD8
#define COMMAND_SUBSECTOR_ERASE		0x20	///4KB
#define COMMAND_BULK_ERASE			0xC7
#define COMMAND_READ_ID				0x9F
#define COMMAND_WRITE_STATUS		0x01
#define COMMAND_EXTENDED_ADDRESS_WRITE	0xC5
#define COMMAND_EXTENDED_ADDRESS_READ	0xC8


//#define COMMAND_SECTOR_ERASE				0xD8 /* Sector Erase command */
//#define COMMAND_4KB_SUBSECTOR_ERASE		0x20 /* 4KB Subsector Erase command */
//#define COMMAND_32KB_SUBSECTOR_ERASE		0x52 /* 32KB Subsector Erase command */




/* Flash Definitions */
#define PAGE_SIZE			256
#define SUBSECTOR_SIZE		4096
#define SECTOR_SIZE			65536
#define READ_WRITE_EXTRA_BYTES	4

/* Function Prototypes */
int init_flash(void);
int SpiFlashWriteEnable(XQspiPs *SpiPtr);
int SpiFlashWrite(XQspiPs *SpiPtr, u32 Addr, u32 ByteCount, u8 WriteCmd, u8 *data);
int SpiFlashRead(XQspiPs *QspiPtr, u32 Addr, u32 ByteCount, u8 ReadCmd, u8* buf);
int SpiFlashBulkErase(XQspiPs *SpiPtr);
int SpiFlashSectorErase(XQspiPs *SpiPtr, u32 Addr);
int SpiFlashSubSectorErase(XQspiPs *SpiPtr, u32 Addr);
int SpiFlashGetStatus(XQspiPs *SpiPtr);
int SpiFlashWaitForFlashReady(void);
int SpiFlashReadID(XQspiPs *SpiPtr, u8 *id);
int SpiFlashGlobalUnprotect(void);
int SpiFlashGlobalProtect(void);
void SpiHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount);

int SerialDownload();
unsigned short updatecrc16(unsigned short crc, unsigned char ser_data);


#define SPIF_FSBL_IMAGE_BASE		0x000000
#define SPIF_FSBL_IMAGE_SIZE		0x020000	// 	128KB
	
#define SPIF_UMON_IMAGE_BASE		0x020000	
#define SPIF_UMON_IMAGE_SIZE		0x060000	// 	384KB
	
//#define SPIF_APP_IMAGE_BASE			0x080000
//#define SPIF_APP_IMAGE_SIZE			0x080000	// 	512KB
	
#define SPIF_TFS_BASE				0xFA0000	//	
#define SPIF_TFS_SIZE				0x060000	// 	384KB

//#define SPIF_PLX_IMAGE_BASE			0x100000	//	Size = 4E0000
//#define SPIF_PLY_IMAGE_BASE			0x5E0000	//	Size = 4E0000
//#define SPIF_PLZ_IMAGE_BASE			0xAC0000	//	Size = 4E0000  B0 E000
//#define SPIF_PLX_IMAGE_SIZE			0x4E0000	// 	5MB


#define SPIF_PLX_IMAGE_BASE			0x180000	//	Size = 400000
#define SPIF_PLY_IMAGE_BASE			0x580000	//	Size = 400000
#define SPIF_PLZ_IMAGE_BASE			0x980000	//	Size = 400000
#define SPIF_PL_IMAGE_SIZE			0x400000	// 	4MB



#define SPIF_APPX_IMAGE_BASE		0x080000
#define SPIF_APPZ_IMAGE_BASE		0x100000
#define SPIF_APP_IMAGE_SIZE			0x080000	// 	512KB


#define SPIF_PL_INFO_BASE			0xFA0000
#define SPIF_APP_INFO_BASE			0xFA0004


#define SPIF_FILE_SAVE_BASE			0xFA1000
#define SPIF_FILE_SAVE_SIZE			0x001000	//	4KB

#define SPIF_TEMP_TABLE_BASE		0xFA2000
#define SPIF_TEMP_TABLE_SIZE		0x001000	//	4KB

#define SPIF_AMP_ONOFF_BASE			0xFA3000
#define SPIF_AMP_ONOFF_SIZE			0x001000	//	4KB

#define SPIF_EQUAL_TABLE_BASE		0xFA4000
#define SPIF_EQUAL_TABLE_SIZE		0x001000	//	4KB

#define SPIF_ICS_INFO_BASE			0xFA5000
#define SPIF_ICS_INFO_SIZE			0x001000	//	4KB



#endif
