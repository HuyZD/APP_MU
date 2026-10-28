

/*
* AT25 SPI EEPROM (using spidev driver)
*/
//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/unistd.h>
//#include <linux/spi/spidev.h>
//#include "genlib.h"
//#include "stddefs.h"
//#include "cli.h"
//#include "psa.h"
//
//static int spi_fpga_fd;
//void spi_fpga_open();
//void spi_fpga_close();
//
//extern int 		api_initGpio(unsigned int port_num, unsigned int dir, unsigned int val);
//extern int 		api_setGpio(unsigned int port_num, unsigned int val);
//extern int 		api_getGpio(unsigned int port_num);
//extern int 		api_freeGpio(unsigned int port_num);
//extern int 		api_checkGpioFree(unsigned int port_num);
//
//static void pabort(const char *s)
//{
//  perror(s);
//  abort();
//}
//
//static uint8_t mode=0;
//static uint8_t bits = 8;
//static uint32_t speed = 5000000;
////static uint32_t speed = 1000000;
//
///*
//	FPGA configuration 을 위한 시험용 function
//	0부터 ~ 127 를 
//	순차적으로 1byte 씩 출력하는 내용
//*/
//static void fpga_config_test(int fd)
//{
//	int ret;
//	int i;
//	struct spi_ioc_transfer message[2] = {0, };	
//	uint8_t buf[128] = {0, };
//	
//	for(i=0;i<128;i++)
//		buf[i] = i;
//
//	message[0].tx_buf = (unsigned long)buf;
//	message[0].len = sizeof(buf);
//	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//	if (ret < 1)
//		pabort("fpga config failed");
//}
//
///*
// ************************************************
// * _configFpga Function:
// * fpga program(write) start api
// ************************************************
//*/
//
//#define	FPGA_CONF_RESETB		26	// pin number
//#define	FPGA_CONF_CS			75  // pin number
//#define	FPGA_CONF_DONE			11  // pin number
//#define	FPGA_TIMEOUT_LATENCY	100
//
//#define	MAX_SIZE_PER_ONE	2048
//
//static void configFpga(uint32_t *buf, int size)
//{
//	int ret = 0;
//	int fd;
//	unsigned int 		i;	
//	unsigned char 		*ptr = (unsigned char *)buf;
//	int					timeout;
//
//	char data[MAX_SIZE_PER_ONE];
//
//	/* Set GPIO Direction Pin */
//	api_initGpio(FPGA_CONF_CS,0,0);		// Input Mode
//	api_initGpio(FPGA_CONF_RESETB,1,1);	// Output Mode
//	api_initGpio(FPGA_CONF_DONE,0,0);		// Input Mode
//
//	printf("Programming xlnx fpga..\n");
//
//	/* open the spi device */
//	fd = open("/dev/spidev6.0", O_RDWR);
//	if (fd < 0)
//	  pabort("can't open device");
//
//	/* set the spi mode */
//	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
//	if (ret == -1)
//	  pabort("can't set spi mode");
//
//	/* set the spi bits per word */
//	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
//	if (ret == -1)
//	  pabort("can't set bits per word");
//
//	/* set the spi max speed */
//	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
//	if (ret == -1)
//	  pabort("can't set max speed hz");
//	
//	struct spi_ioc_transfer message[2] = {0, };
//
//	message[0].tx_buf = (unsigned long)data;
//	message[0].len = sizeof(data);
//
////	message[0].tx_buf = (unsigned long)buf;
////	message[0].len = size;
//
//	/* Set GPIO */
//	api_setGpio(FPGA_CONF_RESETB,1);	// High->Low>High , toggle 시켜야 시작한다.
//	api_setGpio(FPGA_CONF_RESETB,0);	// High->Low>High , toggle 시켜야 시작한다.
//	usleep(100000);
//	if (api_getGpio(FPGA_CONF_CS) == 0) {
//		printf("INIT_B Cleared\n");
//	}
//	api_setGpio(FPGA_CONF_RESETB,1);	// High->Low>High , toggle 시켜야 시작한다.
//	usleep(100000);
//	if (api_getGpio(FPGA_CONF_CS) == 1) {
//		printf("INIT_B Cleared\n");
//	}
//
////	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
////	if (ret < 1)
////		pabort("FPGA write failed");
//	
//	/*
//		64byte 씩 전송.
//		변경 가능함 
//	*/
//    for (i=0;i<size;i++) {     	     	
//		data[i%MAX_SIZE_PER_ONE] = *(ptr+i);
//
//		if ( (i%MAX_SIZE_PER_ONE) == (MAX_SIZE_PER_ONE-1) ) {
//			ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//			if (ret < 1)
//	    		pabort("FPGA write failed");
//	    } else if (i == (size-1)) {
//			ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//			if (ret < 1)
//	    		pabort("FPGA write failed");	    	
//	    }
//	    /*
//	    	Padding 부분 나머지 처리 알고리즘 보강 예정
//	    */		
//	}
//
//	/* */
//    for (i=0;i<256;i++) {     	     	
//     	data[i] = 0;
//
//		if (i%256 == 255) {
//			ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//			if (ret < 1)
//	    		pabort("FPGA write failed");
//	    }		
//	}
//	
//	close(fd); //closed spi fd 
//
///* Clock the FPGA to finish the initialization */
///* Wait for INIT to go high */
//    timeout = FPGA_TIMEOUT_LATENCY*1000;
//
//	printf("Waiting FPGA Done..\n");
//	while (timeout--) {
//		if (api_getGpio(FPGA_CONF_DONE) == 1) {
//			printf("### fpga_program ==> FPGA_DONE = 1 ###\n");
//			break;
//		}        
//	}
//
//	if (timeout < 0) {
//		printf("timeout.. Done did not go high\n");
//		return (-1);
//	}
//}
//
//int	init_fpga(void)
//{
//	int		i;
//
//	char	f_name[32];
//	FILE    *pFile;
//	uint32_t	*fpga_buf;
//	uint32_t	f_size	= 0;
//	char    *f_buffer;
//
//	/*	fpga 파일은
//		/data/app안에서 존재함
//	*/
//#if	1// 파일 지정
//	sprintf(f_name,"/data/app/NOS.bin");
//	//sprintf(f_name,"/data/app/top_NOS_TRI.bin");
//	pFile = fopen(f_name, "rb");
//#else // link 방식
//	pFile = fopen("/data/app/fpga", "rb");
//#endif
//
//	if(pFile == NULL) {
//		printf("### FPGA File(%s) Open Error ###\n", f_name);
//    	return(-1);
//	}
//
//	fseek(pFile, 0, SEEK_END);
//	f_size = ftell(pFile);
//	rewind(pFile);
//	printf("### FPGA f_buffer Size = [0x%x] ###\n", f_size);
//
//	f_buffer = (char *)malloc(sizeof(char)*f_size);
//	if(f_buffer == NULL) {
//		printf("### FPGA f_buffer Malloc Error ###\n");
//    	return(-1);
//	}
//
//	if(fread(f_buffer, 1, f_size, pFile) < 0) {
//		printf("### FPGA f_buffer Fread Error ###\n");
//    	return(-1);
//	}
//	else {								
//		fpga_buf = (volatile uint32_t *)f_buffer;
//		printf("### FPGA config... %s,%d ###\n",f_name,f_size);     
//		configFpga((volatile uint32_t *)f_buffer, (int)f_size);
//	}
//
//	fclose(pFile);
//	free(f_buffer);
//
//	printf("Configuration Done..\n");
//}
//
//
//
//#define FPGA_WRITE		(0x80)  /*  */
///*
//	FPGA 24bit(address)/32bit(data) DATA I/F
//	SPI Read API
//*/
//static unsigned int spi_fpgaread(int fd, unsigned int address)
//{
//	int ret;
//	unsigned int val=0;  
//	char address1, address2, address3;
//
//	/* 상위 24 bit : Address
//		MSB : Read(0),Write(1)
//		8bit 단위로 전송되기 때문에, 1byte 3개로 변환
//	*/
//	address1 = ((address>>16) & 0xff) & ~FPGA_WRITE;;
//	address2 = ((address>>8) & 0xff);
//	address3 = ((address>>0) & 0xff);
//
//	char read_addr[3] = {address1, address2, address3};
//
//	/* 하위 32bit : Data 
//		4byte, 수신 버퍼
//		MISO로 전달되는 데이타를 해당 버퍼에 저장한다.
//	*/
//	uint8_t rx[4] = {0, };
//
//	/* 
//		SPI IOCTL 버퍼 2개 생성
//		message[0]은 Address 32bit 전송 처리용
//		message[1]은 수신 데이타 처리용
//	*/
//	struct spi_ioc_transfer message[2] = {0, };
//
//	message[0].tx_buf = (unsigned long)read_addr;
//	message[0].len = sizeof(read_addr);
//
//	message[0].rx_buf = (unsigned long)rx;
//	message[1].len = 4;	// Data: 32bits
//
//	ret = ioctl(fd, SPI_IOC_MESSAGE(2), &message);
//	if (ret < 1)
//		pabort("FPGA read failed");
//
//	val =  (rx[0] << 24);
//	val |= (rx[1] << 16);
//	val |= (rx[2] <<  8);
//	val |= (rx[3] <<  0);
//
//	return val;
//}
//
///*
//	FPGA 24bit(address)/32bit(data) I/F
//	SPI Write API
//*/
//static unsigned int spi_fpgawrite(int fd, unsigned int address, unsigned int data)
//{
//	int ret;
//	char address1, address2, address3;
//	char data1, data2, data3, data4;
//
//	/* 상위 24bit : Address
//		MSB : Read(0),Write(1)
//		8bit 단위로 전송되기 때문에, 1byte 4개로 변환
//	*/
//	address1 = ((address>>16) & 0xff)| FPGA_WRITE;
//	address2 = ((address>>8) & 0xff);
//	address3 = ((address>>0) & 0xff);
//
//	/* 하위 32bit : 송신 Data 
//	*/	
//	data1 = ((data>>24) & 0xff);
//	data2 = ((data>>16) & 0xff);
//	data3 = ((data>>8) & 0xff);
//	data4 = ((data>>0) & 0xff);
//
//	char write_buf[7] = {address1, address2, address3, data1, data2, data3, data4};
//
//	struct spi_ioc_transfer message[2] = {0, };
//
//	message[0].tx_buf = (unsigned long)write_buf;
//	message[0].len = sizeof(write_buf);
//
//	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//	if (ret < 1)
//    	pabort("FPGA write failed");
//}
//
//
//void spi_fpga_open()
//{
//	int ret = 0;
//	
//	/* open the spi device */
//	spi_fpga_fd = open("/dev/spidev1.0", O_RDWR);
//	if (spi_fpga_fd < 0)
//	  pabort("can't open device");
//	
//	/* set the spi mode */
//	ret = ioctl(spi_fpga_fd, SPI_IOC_WR_MODE, &mode);
//	if (ret == -1)
//	  pabort("can't set spi mode");
//	
//	/* get the spi mode (test) */
//	ret = ioctl(spi_fpga_fd, SPI_IOC_RD_MODE, &mode);
//	if (ret == -1)
//	  pabort("can't get spi mode");
//	
//	/* set the spi bits per word */
//	ret = ioctl(spi_fpga_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
//	if (ret == -1)
//	  pabort("can't set bits per word");
//	
//	/* get the spi bits per word (test) */
//	ret = ioctl(spi_fpga_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
//	if (ret == -1)
//	  pabort("can't get bits per word");
//	
//	/* set the spi max speed */
//	ret = ioctl(spi_fpga_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
//	if (ret == -1)
//	  pabort("can't set max speed hz");
//	
//	/* get the spi max speed (test) */
//	ret = ioctl(spi_fpga_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
//	if (ret == -1)
//	  pabort("can't get max speed hz");
//	
//	puts("");
//	/* output successful settings to the terminal */
//	printf("The spi mode is set to: %d\n", mode);
//	printf("Bits per word: %d\n", bits);
//	printf("Max speed is set to: %d Hz (%d KHz) (%d MHz)\n", speed, speed/1000, speed/1000000);
//
////	eepromwritebyte(fd);
////	usleep(5000);
////	eepromread(fd);
////	printf("0x%08X\n", spi_fpgaread(fd, 0x0a0a0a0a));
//	
////	spi_fpgawrite(int fd, unsigned int address, unsigned int data);
////	fpga_config(fd);
//
////	fpga_config_test(fd);
////	close(fd);
//
////	return ret;
//}
//
//void spi_fpga_close()
//{
//	close(spi_fpga_fd);	
//}
//
///* FPGA_outb() : SPI Write API
// *	FPGA 24bit(address)/32bit(data) I/F
// */
//struct spi_transfer {
//	union {
//		unsigned int 		_intval;
//		struct {
//			unsigned int	ADDR3				:  7;
//			unsigned int	nWR					:  1;
//			unsigned int	ADDR2				:  8;
//			unsigned int	ADDR1				:  8;
//			unsigned int	ADDR0				:  8;
//		} _bitval;
//	} ADDR;
//	union {
//		unsigned int 		_intval;
//		struct {
//			unsigned int	DATA3				:  8;
//			unsigned int	DATA2				:  8;
//			unsigned int	DATA1				:  8;
//			unsigned int	DATA0				:  8;
//		} _bitval;
//	} DATA;
//};
//
//int FPGA_outl(unsigned int addr, unsigned int data)
//{
//	int ret;
//	struct spi_ioc_transfer message[2] = {0, };
//	
//	struct spi_transfer wr_buf, rd_buf;
//
//	/* 상위 24bit : Address
//		MSB : Read(0),Write(1)
//		8bit 단위로 전송되기 때문에, 1byte 4개로 변환
//	*/
//	wr_buf.ADDR._bitval.nWR		= 0;
//	wr_buf.ADDR._bitval.ADDR0 	= ((addr >>  0) & 0xff);
//	wr_buf.ADDR._bitval.ADDR1 	= ((addr >>  8) & 0xff);
//	wr_buf.ADDR._bitval.ADDR2 	= ((addr >> 16) & 0xff);
//	wr_buf.ADDR._bitval.ADDR3 	= ((addr >> 24) & 0xff);
//	
//	wr_buf.DATA._bitval.DATA0 	= ((data >>  0) & 0xff);
//	wr_buf.DATA._bitval.DATA1 	= ((data >>  8) & 0xff);
//	wr_buf.DATA._bitval.DATA2 	= ((data >> 16) & 0xff);
//	wr_buf.DATA._bitval.DATA3 	= ((data >> 24) & 0xff);
//
//	message[0].tx_buf = (unsigned long)&wr_buf;
//	message[0].rx_buf = (unsigned long)&rd_buf;
//	message[0].len = sizeof(struct spi_transfer);
//	ret = ioctl(spi_fpga_fd, SPI_IOC_MESSAGE(1), &message);
//	if (ret < 1)
//    	pabort("FPGA write failed");
//
//	//printf("%02X%02X%02X%02X\n", rd_buf.DATA._bitval.DATA3,rd_buf.DATA._bitval.DATA2,rd_buf.DATA._bitval.DATA1,rd_buf.DATA._bitval.DATA0);
//	
//	return (4);
//}
//
//unsigned int FPGA_inl(unsigned int addr)
//{
//	int 						ret;
//	struct spi_ioc_transfer 	message[2] = {0, };
//	struct spi_transfer 		wr_buf, rd_buf;
//	unsigned int				data=0;
//
//	/* 상위 24bit : Address
//		MSB : Read(0),Write(1)
//		8bit 단위로 전송되기 때문에, 1byte 4개로 변환
//	*/
//	wr_buf.ADDR._bitval.nWR		= 1;							/* '1'->read transaction */
//	wr_buf.ADDR._bitval.ADDR0 	= ((addr >>  0) & 0xff);
//	wr_buf.ADDR._bitval.ADDR1 	= ((addr >>  8) & 0xff);
//	wr_buf.ADDR._bitval.ADDR2 	= ((addr >> 16) & 0xff);
//	wr_buf.ADDR._bitval.ADDR3 	= ((addr >> 24) & 0xff);
//
//	message[0].tx_buf = (unsigned long)&wr_buf;
//	message[0].rx_buf = (unsigned long)&rd_buf;
//	message[0].len = sizeof(struct spi_transfer);
//
//	ret = ioctl(spi_fpga_fd, SPI_IOC_MESSAGE(1), &message);
//	if (ret < 1)
//    	pabort("FPGA read failed");
//
//	data |= (rd_buf.DATA._bitval.DATA0 <<  0);
//	data |= (rd_buf.DATA._bitval.DATA1 <<  8);
//	data |= (rd_buf.DATA._bitval.DATA2 << 16);
//	data |= (rd_buf.DATA._bitval.DATA3 << 24);
//
//	return (data);
//}
//
//
///* configFpga
// */
//void configFPGA(char *buf, int size)
//{
//	int 				ret = 0;
//	int 				fd;
//	unsigned int 		i;	
//	unsigned char 		*ptr = (unsigned char *)buf;
//	int					timeout;
//
//	char 				data=0;
//	struct spi_ioc_transfer message[2] = {0, };
//	int					intg,frac;
//
//
//	/* Set GPIO Direction Pin */
//	api_initGpio(FPGA_CONF_CS,0,0);		// Input Mode
//	api_initGpio(FPGA_CONF_RESETB,1,1);	// Output Mode
//	api_initGpio(FPGA_CONF_DONE,0,0);		// Input Mode
//
//	printf("Programming xlnx fpga..\n");
//
//	/* open the spi device */
//	fd = open("/dev/spidev6.0", O_RDWR);
//	if (fd < 0)
//	  pabort("can't open device");
//
//	/* set the spi mode */
//	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
//	if (ret == -1)
//	  pabort("can't set spi mode");
//
//	/* set the spi bits per word */
//	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
//	if (ret == -1)
//	  pabort("can't set bits per word");
//
//	/* set the spi max speed */
//	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
//	if (ret == -1)
//	  pabort("can't set max speed hz");
//	
//	/* Set GPIO */
//	api_setGpio(FPGA_CONF_RESETB,1);	// High->Low>High , toggle 시켜야 시작한다.
//	api_setGpio(FPGA_CONF_RESETB,0);	// High->Low>High , toggle 시켜야 시작한다.
//	usleep(100000);
//	if (api_getGpio(FPGA_CONF_CS) == 0) {
//		printf("INIT_B Cleared\n");
//	}
//	api_setGpio(FPGA_CONF_RESETB,1);	// High->Low>High , toggle 시켜야 시작한다.
//	usleep(100000);
//	if (api_getGpio(FPGA_CONF_CS) == 1) {
//		printf("INIT_B Cleared\n");
//	}
//
//#define	FPGA_CFG_SIZE	4096		//	최대한 4096임. 최대 사이즈가 소스상에서 확인되지는 않음. 향후 문서적으로 확인 필요함. 혹은 우리넷에 SPI 드라이버 수정요청할 필요 있어 보임. 
//
//	intg=size/FPGA_CFG_SIZE;
//	frac=size%FPGA_CFG_SIZE;
//	for (i=0;i<intg;i++) {
//		message[0].tx_buf = (unsigned long)buf+i*FPGA_CFG_SIZE;
//		message[0].len = FPGA_CFG_SIZE;
//		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//		if (ret < 1)
//			pabort("FPGA write failed(0)");
//	}
//	message[0].tx_buf = (unsigned long)buf+i*FPGA_CFG_SIZE;
//	message[0].len = frac;
//	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//	if (ret < 1)
//		pabort("FPGA write failed(1)");
//
//	/* */
//	message[0].tx_buf = (unsigned long)&data;
//	message[0].len = 1;
//    for (i=0;i<4;i++) {     	     							//	최소 2개 이상의 Redundancy Clock Cylce이 필요함. 
//		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);			//	4,16개의 Redundancy Clock으로도 동작.
//		if (ret < 1)
//	    	pabort("FPGA write failed(2)");
//	}
//	
//	close(fd); //closed spi fd 
//
///* Clock the FPGA to finish the initialization */
///* Wait for INIT to go high */
//    timeout = FPGA_TIMEOUT_LATENCY*1000;
//
//	printf("Waiting FPGA Done..\n");
//	while (timeout--) {
//		if (api_getGpio(FPGA_CONF_DONE) == 1) {
//			printf("### fpga_program ==> FPGA_DONE = 1 ###\n");
//			break;
//		}        
//	}
//
//	if (timeout < 0) {
//		printf("timeout.. Done did not go high\n");
//		return (-1);
//	}
//}
//
///* configFpga
// */
//void TestFpga(char *buf, int size)
//{
//	int 				ret = 0;
//	int 				fd;
//	unsigned int 		i;	
//	int					timeout;
//
//	char 				data=0;
//	struct spi_ioc_transfer message[2] = {0, };
//	int					intg,frac;
//
//
//	/* open the spi device */
//	fd = open("/dev/spidev6.0", O_RDWR);
//	if (fd < 0)
//	  pabort("can't open device");
//
//	/* set the spi mode */
//	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
//	if (ret == -1)
//	  pabort("can't set spi mode");
//
//	/* set the spi bits per word */
//	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
//	if (ret == -1)
//	  pabort("can't set bits per word");
//
//	/* set the spi max speed */
//	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
//	if (ret == -1)
//	  pabort("can't set max speed hz");
//	
//#define	FPGA_CFG_SIZE	4096		//	최대한 4096임. 최대 사이즈가 소스상에서 확인되지는 않음. 향후 문서적으로 확인 필요함. 혹은 우리넷에 SPI 드라이버 수정요청할 필요 있어 보임. 
//
//	intg=size/FPGA_CFG_SIZE;
//	frac=size%FPGA_CFG_SIZE;
//	for (i=0;i<intg;i++) {
//		message[0].tx_buf = (unsigned long)buf+i*FPGA_CFG_SIZE;
//		message[0].len = FPGA_CFG_SIZE;
//		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//		if (ret < 1)
//			pabort("FPGA write failed(0)");
//	}
//	message[0].tx_buf = (unsigned long)buf+i*FPGA_CFG_SIZE;
//	message[0].len = frac;
//	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);
//	if (ret < 1)
//		pabort("FPGA write failed(1)");
//
//	/* */
//	message[0].tx_buf = (unsigned long)&data;
//	message[0].len = 1;
//    for (i=0;i<4;i++) {     	     							//	최소 2개 이상의 Redundancy Clock Cylce이 필요함. 
//		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &message);			//	4,16개의 Redundancy Clock으로도 동작.
//		if (ret < 1)
//	    	pabort("FPGA write failed(2)");
//	}
//	
//	close(fd); //closed spi fd 
//}
//
//void FPGA_RESET(uchar onoff)
//{
//	struct psa_struct 	*pdev;
//	unsigned int 		ioaddr,iodata;
//
//	pdev = getPsaDev();
//
//	ioaddr = pdev->psa_reg_base + PSA_CFG1;
//    iodata = FPGA_inl(ioaddr);
//    iodata = (onoff) ? (iodata | 0x80000000) : (iodata & ~0x80000000);
//	FPGA_outl(ioaddr,iodata);
//}
//
//void init_sysclk(void)
//{
//	struct psa_struct 	*pdev;
//	unsigned int 		ioaddr,iodata;
//	int					i;
//
//	pdev = getPsaDev();
//
//	/* SYS_RST 해제. sys_clk/dac_clk 블록 동작 개시. */
//	ioaddr = pdev->psa_reg_base + PSA_CFG1;
//    iodata = FPGA_inl(ioaddr);
//    iodata &= ~0x80000000;
//	FPGA_outl(ioaddr,iodata);
//	
//	/* 1. Main Clock : sys_clk (clkin = 245.76Mhz), LOCKED0
//	 * 2. DAC  Clock : dac_clk (clkin = 491.52Mhz), LOCKED1
//	 * 3. Ref  Clock : ref_clk (clkin =  30.72Mhz), LOCKED2
//	 * ADC Timing Calibration은 LOCKED2를 기준으로 동작하여야 한다. 
//	 * 30.72Mhz가 가장 늦게 Lock이 걸린다. 따라서 반드시 LOCKED2가 LOCK이 걸릴 때 까지 기다려야 한다. 
//	 */
//	ioaddr = pdev->psa_reg_base + PSA_CFG0;
//	for (i=0;i<1024;i++) {
//    	iodata = FPGA_inl(ioaddr);
//    	if ((iodata & 0x00000100) && /*(iodata & 0x00000200) && */(iodata & 0x00000400)) {
//    		/* LOCKED0,LOCKED1,LOCKED2 모두 잡혀야 다음 단계로 넘어간다. */
//    		break;
//    	}
//	}
//	if (i == 1024) {
//		printf("UnLocked!\n");
//	}
//}
//
//
///* showRegInfo()
// * fpga 내부의 레지스터 값들을 읽는다. 
// */
//static void showRegInfo(const char *desc[], unsigned int reg_base, unsigned int reg_size)
//{
//    //int i;
//	//
//    //for (i=0;i<reg_size;i++) {
//	//    printf("  %x:  %x\r\n", i, *(unsigned char *)(reg_base+i));
//    //}
//    int i;
//	
//    for (i=0;i<reg_size;i++) {
//    	if (i%16 == 0) {
//    		printf("0x%05X: ", (reg_base+i*4));
//    	}
//
//	    printf("%08x ", FPGA_inl(reg_base+i*4));
//
//    	if (i%16 == 15) {
//    		printf("\n");
//    	}
//    }
//}
//
//char *_FpgaHelp[] = {                                                       
//	"FPGA ",
//	"[application_info]",
//    "Operations:",
//    "'fpga read 0', 0번 섹터 Read, 1섹터->64KB",
//    "'fpga -f 'fname' prog",
//    "'fpga reg'",
//    "",
//	0,                                                                    
//};                                                                            
//                                                                              
//int
//_Fpga(int argc, char *argv[])
//{                       
//    int				i,j,k;
//    int     		snum;
//    int     		retval=0;
//	unsigned int	dest, src;
//    int     		bytecnt;
//    char 			*arg1,*arg2,*arg3;
//    struct stat 	sb;
//	int     		opt;
//
//	FILE    		*pFile;
//    char    		*fname=NULL;
//	long			fsize = 0;
//	char    		*f_buffer;
//
//    while ((opt = getopt(argc, argv, "F:")) != -1) {
//        switch (opt) {
//        case 'F':
//            fname = optarg;
//            break;
//        default:
//            retval = 0;
//            break;
//        }
//    }
//
//    arg1 = argv[optind];
//    arg2 = argv[optind+1];
//    arg3 = argv[optind+2];
//
//    if (!strcmp(arg1,"reg")) {
//		showRegInfo(0,0x00000,64);
//		showRegInfo(0,0x10000,64);
//		showRegInfo(0,0x20000,64);
//		showRegInfo(0,0x30000,64);
//		showRegInfo(0,0x40000,64);
//		showRegInfo(0,0x50000,64);
//		showRegInfo(0,0x60000,64);
//		showRegInfo(0,0x70000,64);
//		showRegInfo(0,0xF8000,64);
//    }
//    else if (!strcmp(arg1,"prog")) {
//		if (fname != NULL) {					//	"fpga -F fname prog" 로 fname이 -F option으로 명시되어 있다면, 해당 파일을 open한다. 
//			pFile = fopen(fname, "rb");
//			if(pFile == NULL) {
//				printf("There is no such file(%s) that you specified!!\n", fname);
//    			return(-1);
//			}
//		}
//		else {
//			pFile = fopen("fpga", "rb");		//	"fpga prog" 로 한다면 "fpga"라는 Symbolic Link파일을 찾아, 해당 파일을 open한다. 
//			if(pFile == NULL) {
//				printf("You have to make symbolic link file name of fpga\n");
//    			return(-1);
//			}
//		}
//    		
//		if (stat(fname, &sb) == -1) {
//		     perror("stat");					// symbolic link인 경우 에러 발생. 이럴 경우 fclose해줘야 하나? 확인 필요. 
//		     exit(EXIT_FAILURE);
//		 }
//        
//		fsize = (long)sb.st_size;
//		printf("FPGA File Size = [%d] ###\n", fsize);
//    	
//		f_buffer = (char *)malloc(sizeof(char)*fsize);
//		if(f_buffer == NULL) {
//			printf("### FPGA f_buffer Malloc Error ###\n");
//    		return(-1);
//		}
//    	
//		if(fread(f_buffer, 1, fsize, pFile) < 0) {
//			printf("### FPGA f_buffer Fread Error ###\n");
//		}
//		else {								
//			printf("### FPGA config... %s,%d ###\n",fname,fsize);     
//			configFPGA((volatile uint32_t *)f_buffer, (int)fsize);
//		}
//    	
//		fclose(pFile);
//		free(f_buffer);
//    	
//		printf("Configuration Done..\n");
//    }
//    else if (!strcmp(arg1,"stat")) {
//    	printf("FPGA prog\n");
//
//		//if (argc != 2) {
//		//     fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
//		//     exit(EXIT_FAILURE);
//		// }
//		
//		for (i=0;i<argc;i++) {
//			printf("%d -> argv[%s]\n", i, argv[i]);
//		}
//		if (fname != NULL) {
//			printf("File name %s\n", fname);
//			if (stat(fname, &sb) == -1) {
//			     perror("stat");
//			     exit(EXIT_FAILURE);
//			 }
//			
//			printf("File type:                ");
//			
//			switch (sb.st_mode & S_IFMT) {
//			case S_IFBLK:  printf("block device\n");            break;
//			case S_IFCHR:  printf("character device\n");        break;
//			case S_IFDIR:  printf("directory\n");               break;
//			case S_IFIFO:  printf("FIFO/pipe\n");               break;
//			case S_IFLNK:  printf("symlink\n");                 break;
//			case S_IFREG:  printf("regular file\n");            break;
//			case S_IFSOCK: printf("socket\n");                  break;
//			default:       printf("unknown?\n");                break;
//			}
//			
//			printf("I-node number:            %ld\n", (long) sb.st_ino);
//			printf("Mode:                     %lo (octal)\n",(unsigned long) sb.st_mode);
//			printf("Link count:               %ld\n", (long) sb.st_nlink);
//			printf("Ownership:                UID=%ld   GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);
//			printf("Preferred I/O block size: %ld bytes\n",(long) sb.st_blksize);
//			printf("File size:                %lld bytes\n",(long long) sb.st_size);
//			printf("Blocks allocated:         %lld\n",(long long) sb.st_blocks);
//			printf("Last status change:       %s", ctime(&sb.st_ctime));
//			printf("Last file access:         %s", ctime(&sb.st_atime));
//			printf("Last file modification:   %s", ctime(&sb.st_mtime));
//			
//			//exit(EXIT_SUCCESS);    
//		}
//	}
//    else if (!strcmp(arg1,"reset")) {
//		if (strcmp(arg2, "on") == 0) {
//			FPGA_RESET(1);
//		}
//		else if (strcmp(arg2, "off") == 0) {
//			FPGA_RESET(0);
//		}
//    }
//    else if (!strcmp(arg1,"write")) {
//    	unsigned int	addr,data;
//
//    	if (argc == 4) {
//    		addr = strtoul(arg2,(char **)0,0);
//    		data = strtoul(arg3,(char **)0,0);
//    		FPGA_outl(addr,data);
//    	}
//    }
//    else if (!strcmp(arg1,"read")) {
//    	unsigned int	addr,data;
//
//    	if (argc == 3) {
//    		addr = strtoul(arg2,(char **)0,0);
//    		data = FPGA_inl(addr);
//    		printf("@0x%02x -> 0x%08x\n",addr,data);
//    	}
//    }
//    else if (!strcmp(arg1,"test")) {
//		if (fname != NULL) {					//	"fpga -F fname prog" 로 fname이 -F option으로 명시되어 있다면, 해당 파일을 open한다. 
//			pFile = fopen(fname, "rb");
//			if(pFile == NULL) {
//				printf("There is no such file(%s) that you specified!!\n", fname);
//    			return(-1);
//			}
//		}
//		else {
//			pFile = fopen("fpga", "rb");		//	"fpga prog" 로 한다면 "fpga"라는 Symbolic Link파일을 찾아, 해당 파일을 open한다. 
//			if(pFile == NULL) {
//				printf("You have to make symbolic link file name of fpga\n");
//    			return(-1);
//			}
//		}
//    		
//		if (stat(fname, &sb) == -1) {
//		     perror("stat");					// symbolic link인 경우 에러 발생. 이럴 경우 fclose해줘야 하나? 확인 필요. 
//		     exit(EXIT_FAILURE);
//		 }
//        
//		fsize = (long)sb.st_size;
//		printf("FPGA File Size = [%d] ###\n", fsize);
//    	
//		f_buffer = (char *)malloc(sizeof(char)*fsize);
//		if(f_buffer == NULL) {
//			printf("### FPGA f_buffer Malloc Error ###\n");
//    		return(-1);
//		}
//    	
//		if(fread(f_buffer, 1, fsize, pFile) < 0) {
//			printf("### FPGA f_buffer Fread Error ###\n");
//		}
//		else {								
//			printf("### FPGA config... %s,%d ###\n",fname,fsize);     
//			TestFpga((volatile uint32_t *)f_buffer, (int)fsize);
//		}
//    	
//		fclose(pFile);
//		free(f_buffer);
//    	
//		printf("Configuration Done..\n");
//    }
//	
//	return 0;
//}                                                      

