#ifndef ESP8266_H_
#define ESP8266_H_

#define MS_DELAY_COUNT 160 // MS_DELAY_COUNT = 0.001*1/F_CLK/10

void sendGET(int,unsigned int);
void initESP();
void delayMS(int);

typedef struct esps {
    volatile unsigned int connected;
} ESP;


#endif
