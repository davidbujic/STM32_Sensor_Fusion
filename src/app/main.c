#include "accel_mpu6050.h"
#include "board_config.h"
#include "i2c_driver.h"
#include "stm32f4xx.h"
#include <stdint.h>

void gpio_led_init(void) {
  RCC->AHB1ENR |= (1 << 3); // Enable GPIOD clock

  GPIOD->MODER &=
      ~((0b11 << USER_LED_GREEN_PIN * 2) |
        (0b11 << USER_LED_RED_PIN * 2)); // Clear bits for pins 12 and 14
  GPIOD->MODER |=
      ((0b01 << USER_LED_GREEN_PIN * 2) |
       (0b01 << USER_LED_RED_PIN * 2)); // Set pins 12 and 14 to output mode

  GPIOD->OTYPER &=
      ~((1 << USER_LED_GREEN_PIN) |
        (1 << USER_LED_RED_PIN)); // Set pins 12 and 14 to push-pull
}

int main(void) {
  gpio_led_init(); // Initialize GPIO for onboard LEDs
  i2c1_init();     // Initialize I2C1 for MPU6050 communication

  uint8_t mpu6050_status = mpu6050_init(); // Get MPU6050 initialization status

  if (mpu6050_status) {
    GPIOD->ODR |= (1 << USER_LED_GREEN_PIN); // Turn on green LED for success
    GPIOD->ODR &= ~(1 << USER_LED_RED_PIN);  // Turn off red LED
  } else {
    GPIOD->ODR |= (1 << USER_LED_RED_PIN);    // Turn on red LED for failure
    GPIOD->ODR &= ~(1 << USER_LED_GREEN_PIN); // Turn off green LED
  }

  while (1) {
  } // Infinite loop to keep the program running

  return 0; /* never reached - kept for -Wall cleanliness */
}
