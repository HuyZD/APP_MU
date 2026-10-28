#define   CRC_16_POLYNOMIAL 	0x1021
#define   CRC_16_SEED			0x0000
#define   CRC_16_STEP_SEED		(~((unsigned short) CRC_16_SEED))
#define   CRC_TAB_SIZE			256

static int CRC_CCITT_TABLE( unsigned char i )
{

    int j;
    unsigned short crc, c;

	crc = 0;
	c   = ((unsigned short) i) << 8;

	for (j=0; j<8; j++) {
	    if ( (crc ^ c) & 0x8000 )
	    	crc = ( crc << 1 ) ^ 0x1021;
	    else
	    	crc =   crc << 1;

	    c = c << 1;
	}

	return (crc);

}  /* CRC_CCITT_TABLE */

unsigned short Generate_CRC (char *buf_ptr, int len)
{
	unsigned short data, crc_16;

	len *= 8;
	for (crc_16=CRC_16_SEED;len>=8;len-=8,buf_ptr++) {
		//crc_16 = Crc16_Table[ (crc_16 >> 8) ^ * buf_ptr ] ^ (crc_16 << 8);
		crc_16 = CRC_CCITT_TABLE( (crc_16 >> 8) ^ * buf_ptr ) ^ (crc_16 << 8);
	}
	
	if (len != 0) {
		data = ((unsigned short) (*buf_ptr)) << (8); 	
		while (len-- != 0) {
			if ( ((crc_16 ^ data) & ((unsigned short) 1 << 15)) != 0 ) {  
			crc_16 <<= 1;
			crc_16 ^= CRC_16_POLYNOMIAL; 
			}
			else crc_16 <<= 1; 
			data <<= 1;
		}
	}
	return (crc_16);
}
