#include "board_config.h"
#include "stm32f4xx.h"

void i2c1_init(void) {
  RCC->AHB1ENR |= (1 << 1);  // GPIOB clock enable
  RCC->APB1ENR |= (1 << 21); // I2C1 clock enable

  GPIOB->MODER &= ~(
      (0b11 << (MPU6050_I2C_SCL_PIN * 2)) |
      (0b11 << (MPU6050_I2C_SDA_PIN * 2))); // Clear bits for GPIOB pins 6 and 7
  GPIOB->MODER |= (0b10 << (MPU6050_I2C_SCL_PIN * 2)) |
                  (0b10 << (MPU6050_I2C_SDA_PIN *
                            2)); // Set pins 6 and 7 to alternate function mode

  GPIOB->OTYPER &=
      ~((1 << MPU6050_I2C_SCL_PIN) |
        (1 << MPU6050_I2C_SDA_PIN)); // Clear bits for GPIOB pins 6 and 7
  GPIOB->OTYPER |= (1 << MPU6050_I2C_SCL_PIN) |
                   (1 << MPU6050_I2C_SDA_PIN); // Set pins 6 and 7 to open-drain

  GPIOB->OSPEEDR &= ~(
      (0b11 << (MPU6050_I2C_SCL_PIN * 2)) |
      (0b11 << (MPU6050_I2C_SDA_PIN * 2))); // Clear bits for GPIOB pins 6 and 7
  GPIOB->OSPEEDR |=
      (0 << (MPU6050_I2C_SCL_PIN * 2)) |
      (0 << (MPU6050_I2C_SDA_PIN * 2)); // Set pins 6 and 7 to low speed

  GPIOB->PUPDR &= ~(
      (0b11 << (MPU6050_I2C_SCL_PIN * 2)) |
      (0b11 << (MPU6050_I2C_SDA_PIN * 2))); // Clear bits for GPIOB pins 6 and 7
  GPIOB->PUPDR |=
      (0b01 << (MPU6050_I2C_SCL_PIN * 2)) |
      (0b01 << (MPU6050_I2C_SDA_PIN * 2)); // Set pins 6 and 7 to pull-up mode

  GPIOB->AFR[0] &=
      ~((0b1111 << (MPU6050_I2C_SCL_PIN * 4)) |
        (0b1111 << (MPU6050_I2C_SDA_PIN *
                    4))); // Clear alternate function bits for pins 6 and 7
  GPIOB->AFR[0] |=
      (MPU6050_I2C_AF << (MPU6050_I2C_SCL_PIN * 4)) |
      (MPU6050_I2C_AF << (MPU6050_I2C_SDA_PIN *
                          4)); // Set alternate function for pins 6 and 7

  I2C1->CR2 &= ~(0b111111 << 0); // Clear bits for FREQ
  I2C1->CR2 |= 0x10;             // Set FREQ to 16 MHz

  I2C1->CCR &= ~(1 << 15);    // Clear bit for F/S (standard mode)
  I2C1->CCR &= ~(0xFFF << 0); // Clear bits for CCR
  I2C1->CCR |= 0x50;          // Set CCR to 80 (for 100 kHz)

  I2C1->TRISE &= ~(0b111111 << 0); // Clear bits for TRISE
  I2C1->TRISE |= 0x11;             // Set TRISE to 17 (for 100 kHz)

  I2C1->CR1 |= (1 << 0); // Enable I2C1
}

void i2c1_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t data) {
  I2C1->CR1 |= (1 << 8); // Generate START condition

  while (!(I2C1->SR1 & (1 << 0)))
    ; // Wait for SB (start bit) flag

  I2C1->DR = (slave_addr << 1); // Send slave address with write bit

  while (!(I2C1->SR1 & (1 << 1)))
    ; // Wait for ADDR (address sent) flag

  // Clear ADDR flag by reading SR1 and SR2
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  while (!(I2C1->SR1 & (1 << 7)))
    ; // Wait for TXE (data register empty) flag

  I2C1->DR = reg_addr; // Send register address

  while (!(I2C1->SR1 & (1 << 7)))
    ; // Wait for TXE (data register empty) flag

  I2C1->DR = data; // Send data

  while (!(I2C1->SR1 & (1 << 2)))
    ; // Wait for BTF (byte transfer finished) flag, after sending the last byte

  I2C1->CR1 |= (1 << 9); // Generate STOP condition
}

void i2c1_write_multiple_bytes(uint8_t slave_addr, uint8_t reg_addr,
                               uint8_t *data, uint8_t length) {
  I2C1->CR1 |= (1 << 8); // Generate START condition

  while (!(I2C1->SR1 & (1 << 0)))
    ; // Wait for SB (start bit) flag

  I2C1->DR = (slave_addr << 1); // Send slave address with write bit

  while (!(I2C1->SR1 & (1 << 1)))
    ; // Wait for ADDR (address sent) flag

  // Clear ADDR flag by reading SR1 and SR2
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  while (!(I2C1->SR1 & (1 << 7)))
    ; // Wait for TXE (data register empty) flag

  I2C1->DR = reg_addr; // Send register address

  for (uint8_t i = 0; i < length; i++) {
    while (!(I2C1->SR1 & (1 << 7)))
      ; // Wait for TXE (data register empty) flag

    I2C1->DR = data[i]; // Send data byte
  }

  while (!(I2C1->SR1 & (1 << 2)))
    ; // Wait for BTF (byte transfer finished) flag, after sending the last byte

  I2C1->CR1 |= (1 << 9); // Generate STOP condition
}

uint8_t i2c1_read(uint8_t slave_addr, uint8_t reg_addr) {
  I2C1->CR1 |= (1 << 8); // Generate START condition

  while (!(I2C1->SR1 & (1 << 0)))
    ; // Wait for SB (start bit) flag

  I2C1->DR = (slave_addr << 1); // Send slave address with write bit

  while (!(I2C1->SR1 & (1 << 1)))
    ; // Wait for ADDR (address sent) flag

  // Clear ADDR flag by reading SR1 and SR2
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  while (!(I2C1->SR1 & (1 << 7)))
    ; // Wait for TXE (data register empty) flag

  I2C1->DR = reg_addr; // Send register address

  while (!(I2C1->SR1 & (1 << 2)))
    ; // Wait for BTF (byte transfer finished) flag

  I2C1->CR1 |= (1 << 8); // Generate repeated START condition

  while (!(I2C1->SR1 & (1 << 0)))
    ; // Wait for SB (start bit) flag

  I2C1->DR = (slave_addr << 1) | 0x01; // Send slave address with read bit

  while (!(I2C1->SR1 & (1 << 1)))
    ; // Wait for ADDR (address sent) flag

  I2C1->CR1 &= ~(1 << 10); // Clear ACK bit, so that NACK is sent immediately
                           // after the next byte is received

  I2C1->CR1 |= (1 << 9); // Generate STOP condition

  // Clear ADDR flag by reading SR1 and SR2
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  while (!(I2C1->SR1 & (1 << 6)))
    ; // Wait for RXNE (data register not empty) flag

  return I2C1->DR; // Read and return the received data
}