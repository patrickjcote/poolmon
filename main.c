#include <msp430.h>
#include <stdio.h>

#include "DS18B20.h"
#include "lcd.h"
#include "uart.h"
#include "esp8266.h"

// Defines
#define TIMERA0_OFFSET   64000
#define TIME_DIVIDE 3

// Prototypes
void main(void);

// Globals
volatile unsigned int uartRxBufNdx;
volatile unsigned char uartRxBuf[RX_BUF_SIZE];
volatile unsigned int loopCount = 0;
void main(void) {
     WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    // Set System Clock
    DCOCTL = 0;                             // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;					// Set DCO
    DCOCTL = CALDCO_16MHZ;

    // ---- Push Button ----
    //  Pins Settings
    P1DIR &= ~BIT3;						// P1.3
    P1REN |= BIT3;						// P1.3 resistors enabled for input pins
    //  Interrupts Settings
    P1IE |= BIT3;						// P1.3 interrupt enabled
    P1IES |= BIT3;						// High-to-low edge on P1.3 Interrupt
    P1IFG &= ~BIT3;						// P1 Interrupt Flag cleared

    // ---- ADC Setup ----
    ADC10CTL1 = INCH_4 + ADC10DIV_7 + CONSEQ_0;         // Channel 4, ADC10CLK/8
    ADC10CTL0 = SREF_1 + REF2_5V + REFON + ADC10SHT_3 + ADC10ON + ADC10IE;  // Int. 2.5V & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
    ADC10AE0 |= BIT4;                         // ADC input enable P1.4

    // ---- LED ----
    P1DIR |= BIT0+BIT6;                     // P1.0 output
    P1OUT |= BIT0;

    // ---- Timer A0 ----
    CCTL0 = CCIE;                           // CCR0 interrupt enabled
    CCR0 = TIMERA0_OFFSET;                  // Set Timer A0 Offset
    TACTL = TASSEL_2 + MC_2 + ID_3;         // SMCLK, contmode

    __bis_SR_register(GIE);                 // Interrupts enabled

    // Declare Hardware Structs
    LCD lcd;
    ESP esp;

	// Variables
    volatile long pHadc;

    __delay_cycles(200);

    // Initialize Hardware
    delayMS(200);

    initUART(1);
    
	lcdInit(&lcd);
    
	P1OUT |= BIT6;

	initESP(&esp);
    P1OUT &= ~BIT6;

    // Main Loop
    while(1){

        unsigned int tempC;

        // Start measurements
        P1OUT |= BIT6;							// Green LED On
        //
        tempC = Read_Temperature();             // Read Temp;
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
        pHadc = ADC10MEM;						// Get ADC10MEM Value
        P1OUT &= ~BIT6;							// Green Led Off
        // End Measurements

        // Send Data
        delayMS(1);
        sendGET(1, pHadc);						// Transmit pH ADC Value
        delayMS(1);
        sendGET(0, tempC);
        updateLCD(lcd, uartRxBufNdx, uartRxBuf);

        // Scroll LCD
        volatile int k;
        for(k=0;k<(RX_BUF_SIZE-16);k++){
            updateLCD(lcd, k, uartRxBuf);
            delayMS(15);
        }

        loopCount++;

        updateLCDbottom(lcd, loopCount);

    }// forever while
}// main()


// ------------------ Interrupts ------------------------------------

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
        P1OUT ^= BIT0;
        CCR0 += TIMERA0_OFFSET;                 // Add Offset to CCR0
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);          // Clear CPUOFF bit from 0(SR)
}




