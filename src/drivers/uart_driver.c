#include "uart_driver.h"
#include "board_config.h"
#include "stm32f4xx.h"

void uart2_init(void) {
  RCC->AHB1ENR |= (1 << 0);  // GPIOA clock enable
  RCC->APB1ENR |= (1 << 17); // USART2 clock enable

  GPIOA->MODER &=
      ~((0b11 << (DEBUG_UART_TX_PIN * 2)) |
        (0b11 << (DEBUG_UART_RX_PIN * 2))); // Clear bits for PA2 and PA3
  GPIOA->MODER |= ((0b10 << (DEBUG_UART_TX_PIN * 2)) |
                   (0b10 << (DEBUG_UART_RX_PIN *
                             2))); // Set PA2 and PA3 to alternate function mode

  GPIOA->OTYPER &= ~((1 << DEBUG_UART_TX_PIN) |
                     (1 << DEBUG_UART_RX_PIN)); // Clear bits for PA2 and PA3
                                                // and set to push-pull

  GPIOA->AFR[0] &=
      ~((0b1111 << (DEBUG_UART_TX_PIN * 4)) |
        (0b1111 << (DEBUG_UART_RX_PIN *
                    4))); // Clear alternate function bits for PA2 and PA3
  GPIOA->AFR[0] |=
      ((DEBUG_UART_AF << (DEBUG_UART_TX_PIN * 4)) |
       (DEBUG_UART_AF << (DEBUG_UART_RX_PIN * 4))); // Set alternate function
                                                    // for PA2 and PA3

  USART2->BRR =
      (8 << 4) | 11; // Set baud rate to 115200 (assuming 16 MHz clock)

  USART2->CR1 |= (1 << 3); // Transmitter enable

  USART2->CR1 |= (1 << 13); // USART enable
}

void uart2_write_byte(uint8_t byte) {
  while (!(USART2->SR & (1 << 7)))
    ; // Wait for TXE (transmit data register empty) flag

  USART2->DR = byte; // Write the byte to the data register
}

void uart2_write_string(const char *str) {
  while (*str != '\0') {
    uart2_write_byte((uint8_t)(*str)); // Write each character
    str++;
  }

  while (!(USART2->SR & (1 << 6)))
    ; // Wait for TC (transmission complete) flag
}