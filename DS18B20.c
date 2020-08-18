#include <msp430.h>
#include "DS18B20.h"


// Drivers to access DS1822 Temperature Sensor over a 1-wire comm bus from:
// https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/162.html


//////////////////////////////////////////////////////////////////////////////
// OW_RESET - performs a reset on the one-wire bus and
// returns the presence detect. Reset is 480us
//
unsigned char ow_reset(void)
{
    unsigned char presence;
    // pull DQ low to start timeslot
    DS_PxDIR |= DS_DQ_BIT;   // Set DQ as Output
    DS_PxOUT &= ~DS_DQ_BIT;  //pull DQ line low

    __delay_cycles(490*US_MULTIPLIER); // leave it low for atleast 480us
    // allow line to return high by external pullup
    DS_PxDIR &= ~DS_DQ_BIT;   // Release DQ line to allow pull up or READ
    __delay_cycles(100*US_MULTIPLIER); // wait for presence 70us
    presence = DS_PxIN & DS_DQ_BIT; // get presence signal
    __delay_cycles(380*US_MULTIPLIER); // wait for end of timeslot
    return(presence); // presence signal returned
} // 0=presence, 1 = no part


//////////////////////////////////////////////////////////////////////////////
// WRITE_BIT - writes a bit to the one-wire bus, passed in bitval.
//
void write_bit(char bitval)
{

    // pull DQ low to start timeslot
    DS_PxDIR |= DS_DQ_BIT;   // Set DQ as Output
    DS_PxOUT &= ~DS_DQ_BIT;  //pull DQ line low to initiate start condition
    __delay_cycles(US_MULTIPLIER); // hold write condition for 1us

    if(bitval==1){
        //return DQ high if write 1
        DS_PxDIR &= ~DS_DQ_BIT;   // Release DQ line to allow pull up or READ
    }
    __delay_cycles(104*US_MULTIPLIER); // hold value for remainder of timeslot
    DS_PxDIR &= ~DS_DQ_BIT;   // Release DQ line to allow pull up or READ
}//


//////////////////////////////////////////////////////////////////////////////
// WRITE_BYTE - writes a byte to the one-wire bus.
//
void write_byte(char val)
{
    unsigned char i;
    unsigned char temp;
    for (i=0; i<8; i++) // writes byte, one bit at a time
    {
        temp = val>>i; // shifts val right 'i' spaces
        temp &= 0x01; // copy that bit to temp
        write_bit(temp); // write bit in temp into
    }
    __delay_cycles(104*US_MULTIPLIER);
}


//////////////////////////////////////////////////////////////////////////////
// READ_BIT - reads a bit from the one-wire bus. The delay
// required for a read is 15us, so the DELAY routine won't work.
// We put our own delay function in this routine in the form of a
// for() loop.
//
unsigned char read_bit(void)
{
    unsigned char readBit;

    DS_PxDIR |= DS_DQ_BIT;   // Set DQ as Output
    DS_PxOUT &= ~DS_DQ_BIT;  //pull DQ line low to initiate READ condition
    __delay_cycles(US_MULTIPLIER); // hold write condition for 1us
    DS_PxDIR &= ~DS_DQ_BIT;   // Release DQ line to allow pull up or READ

    __delay_cycles(14*US_MULTIPLIER); // delay 14us from start of timeslot

    readBit = DS_PxIN & DS_DQ_BIT;

    return(readBit); // return value of DQ line
}

//////////////////////////////////////////////////////////////////////////////
// READ_BYTE - reads a byte from the one-wire bus.
//
unsigned char read_byte(void)
{
    unsigned char i;
    unsigned char value = 0;
    for (i=0;i<8;i++)
    {
        if(read_bit()) value|=0x01<<i; // reads byte in, one byte at a time and then
        // shifts it left
        __delay_cycles(120*US_MULTIPLIER); // wait for rest of timeslot
    }
    return(value);
}



unsigned int Read_Temperature(void)
{
    char get[9];
    char temp_lsb,temp_msb;
    unsigned int tempOut = 0;
    volatile int k, intC = 0;
    ow_reset();
    write_byte(0xCC); // Skip ROM
    write_byte(0x44); // Start Conversion
    __delay_cycles(750*US_MULTIPLIER);
    ow_reset();
    write_byte(0xCC); // Skip ROM
    write_byte(0xBE); // Read Scratch Pad
    for (k=0;k<9;k++){get[k]=read_byte();}
    temp_msb = get[1]; // Sign byte + lsbit
    temp_lsb = get[0]; // Temp data plus lsb
    intC = ((temp_msb<<4) & 0x70) + ((temp_lsb>>4) & 0x0F);
    // Temp out Format: 2^[S S S S  S 6 5 4  3 2 1 0 -1 -2 -3 -4]
    tempOut = (temp_msb<<8) + temp_lsb;

       return tempOut;
}



