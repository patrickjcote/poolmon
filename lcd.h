#ifndef LCD_H_
#define LCD_H_

#define LCD_RS		BIT3
#define LCD_CS		BIT0
#define LCD_PxOUT 	P2OUT
#define LCD_PxDIR 	P2DIR

#define MOSI		BIT2
#define SCL			BIT1
#define SPI_PxDIR	P2DIR
#define SPI_PxIN	P2IN
#define SPI_PxOUT	P2OUT
#define SPI_PERIOD	50

#define _RS_Command() 	LCD_PxOUT &= ~LCD_RS
#define _RS_Data()		LCD_PxOUT |= LCD_RS

typedef struct lcds {
	volatile unsigned int currentPosition;
	volatile unsigned char memory[2][16];
} LCD;


void lcdInit(LCD *);
void lcdComByte(LCD *,char);
void lcdDataByte(LCD *,char);
void lcdBackspace(LCD *);
void redrawScreen(LCD *);
void updateLCD(LCD lcd, int bufferNdx,  volatile unsigned char *);


#endif
