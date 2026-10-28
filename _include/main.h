/*
 * main.h
 *
 *  Created on: 2023. 4. 10.
 *      Author: LJH
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_


unsigned long FPGA_inl(unsigned int addr);
unsigned short FPGA_inw(unsigned int addr);
unsigned char FPGA_inb(unsigned int addr);
void FPGA_outl(unsigned int addr, unsigned int data);
void FPGA_outw(unsigned int addr, unsigned short data);
void FPGA_outb(unsigned int addr, unsigned char data);


#endif /* SRC_MAIN_H_ */
