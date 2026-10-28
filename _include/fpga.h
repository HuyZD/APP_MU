/* fpga.h  */
#ifndef __FPGA_H__
#define __FPGA_H__

inline static unsigned long FPGA_inl(unsigned int p)
{
	unsigned char *__p = (unsigned char *)p;
	unsigned long __r = 0;

	__r = *( unsigned char *)(__p+3);
	__r <<= 8;
	__r |= *( unsigned char *)(__p+2);
	__r <<= 8;
	__r |= *( unsigned char *)(__p+1);
	__r <<= 8;
	__r |= *( unsigned char *)(__p);

	return (__r);
}

//inline static unsigned long FPGA_inl(unsigned int p)
//{
//    unsigned char *__p = (unsigned char *)p;
//    unsigned long __r = 0;
//
//    __r = *( unsigned char *)(__p+3);
//    __r <<= 8;
//    __r |= *( unsigned char *)(__p+2);
//    __r <<= 8;
//    __r |= *( unsigned char *)(__p+1);
//    __r <<= 8;
//    __r |= *( unsigned char *)(__p);
//
//    return (__r);
////    return *(volatile unsigned int *)(p);
//}

inline static unsigned short FPGA_inw(unsigned int p) 
{
    //unsigned int __r = 0;
    
    //__r |= (*(volatile unsigned char *)(p) << 0);
    //__r |= (*(volatile unsigned char *)(p+1) << 8);
    
    return *(volatile unsigned short *)(p);
    //return (unsigned int)(__r); 
}

inline static unsigned char FPGA_inb(unsigned int p)
{
    return *(volatile unsigned char *)(p);
}

inline static void FPGA_outl(unsigned int r, unsigned int d)
{
    //unsigned long __d = d;
    //
    //*((volatile unsigned char *)(r)) = __d & 0xff; 
    //__d >>= 8;
    //*((volatile unsigned char *)(r+1)) = __d & 0xff;
    //__d >>= 8;
    //*((volatile unsigned char *)(r+2)) = __d & 0xff;
    //__d >>= 8;
    //*((volatile unsigned char *)(r+3)) = __d & 0xff;
	*((unsigned int *)((r)&~3)) = d;
}

//inline static void FPGA_outl(unsigned int r, unsigned int d)
//{
//    //unsigned long __d = d;
//    //
//    //*((volatile unsigned char *)(r)) = __d & 0xff;
//    //__d >>= 8;
//    //*((volatile unsigned char *)(r+1)) = __d & 0xff;
//    //__d >>= 8;
//    //*((volatile unsigned char *)(r+2)) = __d & 0xff;
//    //__d >>= 8;
//    //*((volatile unsigned char *)(r+3)) = __d & 0xff;
//	*((volatile unsigned int *)((r)&~3)) = d;
//}

inline static void FPGA_outw(unsigned int r, unsigned short d)
{
	//*((volatile unsigned char *)(r)) = d & 0xff;
	//*((volatile unsigned char *)(r+1)) = (d >> 8) & 0xff;
	//*((volatile unsigned short *)(r)) = d;
	//*((unsigned short *)(r)) = d;
	*((unsigned short *)((r)&~1)) = d;
}

inline static void FPGA_outb(int r, unsigned char d)   
{
	unsigned short __d = (unsigned char)(d);
	unsigned short __w = FPGA_inw((r)&~1);
	__w &= ((r)&1) ? 0x00FF : 0xFF00;
	__w |= ((r)&1) ? __d<<8 : __d;
	FPGA_outw((r)&~1, __w);
}
	
//inline static void FPGA_outb(unsigned int r, char d)   
//{
//    *((volatile unsigned char *)(r)) = d;
//}

//inline static void FPGA_outb(unsigned int addr, unsigned char val)
//{
//	unsigned char *m_addr;
//
//	m_addr = (unsigned char *)addr;
//	*m_addr = val;
//}

#endif
