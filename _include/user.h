//#define DDR_BASE_ADDR		 XPAR_PSU_DDR_0_S_AXI_BASEADDR
//#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x1000000)
//#define RX_BD_SPACE_BASE	(MEM_BASE_ADDR)
//#define RX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x0000FFFF)
//#define TX_BD_SPACE_BASE	(MEM_BASE_ADDR + 0x00010000)
//#define TX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x0001FFFF)
//#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
//#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
//#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x004FFFFF)
//
//#define DMA_PASSTHROUGH_SUCCESS            0
//#define DMA_PASSTHROUGH_DMA_INIT_FAIL     -1
//#define DMA_PASSTHROUGH_XFER_FAIL         -2

// EMIO PINs
#define RESET_PIN   	78
#define SELECT_PIN_1  	79
#define CPRI_TOP_RESET	81



// Funtion Include
extern u32 gpio_config();
extern u32 write_pin(u32 Pin, u32 Data);
extern u32 read_pin(u32 Pin);
