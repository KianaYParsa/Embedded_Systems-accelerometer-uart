#include <xc.h>
#include <stdint.h>
#include "imu.h"
#include "spi.h"
#include "timer.h"

// SPI Defines
#define MAG_CS LATDbits.LATD6    
#define MAG_CS_DIR TRISDbits.TRISD6

#define ACCEL_CS LATBbits.LATB3  
#define ACCEL_CS_DIR TRISBbits.TRISB3

#define GYRO_CS LATBbits.LATB4   
#define GYRO_CS_DIR TRISBbits.TRISB4


void imu_init(void) {
    ACCEL_CS_DIR = 0; 
    GYRO_CS_DIR = 0;  
    MAG_CS_DIR = 0;   
    ACCEL_CS = 1;
    GYRO_CS = 1;      
    MAG_CS = 1;  
}

void accel_write_reg(unsigned char addr, unsigned char value) {
    ACCEL_CS = 0;
    spi_write(addr & 0x7F); 
    spi_write(value);
    ACCEL_CS = 1;
}

AxesRaw_t accel_read_xyz(void) {
    AxesRaw_t accel;
    unsigned int lsb, msb;

    ACCEL_CS = 0;
    
    // Send the start address (0x02 for ACC) with the Read bit (0x80)
    spi_write(0x02 | 0x80); 
    
    // Read x-axis
    lsb = spi_write(0x00);
    msb = spi_write(0x00);
    lsb = lsb & 0x00F0;
    accel.x = (int)((msb << 8) | lsb) >> 4; 
    
    // Read y-axis
    lsb = spi_write(0x00);
    msb = spi_write(0x00);
    lsb = lsb & 0x00F0;
    accel.y = (int)((msb << 8) | lsb) >> 4; 
    
    // Read z-axis
    lsb = spi_write(0x00);
    msb = spi_write(0x00);
    lsb = lsb & 0x00F0;
    accel.z = (int)((msb << 8) | lsb) >> 4; 

    ACCEL_CS = 1;

    return accel;
}