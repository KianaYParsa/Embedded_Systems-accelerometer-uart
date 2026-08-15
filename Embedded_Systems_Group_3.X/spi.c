#include <xc.h>
#include "spi.h"

void setup_spi(void) {
    // Unlock PPS
    __builtin_write_OSCCONL(OSCCON & 0xBF); 

    TRISAbits.TRISA1 = 1;
    TRISFbits.TRISF12 = 0;
    TRISFbits.TRISF13 = 0;
    
    RPINR20bits.SDI1R = 0b0010001; 
    RPOR12bits.RP109R = 0b000101;  
    RPOR11bits.RP108R = 0b000110;  
    
    // Lock PPS
    __builtin_write_OSCCONL(OSCCON | 0x40); 
    
    SPI1CON1bits.MSTEN = 1;  
    SPI1CON1bits.MODE16 = 0; 
    SPI1CON1bits.PPRE = 0b00;
    SPI1CON1bits.SPRE = 0b000;
    SPI1CON1bits.CKP = 1;    
    SPI1CON1bits.CKE = 0;    
    SPI1CON1bits.SMP = 0;    
    
    SPI1STATbits.SPIROV = 0; 
    SPI1STATbits.SPIEN = 1;  
}

unsigned int spi_write(unsigned int data) {
    unsigned int value;
    while(SPI1STATbits.SPITBF == 1); 
    SPI1BUF = data;                  
    while(SPI1STATbits.SPIRBF == 0);
    value = SPI1BUF;
    return value;                  
}