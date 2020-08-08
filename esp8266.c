#include <msp430.h>
#include "esp8266.h"
#include "uart.h"

//unsigned const char CONST_SSID_STR[] = "AT+CWJAP=\"SSID\",\"PASSWORD\"\0";

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


void sendGET(int seq, unsigned int data){

    printlnUART("AT+CIPSTART=\"TCP\",\"www.patrickcote.net\",80", 42);
    delayMS(20);
    unsigned char sendData[50] = "GET /test.php?name=MSP430&xxxxx=XXXX  HTTP/1.1\0";

    // Select ph or temp
    if(seq == 1){
        sendData[26] = 'p';
        sendData[27] = 'h';
        sendData[28] = 'D';
        sendData[29] = 'a';
        sendData[30] = 't';
    }
    else{
        sendData[26] = 't';
        sendData[27] = 'e';
        sendData[28] = 'm';
        sendData[29] = 'p';
        sendData[30] = 'C';
    }
    // Load Data
    sendData[32] = 0x30;
    sendData[33] = data/100+0x30;
    sendData[34] = (data%100)/10+0x30;
    sendData[35] = data%10+0x30;

    printlnUART("AT+CIPSEND=83", 13);
    delayMS(20);
    printlnUART(sendData, 0);    // 52
    printlnUART("Host: www.patrickcote.net\r\n", 27);                 //29
    delayMS(20);

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
