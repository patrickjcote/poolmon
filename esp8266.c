#include <msp430.h>
#include "esp8266.h"
#include "uart.h"

unsigned const char CONST_SSID_STR[] = "AT+CWJAP=\"SSID\",\"PASSWORD\"\0";

void initESP(){

    printlnUART("ATE1",4);
    delayMS(20);
    printlnUART("AT+RST",6);
    delayMS(200);
    printlnUART("AT+CWMODE=1",11);
    delayMS(200);
    printlnUART(CONST_SSID_STR,0);
    delayMS(500);
    printlnUART("AT+CIFSR",8);
    delayMS(20);
}


void sendGET(int seq, unsigned int pHadc){

    printlnUART("AT+CIPSTART=\"TCP\",\"www.patrickcote.net\",80", 42);
    delayMS(20);

    unsigned char pHdata[50] = "GET /test.php?name=MSP430&phDat=XXXX  HTTP/1.1";
    pHdata[32] = 0x30;
    pHdata[33] = pHadc/100+0x30;
    pHdata[34] = (pHadc%100)/10+0x30;
    pHdata[35] = pHadc%10+0x30;

    if(seq%2){
        printlnUART("AT+CIPSEND=83", 13);
        delayMS(20);
        printlnUART(pHdata, 46);    // 52
        printlnUART("Host: www.patrickcote.net\r\n", 27);                 //29
        delayMS(20);
    }
    else{
        printlnUART("AT+CIPSEND=55", 13);
        delayMS(20);
        printlnUART("GET /test.txt HTTP/1.1", 22);    // 24
        printlnUART("Host: www.patrickcote.net\r\n", 27);                 //29
        delayMS(200);
    }

    printlnUART("AT+CIPCLOSE", 11);

}


// Lazy Delay Function
// TODO: Replace with Timer
void delayMS(int ms){

    volatile int delayCount;
    for( delayCount = 0; delayCount < ms*10; delayCount++){
        __delay_cycles(MS_DELAY_COUNT);
    }

}
