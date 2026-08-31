#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>

/**
 * @brief Initialize I2C1: enable clocks, configure PB6/PB7 as I2C1 AF,
 *        configure CR2/CCR/TRISE for 100kHz standard mode, enable peripheral.
 */
void i2c1_init(void);

/**
 * @brief Write a single byte to a device register.
 * @param slave_addr  7-bit slave address (not pre-shifted).
 * @param reg_addr    Register address to write to.
 * @param data        Byte to write.
 */
void i2c1_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);

/**
 * @brief Write multiple consecutive bytes starting at a device register.
 * @param slave_addr  7-bit slave address (not pre-shifted).
 * @param reg_addr    Starting register address.
 * @param data        Buffer of bytes to write.
 * @param length      Number of bytes to write.
 */
void i2c1_write_multiple_bytes(uint8_t slave_addr, uint8_t reg_addr,
                               uint8_t *data, uint8_t length);

/**
 * @brief Read a single byte from a device register (uses repeated START).
 * @param slave_addr  7-bit slave address (not pre-shifted).
 * @param reg_addr    Register address to read from.
 * @return            The byte read.
 */
uint8_t i2c1_read(uint8_t slave_addr, uint8_t reg_addr);

/**
 * @brief Read multiple consecutive bytes starting at a device register
 *        (uses repeated START; caller supplies the destination buffer).
 * @param slave_addr  7-bit slave address (not pre-shifted).
 * @param reg_addr    Starting register address.
 * @param data        Caller-supplied buffer to write received bytes into.
 * @param length      Number of bytes to read.
 */
void i2c1_read_multiple_bytes(uint8_t slave_addr, uint8_t reg_addr,
                              uint8_t *data, uint8_t length);

#endif /* I2C_DRIVER_H */