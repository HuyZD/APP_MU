///*
// * queue.h
// *
// *  Created on: 2022. 2. 7.
// *      Author: LJH
// */
//
//#ifndef INC_UART_QUEUE_H_
//#define INC_UART_QUEUE_H_
//
////#include "main.h"
//#include <stdint.h>
//
//#define QUEUE_SIZE 128
//
//#define TRUE 1
//#define FALSE 0
//
//typedef struct{
//  int rear;
//  int front;
//  uint8_t data[QUEUE_SIZE];
//}Queue;
//
//void Init_queue(Queue *q);
//int is_empty(Queue *q);
//int is_full(Queue *q);
//void queue_clear(Queue *q);
//void enqueue(Queue *q, uint8_t data);
//uint8_t dequeue(Queue *q);
//
//void Init_Uart0();
//void Uart0_putchar(uint8_t data);
//void Uart0_getchar(uint8_t);
//uint8_t Uart0_returnchar();
//uint8_t Uart0_empty();
//void uprintf(const char *format, ...);
//void Uart0_puts(char *str);
//
//
//#endif /* INC_UART_QUEUE_H_ */
