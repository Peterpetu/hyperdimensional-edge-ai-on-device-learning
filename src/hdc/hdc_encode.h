/**
 * @file    hdc_encode.h
 * @brief   HDC Encoding Functions - Declarations
 * @version 2.0.0
 * @note    Converts analog sensor values to hypervectors
 *
 * @details This header provides declarations for HDC encoding functions.
 *          Implementation is in hdc_encode.c (Engineer/JPL standard practice).
 *          Thermometer encoding maps continuous values to binary hypervectors
 *          in a way that preserves similarity relationships.
 */

#ifndef HDC_ENCODE_H
#define HDC_ENCODE_H

#include <stdint.h>
#include "hdc_core.h"

/* =============================================================================
 * Constants
 * ========================================================================== */

/** @brief Number of thermometer encoding levels (matches HV_DIMENSIONS) */
#define THERMO_LEVELS       128U

/** @brief Maximum ADC value (10-bit ADC) */
#define ADC_MAX             1023U

/* =============================================================================
 * Function Declarations - Basic Encoding
 * ========================================================================== */

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
void hdc_encode_thermometer(hv_t hv, uint16_t value, uint16_t max_value);

/**
 * @brief   Encode an ADC value (0-1023) to hypervector
 * @param   hv Output hypervector
 * @param   adc_value 10-bit ADC reading
 */
void hdc_encode_adc(hv_t hv, uint16_t adc_value);

/**
 * @brief   Encode a bipolar (signed) value
 * @param   hv Output hypervector
 * @param   value Signed input value
 * @param   min_val Minimum expected value
 * @param   max_val Maximum expected value
 *
 * @note    Useful for temperature readings or other signed sensors
 */
void hdc_encode_bipolar(hv_t hv, int16_t value, int16_t min_val, int16_t max_val);

/* =============================================================================
 * Function Declarations - Multi-Channel Encoding
 * ========================================================================== */

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
    const hv_t* basis_vectors);

#endif /* HDC_ENCODE_H */
