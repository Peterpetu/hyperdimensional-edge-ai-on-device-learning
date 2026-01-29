/**
 * @file    hdc_core.c
 * @brief   Hyperdimensional Computing Core Operations - Implementation
 * @version 2.0.0
 * @note    128-bit hypervectors (16 bytes), optimized for ATmega328P
 *
 * @details This file contains the implementation of HDC core operations.
 *          Separating implementation from declarations follows Engineer/JPL
 *          coding standards and enables proper code coverage measurement.
 */

#include "hdc_core.h"
#include <string.h>

/**
 * @brief   Count set bits in a single byte (population count)
 * @param   byte Input byte
 * @return  Number of bits set to 1 (0-8)
 * @note    Uses parallel bit counting algorithm for efficiency
 */
uint8_t hdc_popcount8(uint8_t byte)
{
    byte = byte - ((byte >> 1) & 0x55U);
    byte = (byte & 0x33U) + ((byte >> 2) & 0x33U);
    return (byte + (byte >> 4)) & 0x0FU;
}

/**
 * @brief   Count total set bits in a hypervector
 * @param   hv Input hypervector
 * @return  Total number of bits set to 1 (0-128)
 */
uint8_t hdc_popcount(const hv_t hv)
{
    uint8_t count = 0U;
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        count += hdc_popcount8(hv[i]);
    }
    return count;
}

/**
 * @brief   XOR two hypervectors (binding operation)
 * @param   result Output hypervector
 * @param   a First input hypervector
 * @param   b Second input hypervector
 * @note    XOR is the primary binding operation in HDC
 */
void hdc_xor(hv_t result, const hv_t a, const hv_t b)
{
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        result[i] = a[i] ^ b[i];
    }
}

/**
 * @brief   OR two hypervectors
 * @param   result Output hypervector
 * @param   a First input hypervector
 * @param   b Second input hypervector
 */
void hdc_or(hv_t result, const hv_t a, const hv_t b)
{
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        result[i] = a[i] | b[i];
    }
}

/**
 * @brief   AND two hypervectors
 * @param   result Output hypervector
 * @param   a First input hypervector
 * @param   b Second input hypervector
 */
void hdc_and(hv_t result, const hv_t a, const hv_t b)
{
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        result[i] = a[i] & b[i];
    }
}

/**
 * @brief   Bundle (accumulate) a pattern into memory using OR
 * @param   memory Hypervector memory to accumulate into
 * @param   pattern Pattern to add
 * @note    This is a simplified bundling using OR (saturating)
 */
void hdc_bundle(hv_t memory, const hv_t pattern)
{
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        memory[i] |= pattern[i];
    }
}

/**
 * @brief   Calculate Hamming distance between two hypervectors
 * @param   a First hypervector
 * @param   b Second hypervector
 * @return  Hamming distance (0-128)
 * @note    Lower distance = more similar
 */
uint8_t hdc_hamming(const hv_t a, const hv_t b)
{
    uint8_t distance = 0U;
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        distance += hdc_popcount8(a[i] ^ b[i]);
    }
    return distance;
}

/**
 * @brief   Calculate similarity between two hypervectors
 * @param   a First hypervector
 * @param   b Second hypervector
 * @return  Similarity score (0-128, higher = more similar)
 */
uint8_t hdc_similarity(const hv_t a, const hv_t b)
{
    return HV_DIMENSIONS - hdc_hamming(a, b);
}

/**
 * @brief   Clear a hypervector (set all bits to 0)
 * @param   hv Hypervector to clear
 */
void hdc_clear(hv_t hv)
{
    memset(hv, 0, HV_BYTES);
}

/**
 * @brief   Fill a hypervector with a byte value
 * @param   hv Hypervector to fill
 * @param   value Byte value to fill with
 */
void hdc_fill(hv_t hv, uint8_t value)
{
    memset(hv, value, HV_BYTES);
}

/**
 * @brief   Copy a hypervector
 * @param   dest Destination hypervector
 * @param   src Source hypervector
 */
void hdc_copy(hv_t dest, const hv_t src)
{
    memcpy(dest, src, HV_BYTES);
}

/**
 * @brief   Circular permutation of a hypervector
 * @param   hv Hypervector to permute (modified in place)
 * @param   shifts Number of bit positions to shift
 * @note    Permutation creates orthogonal vectors for sequence encoding
 */
void hdc_permute(hv_t hv, uint8_t shifts)
{
    if (shifts == 0U) {
        return;
    }

    shifts = shifts % HV_DIMENSIONS;
    uint8_t byte_shift = shifts / 8U;
    uint8_t bit_shift = shifts % 8U;

    hv_t temp;
    hdc_clear(temp);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        uint8_t dest_byte = (i + byte_shift) % HV_BYTES;

        if (bit_shift == 0U) {
            temp[dest_byte] = hv[i];
        } else {
            temp[dest_byte] |= (hv[i] << bit_shift);
            uint8_t next_byte = (dest_byte + 1U) % HV_BYTES;
            temp[next_byte] |= (hv[i] >> (8U - bit_shift));
        }
    }

    hdc_copy(hv, temp);
}
