#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

void i2c1_init(void);

void i2c1_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);

void i2c1_write_multiple_bytes(uint8_t slave_addr, uint8_t reg_addr,
                               uint8_t *data, uint8_t length);

uint8_t i2c1_read(uint8_t slave_addr, uint8_t reg_addr);

void i2c1_read_multiple_bytes(uint8_t slave_addr, uint8_t reg_addr,
                              uint8_t *data, uint8_t length);

#endif /* I2C_DRIVER_H */