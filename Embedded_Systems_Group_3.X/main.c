#include <xc.h>
#include <stdio.h>
#include <math.h>

#include "timer.h"
#include "uart.h"
#include "spi.h"
#include "imu.h"

extern unsigned char uart_frequency; 
extern unsigned char acc_bandwidth;

// Task 1: Simulate a 7 ms algorithm
void algorithm(void)
{
    tmr_wait_ms(TIMER2, 7);
}

int main(void)
{
    int ret = 0;
    
    // Initializing counters
    int ld2_counter = 0;
    int acc_axes_counter = 0;
    int uart_acc_counter = 0;
    int uart_ang_counter = 0;
    
    unsigned char current_bandwidth = 0; 
    char uartBuffer[64]; 
    AxesRaw_t accel_data = {0, 0, 0}; 

    // Hardware Initialization
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000; // Disable Analog
    
    TRISGbits.TRISG9 = 0;
    LATGbits.LATG9 = 0;

    setup_spi();
    imu_init();
    uart1_init();

    __builtin_enable_interrupts();
    tmr_setup_period(TIMER1, 10); // 100 Hz Loop
    
    while (1)
    {
        /* --- Task 1: Algorithm --- */
        algorithm();

        /* --- Task 3: UART Processing --- */
        uart1_process_commands();
        
        // Handle Bandwidth changes dynamically
        if (acc_bandwidth != current_bandwidth) 
        {
            accel_write_reg(0x10, acc_bandwidth);
            current_bandwidth = acc_bandwidth; 
        }

        /* --- Task 2: Blinking LED (1 Hz) --- */
        ld2_counter++;
        if (ld2_counter >= 50)
        {
            LATGbits.LATG9 = !LATGbits.LATG9;
            ld2_counter = 0;
        }
        
        /* --- Task 4: Acquire Accelerometer (50 Hz) --- */
        acc_axes_counter++;
        if (acc_axes_counter >= 2) 
        {
            acc_axes_counter = 0;
            accel_data = accel_read_xyz(); 
        }

        /* --- Task 6: Send ACC to UART --- */
        if (uart_frequency > 0) 
        {
            int acc_ticks_threshold = 100 / uart_frequency; 
            uart_acc_counter++;
            
            if (uart_acc_counter >= acc_ticks_threshold) 
            {
                uart_acc_counter = 0; 
                sprintf(uartBuffer, "$ACC,%d,%d,%d*\r\n", accel_data.x, accel_data.y, accel_data.z);
                uart1_send_string(uartBuffer);
            }
        } 
        else 
        {
            uart_acc_counter = 0;
        }

        /* --- Task 5 and 7: Calculate and Send Angles to UART --- */
        uart_ang_counter++;
        if (uart_ang_counter >= 20)
        {
            uart_ang_counter = 0;

            double ax = (double)accel_data.x;
            double ay = (double)accel_data.y;
            double az = (double)accel_data.z;
            
            // This is based on the formula provided in IMU datasheet
            // As per our research, there is another standard equation for roll and pitch that should be used
            // For the sake of this assignment, we are sticking to the formula from datasheet
            double pitch = atan2(sqrt(ax*ax + ay*ay), az) * (180.0 / M_PI);
            double roll  = atan2(-ay, ax) * (180.0 / M_PI);
            
            sprintf(uartBuffer, "$ANG,%.2f,%.2f*\r\n", roll, pitch);
            uart1_send_string(uartBuffer);
        }

        ret = tmr_wait_period(TIMER1);
        (void)ret;
    }

    return 0;
}