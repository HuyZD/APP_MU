/* spif_map.h:
 * Defines used to configure the memory map as it is to be used on
 * the SPI flash...
 * 
 *  0x3fffff -------------
 *           |           |
 *           |           |
 *           | TFS SPACE |
 *           |           |
 *           |           |
 *           |           |
 *           |           |
 *           |           |
 *  0x080000 -------------
 *           |           |
 *           | bram.bin  |
 *           |  (uMON)   |
 *           |           |
 *  0x018000 -------------
 *           |           |
 *           |   Misc    |
 *           |           |
 *  0x012000 -------------
 *           | SERIALNUM |
 *  0x011000 -------------
 *           |  MACADDR  |
 *  0x010000 -------------
 *           | mode3.bin |
 *  0x000000 -------------
 * 
 */ 
#if 1	//bk
#define SPIF_SIZE	0x1000000	/* 16MByte */
#define SPIF_EOM	0xffffff
#else
#define SPIF_SIZE	0x400000
#define SPIF_EOM	0x3fffff
#endif

//#define SPIF_MODE3_IMAGE_BASE	0
//#define SPIF_MODE3_IMAGE_SIZE	0x010000
//
//#define SPIF_MAC_ADDRS_BASE		0x010000
//#define SPIF_MAC_ADDRS_SIZE		0x001000
//
//#define SPIF_SNO_ADDRS_BASE		0x011000
//#define SPIF_SNO_ADDRS_SIZE		0x001000
//
//#define SPIF_MISC_BASE			0x012000
//#define SPIF_MISC_SIZE			0x00e000
