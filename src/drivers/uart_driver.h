#ifndef UART_DRIVER_H
#define UART_DRIVER_H
#include <stdint.h>

void uart2_init(void);

void uart2_write_byte(uint8_t byte);

void uart2_write_string(const char *str);

#endif