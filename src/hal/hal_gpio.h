/**
 * @file    hal_gpio.h
 * @brief   HAL - GPIO (General Purpose I/O)
 * @version 1.0.0
 * @note    Target: ATmega328P (Arduino Uno R3)
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

/* Pin definitions (Arduino Uno mapping) */
#define GPIO_PIN_D0     0U
#define GPIO_PIN_D1     1U
#define GPIO_PIN_D2     2U
#define GPIO_PIN_D3     3U
#define GPIO_PIN_D4     4U
#define GPIO_PIN_D5     5U
#define GPIO_PIN_D6     6U
#define GPIO_PIN_D7     7U
#define GPIO_PIN_D8     8U
#define GPIO_PIN_D9     9U
#define GPIO_PIN_D10    10U
#define GPIO_PIN_D11    11U
#define GPIO_PIN_D12    12U
#define GPIO_PIN_D13    13U
#define GPIO_PIN_LED    GPIO_PIN_D13

#define GPIO_DIR_INPUT  0U
#define GPIO_DIR_OUTPUT 1U
#define GPIO_STATE_LOW  0U
#define GPIO_STATE_HIGH 1U

typedef uint8_t gpio_pin_t;
typedef uint8_t gpio_dir_t;
typedef uint8_t gpio_state_t;

typedef enum {
    GPIO_OK = 0,
    GPIO_ERROR_INVALID_PIN
} gpio_status_t;

static inline volatile uint8_t* gpio_get_port_reg(gpio_pin_t pin)
{
    return (pin < 8U) ? &PORTD : &PORTB;
}

static inline volatile uint8_t* gpio_get_ddr_reg(gpio_pin_t pin)
{
    return (pin < 8U) ? &DDRD : &DDRB;
}

static inline volatile uint8_t* gpio_get_pin_reg(gpio_pin_t pin)
{
    return (pin < 8U) ? &PIND : &PINB;
}

static inline uint8_t gpio_get_bit(gpio_pin_t pin)
{
    return (pin < 8U) ? pin : (pin - 8U);
}

static inline void hal_gpio_init(void) { }

static inline gpio_status_t hal_gpio_set_direction(gpio_pin_t pin, gpio_dir_t direction)
{
    if (pin > GPIO_PIN_D13) {
        return GPIO_ERROR_INVALID_PIN;
    }

    volatile uint8_t* ddr = gpio_get_ddr_reg(pin);
    uint8_t bit = gpio_get_bit(pin);

    if (direction == GPIO_DIR_OUTPUT) {
        *ddr |= (1U << bit);
    } else {
        *ddr &= ~(1U << bit);
    }

    return GPIO_OK;
}

static inline gpio_status_t hal_gpio_write(gpio_pin_t pin, gpio_state_t state)
{
    if (pin > GPIO_PIN_D13) {
        return GPIO_ERROR_INVALID_PIN;
    }

    volatile uint8_t* port = gpio_get_port_reg(pin);
    uint8_t bit = gpio_get_bit(pin);

    if (state == GPIO_STATE_HIGH) {
        *port |= (1U << bit);
    } else {
        *port &= ~(1U << bit);
    }

    return GPIO_OK;
}

static inline gpio_state_t hal_gpio_read(gpio_pin_t pin)
{
    volatile uint8_t* pinreg = gpio_get_pin_reg(pin);
    uint8_t bit = gpio_get_bit(pin);

    return ((*pinreg & (1U << bit)) != 0U) ? GPIO_STATE_HIGH : GPIO_STATE_LOW;
}

static inline gpio_status_t hal_gpio_toggle(gpio_pin_t pin)
{
    if (pin > GPIO_PIN_D13) {
        return GPIO_ERROR_INVALID_PIN;
    }

    volatile uint8_t* port = gpio_get_port_reg(pin);
    uint8_t bit = gpio_get_bit(pin);

    *port ^= (1U << bit);

    return GPIO_OK;
}

static inline gpio_status_t hal_gpio_set_pullup(gpio_pin_t pin, bool enable)
{
    return hal_gpio_write(pin, enable ? GPIO_STATE_HIGH : GPIO_STATE_LOW);
}

#endif /* HAL_GPIO_H */
