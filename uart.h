#ifndef UART_H_
#define UART_H_

#define RX_BUF_SIZE 400

extern volatile unsigned int uartRxBufNdx;
extern volatile unsigned char uartRxBuf[RX_BUF_SIZE];

void initUART(int);
void clearBuffer();
void printlnUART(unsigned char messagePR[], int lengthPR);

#endif
