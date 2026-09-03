#ifndef UART_DRIVER_H
#define UART_DRIVER_H
#include <stdint.h>

/**
 * @brief Initialize UART2
 *
 * This function initializes the UART2 peripheral.
 */
void uart2_init(void);

/**
 * @brief Write a single byte to UART2
 *
 * This function writes a single byte to the UART2 peripheral.
 * @param byte The byte to write
 */
void uart2_write_byte(uint8_t byte);

/**
 * @brief Write a string to UART2
 *
 * This function writes a string to the UART2 peripheral.
 * @param str The string to write
 */
void uart2_write_string(const char *str);

#endif