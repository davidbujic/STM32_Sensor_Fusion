#ifndef ACCEL_MPU6050_H
#define ACCEL_MPU6050_H
#include "board_config.h"
#include <stdint.h>

#define PWR_MGMT_1_ADDR 0x6B
#define WHO_AM_I_ADDR 0x75

uint8_t mpu6050_init(void);

#endif /* ACCEL_MPU6050_H */