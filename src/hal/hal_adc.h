/**
 * @file    hal_adc.h
 * @brief   HAL - ADC (Analog-to-Digital Converter)
 * @version 2.0.0
 * @note    Target: ATmega328P, 10-bit resolution, 125kHz clock
 *
 * @details All blocking functions include timeout guards per Engineer/JPL
 *          coding standard requirement for bounded loops.
 */

#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>

#define ADC_CHANNEL_0       0U
#define ADC_CHANNEL_1       1U
#define ADC_CHANNEL_2       2U
#define ADC_CHANNEL_3       3U
#define ADC_CHANNEL_4       4U
#define ADC_CHANNEL_5       5U
#define ADC_CHANNEL_TEMP    8U
#define ADC_CHANNEL_BANDGAP 14U
#define ADC_CHANNEL_GND     15U
#define ADC_CHANNEL_MAX     5U

#define ADC_REF_AREF        0U
#define ADC_REF_AVCC        1U
#define ADC_REF_INTERNAL    3U

#define ADC_MAX_VALUE       1023U
#define ADC_VREF_MV         5000U

/** @brief Default timeout for ADC conversion (~1ms at 125kHz ADC clock) */
#define ADC_DEFAULT_TIMEOUT 10000U

/** @brief Error value returned on timeout (impossible ADC value) */
#define ADC_ERROR_VALUE     0xFFFFU

typedef uint8_t adc_channel_t;

typedef enum {
    ADC_OK = 0,
    ADC_ERROR_INVALID_CHANNEL,
    ADC_ERROR_TIMEOUT
} adc_status_t;

static inline void hal_adc_init(void)
{
    ADMUX = (1U << REFS0);
    ADCSRA = (1U << ADEN) | (1U << ADPS2) | (1U << ADPS1) | (1U << ADPS0);
}

static inline void hal_adc_set_reference(uint8_t ref)
{
    ADMUX = (ADMUX & 0x3FU) | ((ref & 0x03U) << REFS0);
}

/**
 * @brief   Read ADC with timeout
 * @param   channel ADC channel to read
 * @param   p_value Pointer to store result
 * @param   timeout Maximum iterations to wait (0 = use default)
 * @return  ADC_OK on success, ADC_ERROR_TIMEOUT if conversion didn't complete
 * @pre     hal_adc_init() must be called first
 */
static inline adc_status_t hal_adc_read_timeout(adc_channel_t channel, uint16_t* p_value, uint16_t timeout)
{
    uint16_t counter = (timeout == 0U) ? ADC_DEFAULT_TIMEOUT : timeout;

    ADMUX = (ADMUX & 0xF0U) | (channel & 0x0FU);
    ADCSRA |= (1U << ADSC);

    while ((ADCSRA & (1U << ADSC)) != 0U) {
        if (counter == 0U) {
            if (p_value != NULL) {
                *p_value = ADC_ERROR_VALUE;
            }
            return ADC_ERROR_TIMEOUT;
        }
        counter--;
    }

    if (p_value != NULL) {
        *p_value = ADC;
    }
    return ADC_OK;
}

/**
 * @brief   Read ADC channel (blocking with default timeout)
 * @param   channel ADC channel (0-5)
 * @return  ADC value (0-1023), or ADC_ERROR_VALUE on timeout
 * @pre     hal_adc_init() must be called first
 */
static inline uint16_t hal_adc_read(adc_channel_t channel)
{
    uint16_t value = ADC_ERROR_VALUE;
    (void)hal_adc_read_timeout(channel, &value, 0U);
    return value;
}

static inline uint16_t hal_adc_read_averaged(adc_channel_t channel, uint8_t samples)
{
    uint32_t sum = 0U;
    for (uint8_t i = 0U; i < samples; i++) {
        sum += hal_adc_read(channel);
    }
    return (uint16_t)(sum / samples);
}

static inline uint16_t hal_adc_to_millivolts(uint16_t adc_value)
{
    return (uint16_t)(((uint32_t)adc_value * ADC_VREF_MV) / 1024UL);
}

static inline int16_t hal_adc_to_temp_celsius(uint16_t adc_value)
{
    uint16_t mv = hal_adc_to_millivolts(adc_value);
    return (int16_t)((mv - 500U) / 10U);
}

#endif /* HAL_ADC_H */
