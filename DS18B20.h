#ifndef DS18B20_H_
#define DS18B20_H_

#define US_MULTIPLIER 16 // 1microsecond = US_MULTIPLIER * SYS_FREQ

#define DS_DQ_BIT  BIT5
#define DS_PxOUT   P2OUT
#define DS_PxIN    P2IN
#define DS_PxDIR   P2DIR


unsigned char ow_reset(void);
void write_bit(char);
void write_byte(char);
unsigned char read_bit(void);
unsigned char read_byte(void);
short Read_Temperature(void);

#endif
