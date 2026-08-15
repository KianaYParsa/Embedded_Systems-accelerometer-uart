#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "uart.h"

// RX circular buffer size.
#define RX_BUFFER_SIZE 64

// TX circular buffer size. We increased this to 256from 64 because every 200 ms, a max of 51 bytes are generated
// We need to be on the safe size and have more space
#define TX_BUFFER_SIZE 256

#define UART1_BRG_VALUE 467

volatile char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned int rx_head = 0;
volatile unsigned int rx_tail = 0;

volatile char tx_buffer[TX_BUFFER_SIZE];
volatile unsigned int tx_head = 0;
volatile unsigned int tx_tail = 0;

// Default values
unsigned char acc_bandwidth = 15;
unsigned char uart_frequency = 10;

char cmd_buffer[20];
unsigned int cmd_index = 0;
unsigned char receiving = 0;

/*
 * Computes the next index of a circular buffer.
 * If the index reaches the end, it wraps back to zero.
 */
unsigned int next_index(unsigned int index, unsigned int size)
{
    index++;

    if (index >= size)
    {
        index = 0;
    }

    return index;
}

void uart1_kick_tx(void)
{
    if ((tx_tail != tx_head) && (U1STAbits.UTXBF == 0))
    {
        U1TXREG = tx_buffer[tx_tail];
        tx_tail = next_index(tx_tail, TX_BUFFER_SIZE);
    }

    if (tx_tail != tx_head)
    {
        IEC0bits.U1TXIE = 1;
    }
}

/*
 * Adds one character to the TX circular buffer.
 */
void uart1_send_char(char c)
{
    unsigned int next;

    IEC0bits.U1TXIE = 0;

    next = next_index(tx_head, TX_BUFFER_SIZE);

    if (next != tx_tail)
    {
        tx_buffer[tx_head] = c;
        tx_head = next;
    }

    uart1_kick_tx();
}

/*
 * Sends a null-terminated string through UART1.
 */
void uart1_send_string(const char *s)
{
    while (*s != '\0')
    {
        uart1_send_char(*s);
        s++;
    }
}

/*
 * unsigned integer parser.
 *
 * Accepts only digits.
 * Rejects:
 * - empty strings
 * - negative numbers
 * - decimal numbers
 * - alphabetic characters
 */
static unsigned char parse_number(const char *s, unsigned char *value)
{
    unsigned int result = 0;

    if (*s == '\0')
    {
        return 0;
    }

    while (*s != '\0')
    {
        if (*s < '0' || *s > '9')
        {
            return 0;
        }

        result = result * 10 + (*s - '0');

        if (result > 255)
        {
            return 0;
        }

        s++;
    }

    *value = (unsigned char)result;
    return 1;
}

/*
 * Sends the error message
 */
static void send_error(void)
{
    uart1_send_string("$ERR,1*");
}

/*
 * Handles a complete command.
 *
 * 1. $BW,xx*
 * Valid xx values: 8 to 15
 *
 * 2. $HZ,yy*
 * Valid yy values: 0, 1, 2, 5, 10
 *
 * Invalid values generate:
 * $ERR,1*
 */
static void handle_command(char *cmd)
{
    unsigned char value;

    if (cmd[0] == 'B' && cmd[1] == 'W' && cmd[2] == ',')
    {
        if (parse_number(&cmd[3], &value) && value >= 8 && value <= 15)
        {
            acc_bandwidth = value;
        }
        else
        {
            send_error();
        }

        return;
    }

    if (cmd[0] == 'H' && cmd[1] == 'Z' && cmd[2] == ',')
    {
        if (parse_number(&cmd[3], &value) &&
            (value == 0 || value == 1 || value == 2 || value == 5 || value == 10))
        {
            uart_frequency = value;
        }
        else
        {
            send_error();
        }

        return;
    }

    send_error();
}

/*
 * Initializes UART1.
 */
void uart1_init(void)
{
    // Unlock PPS registers.
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPINR18bits.U1RXR = 75;
    RPOR0bits.RP64R = 1;

    //  Lock PPS registers again.
    __builtin_write_OSCCONL(OSCCON | 0x40);

    U1MODE = 0;
    U1STA = 0;
    U1BRG = UART1_BRG_VALUE;

    /*
     * STSEL = 0 -> 1 stop bit
     * PDSEL = 0 -> 8-bit data, no parity
     * BRGH  = 0 -> low-speed baud mode
     * ABAUD = 0 -> auto-baud disabled
     * UEN   = 0 -> only U1TX and U1RX are used
     */
    U1MODEbits.STSEL = 0;
    U1MODEbits.PDSEL = 0;
    U1MODEbits.BRGH = 0;
    U1MODEbits.ABAUD = 0;
    U1MODEbits.UEN = 0;

    /*
     * RX interrupt mode:
     * Interrupt when a character is received.
     */
    U1STAbits.URXISEL = 0;

    /*
     * TX interrupt mode:
     * Interrupt when at least one position is free in TX buffer.
     */
    U1STAbits.UTXISEL0 = 0;
    U1STAbits.UTXISEL1 = 0;

    // Clear any old received data.
    while (U1STAbits.URXDA)
    {
        char dummy = U1RXREG;
        (void)dummy;
    }

    // Clear overrun error.
    U1STAbits.OERR = 0;

    // Clear interrupt flags before enabling interrupts.
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;

    /*
     * Enable RX interrupt.
     * TX interrupt is enabled only when there is something to send.
     */
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1TXIE = 0;

    // Enable UART module and UART transmitter.
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
}

/*
 * Processes received UART data.
 */
void uart1_process_commands(void)
{
    char c;

    while (rx_tail != rx_head)
    {
        c = rx_buffer[rx_tail];
        rx_tail = next_index(rx_tail, RX_BUFFER_SIZE);

        if (c == '$')
        {
            receiving = 1;
            cmd_index = 0;
        }

        /*
         * Store command characters until '*' is received.
         */
        else if (receiving)
        {
            if (c == '*')
            {
                cmd_buffer[cmd_index] = '\0';
                receiving = 0;

                handle_command(cmd_buffer);
            }
            else
            {
                if (cmd_index < sizeof(cmd_buffer) - 1)
                {
                    cmd_buffer[cmd_index] = c;
                    cmd_index++;
                }

                else
                {
                    receiving = 0;
                    cmd_index = 0;
                    send_error();
                }
            }
        }
    }
}

/*
 * UART1 RX Interrupt Service Routine.
 */
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    unsigned int next;
    char c;

    /*
     * Clear RX interrupt flag.
     */
    IFS0bits.U1RXIF = 0;

    /*
     * Read all available received characters.
     */
    while (U1STAbits.URXDA)
    {
        c = U1RXREG;
        next = next_index(rx_head, RX_BUFFER_SIZE);

        /*
         * Store character only if buffer is not full.
         * If full, the character is discarded.
         */
        if (next != rx_tail)
        {
            rx_buffer[rx_head] = c;
            rx_head = next;
        }
    }

    /*
     * Clear overrun error if it happens.
     * Without this, UART reception would stop.
     */
    if (U1STAbits.OERR)
    {
        U1STAbits.OERR = 0;
    }
}

/*
 * UART1 TX Interrupt Service Routine.
 */
void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void)
{
    /*
     * Clear TX interrupt flag.
     */
    IFS0bits.U1TXIF = 0;

    /*
     * Fill the UART TX register while it can accept data.
     */
    while ((U1STAbits.UTXBF == 0) && (tx_tail != tx_head))
    {
        U1TXREG = tx_buffer[tx_tail];
        tx_tail = next_index(tx_tail, TX_BUFFER_SIZE);
    }

    /*
     * If there is no more data to send, disable TX interrupt.
     * It will be enabled again when new data is added.
     */
    if (tx_tail == tx_head)
    {
        IEC0bits.U1TXIE = 0;
    }
}