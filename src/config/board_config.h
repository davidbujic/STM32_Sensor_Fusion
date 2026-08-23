/**
 * @file    board_config.h
 * @brief   Board-specific pin/peripheral mapping for STM32F407VG Discovery.
 *
 * This is the ONLY file that should need to change if this codebase is
 * ever ported to a different board or moved from Renode to real hardware.
 * Every driver (i2c_driver, spi_driver, uart_driver) should read pin and
 * peripheral choices from here, never hardcode a GPIO port/pin inline.
 *
 * Fill in the register base addresses / bit definitions once you've pulled
 * them from the STM32F407 reference manual (RM0090) and the Discovery
 * board's schematic. Placeholders below show the shape to fill in.
 */

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <stdint.h>

/* ---------------------------------------------------------------------------
 * System clock
 * -------------------------------------------------------------------------*/
#define SYSCLK_FREQ_HZ 16000000UL // default HSI

/* ---------------------------------------------------------------------------
 * I2C1 - MPU6050 (accelerometer + gyroscope)
 * Discovery board default: I2C1 on PB6 (SCL) / PB7 (SDA), AF4
 * -------------------------------------------------------------------------*/
#define MPU6050_I2C_INSTANCE I2C1
#define MPU6050_I2C_GPIO_PORT GPIOB
#define MPU6050_I2C_SCL_PIN 6
#define MPU6050_I2C_SDA_PIN 7
#define MPU6050_I2C_AF 4
#define MPU6050_I2C_ADDR 0x68U // MPU6050 default 7-bit addr, AD0=0
#define MPU6050_I2C_CLOCK_HZ                                                   \
  100000UL // standard mode; bump to 400000 for fast mode

/* ---------------------------------------------------------------------------
 * SPI - secondary peripheral (e.g. SPI flash/EEPROM)
 * Discovery board default: SPI1 on PA5 (SCK) / PA6 (MISO) / PA7 (MOSI), AF5
 * TODO: pick and confirm a CS pin (any free GPIO, software-controlled)
 * -------------------------------------------------------------------------*/
#define SPI_FLASH_INSTANCE SPI1
#define SPI_FLASH_GPIO_PORT GPIOA
#define SPI_FLASH_SCK_PIN 5
#define SPI_FLASH_MISO_PIN 6
#define SPI_FLASH_MOSI_PIN 7
#define SPI_FLASH_AF 5
#define SPI_FLASH_CS_PORT GPIOA
#define SPI_FLASH_CS_PIN 4 // software-controlled chip select

/* ---------------------------------------------------------------------------
 * UART - debug/fusion output
 * Discovery board default: USART2 on PA2 (TX) / PA3 (RX), AF7
 * (PA2/PA3 are also routed to the onboard ST-LINK VCP on many Discovery
 * boards - convenient, no extra wiring needed even on real hardware later)
 * -------------------------------------------------------------------------*/
#define DEBUG_UART_INSTANCE USART2
#define DEBUG_UART_GPIO_PORT GPIOA
#define DEBUG_UART_TX_PIN 2
#define DEBUG_UART_RX_PIN 3
#define DEBUG_UART_AF 7
#define DEBUG_UART_BAUD 115200UL

/* ---------------------------------------------------------------------------
 * Onboard user LED (STM32F407 Discovery has 4: PD12-PD15)
 * Handy for a first "blink equivalent" sanity check before sensors work.
 * -------------------------------------------------------------------------*/
#define USER_LED_PORT GPIOD
#define USER_LED_GREEN_PIN 12
#define USER_LED_ORANGE_PIN 13
#define USER_LED_RED_PIN 14
#define USER_LED_BLUE_PIN 15

/* ---------------------------------------------------------------------------
 * Interrupt priorities
 * Keep these centralized so NVIC setup in main.c and any driver ISR
 * enabling code agree on priorities at a glance.
 * -------------------------------------------------------------------------*/
#define NVIC_PRIO_I2C1_EV 5
#define NVIC_PRIO_I2C1_ER 5
#define NVIC_PRIO_SPI1 6
#define NVIC_PRIO_USART2 6

#endif /* BOARD_CONFIG_H */