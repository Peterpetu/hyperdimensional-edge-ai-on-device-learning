/**
 * @file    hal.h
 * @brief   HAL - Master Include File
 * @version 1.0.0
 * @note    Target: ATmega328P (Arduino Uno R3) @ 16MHz
 */

#ifndef HAL_H
#define HAL_H

#include "hal_gpio.h"
#include "hal_uart.h"
#include "hal_adc.h"

#define HAL_VERSION_MAJOR   1U
#define HAL_VERSION_MINOR   0U
#define HAL_VERSION_PATCH   0U

#define HAL_TARGET_MCU      "ATmega328P"
#define HAL_TARGET_BOARD    "Arduino Uno R3"
#define HAL_TARGET_CLOCK    16000000UL

static inline void hal_init(void)
{
    hal_gpio_init();
    hal_uart_init();
    hal_adc_init();
}

#endif /* HAL_H */
