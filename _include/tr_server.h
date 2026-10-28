#ifndef __TR_SERVER_H__
#define __TR_SERVER_H__

/*
    Function: vTrServerWorker
    Description:
        Handling the TR server protocol worker task. 
    Parameters:
        uart_index: The index of the UART port to read from.
*/
void vTrServerWorker(short uart_index);

/*
    Function: vTrServerWorkerSAU
    Description:
        Handling the TR server protocol worker task for SAU.
*/
void vTrServerTaskSAU();

/*
    Function: vTrServerWorkerRuBr1
    Description:
        Handling the TR server protocol worker task for RU BR1.
*/
void vTrServerTaskRuBr1();

/*
    Function: vTrServerWorkerRuBr2
    Description:
        Handling the TR server protocol worker task for RU BR2.
*/
void vTrServerTaskRuBr2();

/*
    Function: vTrServerWorkerRuBr3
    Description:
        Handling the TR server protocol worker task for RU BR3.
*/
void vTrServerTaskRuBr3();

/*
    Function: vTrServerWorkerRuBr4
    Description:
        Handling the TR server protocol worker task for RU BR4.
*/
void vTrServerTaskRuBr4();

void WaitForTopologyDone();

#endif