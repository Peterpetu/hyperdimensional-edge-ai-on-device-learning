/**
 * @file    mock_hal.h
 * @brief   Mock Hardware Abstraction Layer for Unit Testing
 * @version 1.0.0
 *
 * @details This mock HAL allows running unit tests on a PC without actual
 *          Arduino hardware. It provides controllable return values and
 *          call tracking for verification.
 *
 * @note    Include this INSTEAD of hal/hal.h when running tests.
 */

#ifndef MOCK_HAL_H
#define MOCK_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================================
 * Mock Configuration
 * ============================================================================ */

#define MOCK_ADC_CHANNELS   8U
#define MOCK_GPIO_PINS      20U
#define MOCK_UART_BUFFER    256U

/* ============================================================================
 * Types (matching real HAL)
 * ============================================================================ */

typedef uint8_t gpio_pin_t;
typedef uint8_t gpio_dir_t;
typedef uint8_t gpio_state_t;
typedef uint8_t adc_channel_t;

#define GPIO_DIR_INPUT      0U
#define GPIO_DIR_OUTPUT     1U
#define GPIO_STATE_LOW      0U
#define GPIO_STATE_HIGH     1U
#define GPIO_PIN_LED        13U

#define ADC_CHANNEL_0       0U
#define ADC_MAX_VALUE       1023U
#define ADC_ERROR_VALUE     0xFFFFU

typedef enum { GPIO_OK = 0, GPIO_ERROR_INVALID_PIN } gpio_status_t;
typedef enum { ADC_OK = 0, ADC_ERROR_INVALID_CHANNEL, ADC_ERROR_TIMEOUT } adc_status_t;
typedef enum { UART_OK = 0, UART_ERROR_TIMEOUT, UART_ERROR_OVERFLOW } uart_status_t;

/* ============================================================================
 * Mock State (for test control and verification)
 * ============================================================================ */

typedef struct {
    /* GPIO mock state */
    gpio_dir_t   gpio_directions[MOCK_GPIO_PINS];
    gpio_state_t gpio_states[MOCK_GPIO_PINS];
    uint32_t     gpio_write_count;
    uint32_t     gpio_read_count;
    uint32_t     gpio_toggle_count;

    /* ADC mock state */
    uint16_t     adc_values[MOCK_ADC_CHANNELS];
    uint32_t     adc_read_count;
    bool         adc_timeout_enabled;

    /* UART mock state */
    char         uart_tx_buffer[MOCK_UART_BUFFER];
    uint16_t     uart_tx_index;
    uint32_t     uart_putc_count;
    bool         uart_timeout_enabled;

    /* Initialization tracking */
    bool         gpio_initialized;
    bool         uart_initialized;
    bool         adc_initialized;

} mock_hal_state_t;

/* Global mock state (defined in test file) */
extern mock_hal_state_t g_mock_hal;

/* ============================================================================
 * Mock Control Functions (for test setup)
 * ============================================================================ */

/**
 * @brief   Reset all mock state to defaults
 */
static inline void mock_hal_reset(void)
{
    memset(&g_mock_hal, 0, sizeof(g_mock_hal));

    /* Set default ADC values to mid-range */
    for (uint8_t i = 0U; i < MOCK_ADC_CHANNELS; i++) {
        g_mock_hal.adc_values[i] = 512U;
    }
}

/**
 * @brief   Set mock ADC value for a channel
 */
static inline void mock_adc_set_value(adc_channel_t channel, uint16_t value)
{
    if (channel < MOCK_ADC_CHANNELS) {
        g_mock_hal.adc_values[channel] = value;
    }
}

/**
 * @brief   Enable/disable ADC timeout simulation
 */
static inline void mock_adc_set_timeout(bool enable)
{
    g_mock_hal.adc_timeout_enabled = enable;
}

/**
 * @brief   Get UART TX buffer contents
 */
static inline const char* mock_uart_get_tx_buffer(void)
{
    return g_mock_hal.uart_tx_buffer;
}

/**
 * @brief   Clear UART TX buffer
 */
static inline void mock_uart_clear_tx_buffer(void)
{
    memset(g_mock_hal.uart_tx_buffer, 0, MOCK_UART_BUFFER);
    g_mock_hal.uart_tx_index = 0U;
}

/* ============================================================================
 * Mock HAL Implementations
 * ============================================================================ */

/* GPIO */
static inline void hal_gpio_init(void)
{
    g_mock_hal.gpio_initialized = true;
}

static inline gpio_status_t hal_gpio_set_direction(gpio_pin_t pin, gpio_dir_t direction)
{
    if (pin >= MOCK_GPIO_PINS) {
        return GPIO_ERROR_INVALID_PIN;
    }
    g_mock_hal.gpio_directions[pin] = direction;
    return GPIO_OK;
}

static inline gpio_status_t hal_gpio_write(gpio_pin_t pin, gpio_state_t state)
{
    if (pin >= MOCK_GPIO_PINS) {
        return GPIO_ERROR_INVALID_PIN;
    }
    g_mock_hal.gpio_states[pin] = state;
    g_mock_hal.gpio_write_count++;
    return GPIO_OK;
}

static inline gpio_state_t hal_gpio_read(gpio_pin_t pin)
{
    if (pin >= MOCK_GPIO_PINS) {
        return GPIO_STATE_LOW;
    }
    g_mock_hal.gpio_read_count++;
    return g_mock_hal.gpio_states[pin];
}

static inline gpio_status_t hal_gpio_toggle(gpio_pin_t pin)
{
    if (pin >= MOCK_GPIO_PINS) {
        return GPIO_ERROR_INVALID_PIN;
    }
    g_mock_hal.gpio_states[pin] = (g_mock_hal.gpio_states[pin] == GPIO_STATE_HIGH)
                                  ? GPIO_STATE_LOW : GPIO_STATE_HIGH;
    g_mock_hal.gpio_toggle_count++;
    return GPIO_OK;
}

/* ADC */
static inline void hal_adc_init(void)
{
    g_mock_hal.adc_initialized = true;
}

static inline uint16_t hal_adc_read(adc_channel_t channel)
{
    if (g_mock_hal.adc_timeout_enabled) {
        return ADC_ERROR_VALUE;
    }
    if (channel >= MOCK_ADC_CHANNELS) {
        return ADC_ERROR_VALUE;
    }
    g_mock_hal.adc_read_count++;
    return g_mock_hal.adc_values[channel];
}

static inline uint16_t hal_adc_read_averaged(adc_channel_t channel, uint8_t samples)
{
    /* For mock: just return the set value (no averaging needed) */
    (void)samples;
    return hal_adc_read(channel);
}

static inline uint16_t hal_adc_to_millivolts(uint16_t adc_value)
{
    return (uint16_t)(((uint32_t)adc_value * 5000UL) / 1024UL);
}

/* UART */
static inline void hal_uart_init(void)
{
    g_mock_hal.uart_initialized = true;
}

static inline void hal_uart_putc(uint8_t data)
{
    if (g_mock_hal.uart_tx_index < (MOCK_UART_BUFFER - 1U)) {
        g_mock_hal.uart_tx_buffer[g_mock_hal.uart_tx_index] = (char)data;
        g_mock_hal.uart_tx_index++;
        g_mock_hal.uart_tx_buffer[g_mock_hal.uart_tx_index] = '\0';
    }
    g_mock_hal.uart_putc_count++;
}

static inline void hal_uart_puts(const char* str)
{
    while (*str != '\0') {
        hal_uart_putc((uint8_t)*str);
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

static inline void hal_uart_print_hex8(uint8_t byte)
{
    static const char hex[] = "0123456789ABCDEF";
    hal_uart_putc(hex[(byte >> 4) & 0x0FU]);
    hal_uart_putc(hex[byte & 0x0FU]);
}

/* Master init */
static inline void hal_init(void)
{
    hal_gpio_init();
    hal_uart_init();
    hal_adc_init();
}

#endif /* MOCK_HAL_H */
