#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H
#include <stdint.h>

/**
 * @brief Initialize SPI1
 *
 * This function initializes the SPI1 peripheral.
 */
void spi1_init(void);

/**
 * @brief Transfer a single byte over SPI1
 *
 * This function transfers a single byte over the SPI1 peripheral.
 * It sends the provided byte and simultaneously receives a byte from the slave
 * device.
 * @param data The byte to send
 * @return The byte received from the slave device
 */
uint8_t spi1_transfer_byte(uint8_t data);

#endif