#ifndef UART_TASK3_H
#define UART_TASK3_H

void uart1_init(void);

void uart1_send_string(const char *s);

void uart1_process_commands(void);

#endif