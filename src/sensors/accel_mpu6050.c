#include "accel_mpu6050.h"
#include "i2c_driver.h"

uint8_t mpu6050_init(void) {
  // Wake up the MPU6050 by writing 0 to the PWR_MGMT_1 register
  i2c1_write(MPU6050_I2C_ADDR, PWR_MGMT_1_ADDR, 0x00);

  // Read the WHO_AM_I register
  uint8_t who_am_i = i2c1_read(MPU6050_I2C_ADDR, WHO_AM_I_ADDR);

  // Check if the WHO_AM_I value matches the expected value (0x68)
  if (who_am_i == 0x68) {
    return 1; // Success
  } else {
    return 0; // Failure
  }
}