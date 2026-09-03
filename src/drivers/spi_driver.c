#include "spi_driver.h"
#include "board_config.h"
#include "stm32f4xx.h"

void spi1_init(void) {
  RCC->AHB1ENR |= (1 << 0);  // GPIOA clock enable
  RCC->APB2ENR |= (1 << 12); // SPI1 clock enable

  GPIOA->MODER &=
      ~((0b11 << (SPI_FLASH_SCK_PIN * 2)) | (0b11 << (SPI_FLASH_MISO_PIN * 2)) |
        (0b11 << (SPI_FLASH_MOSI_PIN * 2))); // Clear bits for PA5, PA6, and PA7
  GPIOA->MODER |=
      ((0b10 << (SPI_FLASH_SCK_PIN * 2)) | (0b10 << (SPI_FLASH_MISO_PIN * 2)) |
       (0b10 << (SPI_FLASH_MOSI_PIN *
                 2))); // Set PA5, PA6, and PA7 to alternate function mode

  GPIOA->OTYPER &= ~((1 << SPI_FLASH_SCK_PIN) | (1 << SPI_FLASH_MISO_PIN) |
                     (1 << SPI_FLASH_MOSI_PIN)); // Clear bits for PA5, PA6, and
                                                 // PA7 and set to push-pull

  GPIOA->AFR[0] &=
      ~((0b1111 << (SPI_FLASH_SCK_PIN * 4)) |
        (0b1111 << (SPI_FLASH_MISO_PIN * 4)) |
        (0b1111 << (SPI_FLASH_MOSI_PIN *
                    4))); // Clear alternate function bits for PA5, PA6, and PA7
  GPIOA->AFR[0] |=
      ((SPI_FLASH_AF << (SPI_FLASH_SCK_PIN * 4)) |
       (SPI_FLASH_AF << (SPI_FLASH_MISO_PIN * 4)) |
       (SPI_FLASH_AF << (SPI_FLASH_MOSI_PIN *
                         4))); // Set alternate function for PA5, PA6, and PA7

  GPIOA->MODER &= ~(0b11 << (SPI_FLASH_CS_PIN * 2)); // Clear bits for CS pin
  GPIOA->MODER |= (0b01 << (SPI_FLASH_CS_PIN * 2)); // Set CS pin to output mode
  GPIOA->OTYPER &= ~(1 << SPI_FLASH_CS_PIN);        // Set CS pin to push-pull
  GPIOA->OSPEEDR &= ~(0b11 << (SPI_FLASH_CS_PIN * 2)); // Reset speed for CS pin
  GPIOA->OSPEEDR |= (0 << (SPI_FLASH_CS_PIN * 2)); // Set CS pin to low speed
  GPIOA->ODR |= (1 << SPI_FLASH_CS_PIN);           // Set CS high (inactive)

  SPI1->CR1 = (1 << 2) | (0b011 << 3) | (1 << 8) |
              (1 << 9); // Master mode, baud rate to fPCLK/16, software slave
                        // management, SSI=1
  SPI1->CR1 &=
      ~((1 << 0) | (1 << 1) | (1 << 7) |
        (1 << 11)); // CPOL=0, CPHA=0 (mode 0), MSB first, 8-bit data frame

  SPI1->CR1 |= (1 << 6); // SPI enable
}

uint8_t spi1_transfer_byte(uint8_t data) {
  while (!(SPI1->SR & (1 << 1)))
    ; // Wait for TXE (transmit buffer empty) flag

  SPI1->DR = data; // Send the byte

  while (!(SPI1->SR & (1 << 0)))
    ; // Wait for RXNE (receive buffer not empty) flag

  return SPI1->DR; // Return the received byte
}