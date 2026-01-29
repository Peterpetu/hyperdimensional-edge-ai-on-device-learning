/**
 * @file    hal_uart.h
 * @brief   HAL - UART Serial Communication
 * @version 2.0.0
 * @note    Target: ATmega328P @ 16MHz, default 9600 8N1
 *
 * @details All blocking functions include timeout guards per Engineer/JPL
 *          coding standard requirement for bounded loops.
 */

#ifndef HAL_UART_H
#define HAL_UART_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE  9600UL
#endif

#define UART_UBRR_VALUE ((F_CPU / (16UL * UART_BAUD_RATE)) - 1UL)

/** @brief Default timeout in loop iterations (~10ms at 16MHz) */
#define UART_DEFAULT_TIMEOUT    50000U

typedef enum {
    UART_OK = 0,
    UART_ERROR_TIMEOUT,
    UART_ERROR_OVERFLOW
} uart_status_t;

static inline void hal_uart_init(void)
{
    UBRR0H = (uint8_t)(UART_UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UART_UBRR_VALUE);
    UCSR0B = (1U << TXEN0) | (1U << RXEN0);
    UCSR0C = (1U << UCSZ01) | (1U << UCSZ00);
}

static inline bool hal_uart_tx_ready(void)
{
    return (UCSR0A & (1U << UDRE0)) != 0U;
}

static inline bool hal_uart_rx_available(void)
{
    return (UCSR0A & (1U << RXC0)) != 0U;
}

/**
 * @brief   Send a single byte with timeout
 * @param   data    Byte to transmit
 * @param   timeout Maximum iterations to wait (0 = use default)
 * @return  UART_OK on success, UART_ERROR_TIMEOUT if TX not ready in time
 * @pre     hal_uart_init() must be called first
 */
static inline uart_status_t hal_uart_putc_timeout(uint8_t data, uint16_t timeout)
{
    uint16_t counter = (timeout == 0U) ? UART_DEFAULT_TIMEOUT : timeout;

    while (!hal_uart_tx_ready()) {
        if (counter == 0U) {
            return UART_ERROR_TIMEOUT;
        }
        counter--;
    }

    UDR0 = data;
    return UART_OK;
}

/**
 * @brief   Send a single byte (blocking with default timeout)
 * @param   data    Byte to transmit
 * @pre     hal_uart_init() must be called first
 * @note    Uses default timeout; ignores timeout errors for simplicity
 */
static inline void hal_uart_putc(uint8_t data)
{
    (void)hal_uart_putc_timeout(data, 0U);
}

/**
 * @brief   Receive a single byte with timeout
 * @param   p_data  Pointer to store received byte
 * @param   timeout Maximum iterations to wait (0 = use default)
 * @return  UART_OK on success, UART_ERROR_TIMEOUT if no data received in time
 * @pre     hal_uart_init() must be called first
 */
static inline uart_status_t hal_uart_getc_timeout(uint8_t* p_data, uint16_t timeout)
{
    uint16_t counter = (timeout == 0U) ? UART_DEFAULT_TIMEOUT : timeout;

    while (!hal_uart_rx_available()) {
        if (counter == 0U) {
            return UART_ERROR_TIMEOUT;
        }
        counter--;
    }

    if (p_data != NULL) {
        *p_data = UDR0;
    }
    return UART_OK;
}

/**
 * @brief   Receive a single byte (blocking with default timeout)
 * @return  Received byte, or 0 on timeout
 * @pre     hal_uart_init() must be called first
 * @note    Uses default timeout; returns 0 on timeout
 */
static inline uint8_t hal_uart_getc(void)
{
    uint8_t data = 0U;
    (void)hal_uart_getc_timeout(&data, 0U);
    return data;
}

static inline void hal_uart_puts(const char* str)
{
    while (*str != '\0') {
        hal_uart_putc((uint8_t)*str);
        str++;
    }
}

static inline void hal_uart_puts_P(const char* str)
{
    char c;
    while ((c = pgm_read_byte(str)) != '\0') {
        hal_uart_putc((uint8_t)c);
        str++;
    }
}

static inline void hal_uart_newline(void)
{
    hal_uart_putc('\r');
    hal_uart_putc('\n');
}

static inline void hal_uart_print_u16(uint16_t num)
{
    char buf[6];
    char* p = &buf[5];
    *p = '\0';

    if (num == 0U) {
        hal_uart_putc('0');
        return;
    }

    while (num > 0U) {
        p--;
        *p = '0' + (char)(num % 10U);
        num /= 10U;
    }

    hal_uart_puts(p);
}

static inline void hal_uart_print_i16(int16_t num)
{
    if (num < 0) {
        hal_uart_putc('-');
        num = -num;
    }
    hal_uart_print_u16((uint16_t)num);
}

static inline void hal_uart_print_hex8(uint8_t byte)
{
    static const char hex[] = "0123456789ABCDEF";
    hal_uart_putc(hex[(byte >> 4) & 0x0FU]);
    hal_uart_putc(hex[byte & 0x0FU]);
}

static inline void hal_uart_print_hex16(uint16_t value)
{
    hal_uart_print_hex8((uint8_t)(value >> 8));
    hal_uart_print_hex8((uint8_t)(value & 0xFFU));
}

static inline void hal_uart_print_hv128(const uint8_t* hv)
{
    for (uint8_t i = 0U; i < 16U; i++) {
        hal_uart_print_hex8(hv[i]);
    }
    hal_uart_newline();
}

#endif /* HAL_UART_H */
