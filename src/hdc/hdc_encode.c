/**
 * @file    hdc_encode.c
 * @brief   HDC Encoding Functions - Implementation
 * @version 2.0.0
 * @note    Converts analog sensor values to hypervectors
 *
 * @details This file contains the implementation of HDC encoding functions.
 *          Thermometer encoding maps continuous values to binary hypervectors
 *          in a way that preserves similarity relationships.
 */

#include "hdc_encode.h"

/**
 * @brief   Encode a value using thermometer encoding
 * @param   hv Output hypervector
 * @param   value Input value to encode
 * @param   max_value Maximum possible input value
 *
 * @details Thermometer encoding sets bits from LSB up to a level
 *          proportional to the input value. Similar values have
 *          similar hypervectors (small Hamming distance).
 *
 * Example (8-bit simplified):
 *   value=0   -> 00000000
 *   value=25% -> 00000011
 *   value=50% -> 00001111
 *   value=75% -> 00111111
 *   value=100%-> 11111111
 */
void hdc_encode_thermometer(hv_t hv, uint16_t value, uint16_t max_value)
{
    hdc_clear(hv);

    if (value >= max_value) {
        hdc_fill(hv, 0xFFU);
        return;
    }

    uint8_t level = (uint8_t)(((uint32_t)value * THERMO_LEVELS) / max_value);

    uint8_t full_bytes = level / 8U;
    uint8_t remaining_bits = level % 8U;

    for (uint8_t i = 0U; i < full_bytes; i++) {
        hv[i] = 0xFFU;
    }

    if ((full_bytes < HV_BYTES) && (remaining_bits > 0U)) {
        hv[full_bytes] = (1U << remaining_bits) - 1U;
    }
}

/**
 * @brief   Encode an ADC value (0-1023) to hypervector
 * @param   hv Output hypervector
 * @param   adc_value 10-bit ADC reading
 */
void hdc_encode_adc(hv_t hv, uint16_t adc_value)
{
    hdc_encode_thermometer(hv, adc_value, ADC_MAX);
}

/**
 * @brief   Encode a bipolar (signed) value
 * @param   hv Output hypervector
 * @param   value Signed input value
 * @param   min_val Minimum expected value
 * @param   max_val Maximum expected value
 *
 * @note    Useful for temperature readings or other signed sensors
 */
void hdc_encode_bipolar(hv_t hv, int16_t value, int16_t min_val, int16_t max_val)
{
    uint16_t range = (uint16_t)(max_val - min_val);
    uint16_t shifted = (uint16_t)(value - min_val);
    hdc_encode_thermometer(hv, shifted, range);
}

/**
 * @brief   Encode multiple sensor channels into a single hypervector
 * @param   result Output combined hypervector
 * @param   values Array of sensor values
 * @param   num_channels Number of channels to encode
 * @param   basis_vectors Array of basis vectors for each channel
 *
 * @details Each channel is encoded with thermometer encoding, then
 *          XORed with its unique basis vector (binding), then all
 *          channels are bundled together using OR.
 */
void hdc_encode_multi_channel(
    hv_t result,
    const uint16_t* values,
    uint8_t num_channels,
    const hv_t* basis_vectors)
{
    hdc_clear(result);
    hv_t temp;

    for (uint8_t ch = 0U; ch < num_channels; ch++) {
        hdc_encode_adc(temp, values[ch]);
        hdc_xor(temp, temp, basis_vectors[ch]);
        hdc_bundle(result, temp);
    }
}
