#include <msp430.h>
#include "uart.h"


void initUART(int mode){

    // Set Pin Modes
    P1SEL = BIT1 + BIT2 ;                   // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                  // P1.1 = RXD, P1.2=TXD
    // Set Clocking
    UCA0CTL1 |= UCSSEL_2;                   // SMCLK
    UCA0BR0 = 139;                          // 16MHz 115200
    UCA0BR1 = 0;                            // 16MHz 115200
    // Set UART Modes
    UCA0MCTL = UCBRS0;                      // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                        // Enable USCI_A0 RX interrupt

}

void printlnUART(unsigned char message[], int msgLen){

    volatile unsigned int i;

    // If msgLen given
    // for msgLen of message[] output that char to the Tx buffer
    if(msgLen>0){
        for(i = 0; i < msgLen; i++){
            while (!(IFG2&UCA0TXIFG));          // USCI_A0 TX buffer ready?
            UCA0TXBUF = message[i];
        }//for
    }
    // Else, print until a null terminator is found
    else{
        for(i = 0; message[i]!='\0'; i++){
            while (!(IFG2&UCA0TXIFG));          // USCI_A0 TX buffer ready?
            UCA0TXBUF = message[i];
        }//for
    }

    while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
    UCA0TXBUF = 0x0D;                       // TX -> Carriage return /r(0D)
    while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
    UCA0TXBUF = 0x0A;                       // Tx -> Line Feed /n(0A)

}//outputMessage()


// UART RX Interrupt
#pragma vector=USCIAB0RX_VECTOR
//  Echo back RXed character, confirm TX buffer is ready first
__interrupt void USCI0RX_ISR(void)
{

    uartRxBuf[uartRxBufNdx] = UCA0RXBUF;                     // Set Received char to rxChar;
    uartRxBufNdx++;

    if(uartRxBufNdx>RX_BUF_SIZE){
        uartRxBufNdx = 0;
    }

    IFG2 = IFG2 & 0x0A;
}
