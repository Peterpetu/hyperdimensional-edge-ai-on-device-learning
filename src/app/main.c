/**
 * @file    main.c
 * @brief   Nano-Edge AI Project - ADC Demo with HAL
 * @version 2.0.0
 * @note    Target: ATmega328P (Arduino Uno R3)
 *
 * @details This application demonstrates reading analog values via the
 *          Hardware Abstraction Layer (HAL) and displaying them over UART.
 *          All hardware access goes through the HAL for portability.
 */

#include <stdint.h>
#include <util/delay.h>

#include "hal/hal.h"

/**
 * @brief   Print a visual bar graph representation of a value
 * @param   value   ADC value (0-1023)
 * @param   max_val Maximum value for scaling (typically 1024)
 * @pre     hal_uart_init() must be called first
 */
static void print_bar_graph(uint16_t value, uint16_t max_val)
{
    const uint8_t BAR_WIDTH = 32U;
    uint8_t bars = (uint8_t)(((uint32_t)value * BAR_WIDTH) / max_val);

    hal_uart_putc('[');
    for (uint8_t i = 0U; i < BAR_WIDTH; i++) {
        hal_uart_putc((i < bars) ? '#' : ' ');
    }
    hal_uart_putc(']');
}

/**
 * @brief   Print a number with right-alignment padding
 * @param   value   Value to print
 * @param   width   Minimum field width (pads with spaces)
 * @pre     hal_uart_init() must be called first
 */
static void print_padded_u16(uint16_t value, uint8_t width)
{
    uint8_t digits = 1U;
    uint16_t temp = value;

    while (temp >= 10U) {
        temp /= 10U;
        digits++;
    }

    while (digits < width) {
        hal_uart_putc(' ');
        digits++;
    }

    hal_uart_print_u16(value);
}

/**
 * @brief   Application entry point
 * @return  int (never returns in embedded)
 */
int main(void)
{
    /* Initialize all HAL modules */
    hal_init();

    /* Configure LED pin as output */
    hal_gpio_set_direction(GPIO_PIN_LED, GPIO_DIR_OUTPUT);

    /* Print startup banner */
    hal_uart_newline();
    hal_uart_puts("========================================\r\n");
    hal_uart_puts("Nano-Edge AI Project v2.0\r\n");
    hal_uart_puts("ADC Demo - Using HAL Layer\r\n");
    hal_uart_puts("========================================\r\n");
    hal_uart_newline();

    uint16_t count = 0U;

    /* Main application loop */
    while (1) {
        /* Toggle LED to show activity */
        (void)hal_gpio_toggle(GPIO_PIN_LED);

        /* Read ADC with averaging for stability */
        uint16_t raw = hal_adc_read_averaged(ADC_CHANNEL_0, 4U);
        uint16_t mv = hal_adc_to_millivolts(raw);

        count++;

        /* Print formatted output */
        hal_uart_putc('#');
        print_padded_u16(count, 4U);
        hal_uart_puts("  Raw:");
        print_padded_u16(raw, 4U);
        hal_uart_puts("  (");
        print_padded_u16(mv, 4U);
        hal_uart_puts("mV)  ");
        print_bar_graph(raw, 1024U);
        hal_uart_newline();

        /* Variable delay based on ADC reading */
        uint16_t delay = 100U + (raw / 4U);
        for (uint16_t i = 0U; i < delay; i++) {
            _delay_ms(1);
        }
    }

    return 0;
}
