/**
 * @file    hdc_core.h
 * @brief   Hyperdimensional Computing Core Operations - Declarations
 * @version 2.0.0
 * @note    128-bit hypervectors (16 bytes), optimized for ATmega328P
 *
 * @details This header provides declarations for HDC core operations.
 *          Implementation is in hdc_core.c (Engineer/JPL standard practice).
 *          Separating declarations from implementations enables:
 *          - Proper code coverage measurement
 *          - Cleaner module interfaces
 *          - Better compile-time dependency management
 */

#ifndef HDC_CORE_H
#define HDC_CORE_H

#include <stdint.h>

/* =============================================================================
 * Constants
 * ========================================================================== */

/** @brief Number of dimensions in hypervector (bits) */
#define HV_DIMENSIONS   128U

/** @brief Size of hypervector in bytes */
#define HV_BYTES        16U

/* =============================================================================
 * Types
 * ========================================================================== */

/** @brief Hypervector type - 128-bit binary vector stored as byte array */
typedef uint8_t hv_t[HV_BYTES];

/* =============================================================================
 * Function Declarations - Population Count
 * ========================================================================== */

/**
 * @brief   Count set bits in a single byte (population count)
 * @param   byte Input byte
 * @return  Number of bits set to 1 (0-8)
 * @note    Uses parallel bit counting algorithm for efficiency
 */
uint8_t hdc_popcount8(uint8_t byte);

/**
 * @brief   Count total set bits in a hypervector
 * @param   hv Input hypervector
 * @return  Total number of bits set to 1 (0-128)
 */
uint8_t hdc_popcount(const hv_t hv);

/* =============================================================================
 * Function Declarations - Bitwise Operations
 * ========================================================================== */

/**
 * @brief   XOR two hypervectors (binding operation)
 * @param   result Output hypervector
 * @param   a First input hypervector
 * @param   b Second input hypervector
 * @note    XOR is the primary binding operation in HDC
 */
void hdc_xor(hv_t result, const hv_t a, const hv_t b);

/**
 * @brief   OR two hypervectors
 * @param   result Output hypervector
 * @param   a First input hypervector
 * @param   b Second input hypervector
 */
void hdc_or(hv_t result, const hv_t a, const hv_t b);

/**
 * @brief   AND two hypervectors
 * @param   result Output hypervector
 * @param   a First input hypervector
 * @param   b Second input hypervector
 */
void hdc_and(hv_t result, const hv_t a, const hv_t b);

/**
 * @brief   Bundle (accumulate) a pattern into memory using OR
 * @param   memory Hypervector memory to accumulate into
 * @param   pattern Pattern to add
 * @note    This is a simplified bundling using OR (saturating)
 */
void hdc_bundle(hv_t memory, const hv_t pattern);

/* =============================================================================
 * Function Declarations - Distance and Similarity
 * ========================================================================== */

/**
 * @brief   Calculate Hamming distance between two hypervectors
 * @param   a First hypervector
 * @param   b Second hypervector
 * @return  Hamming distance (0-128)
 * @note    Lower distance = more similar
 */
uint8_t hdc_hamming(const hv_t a, const hv_t b);

/**
 * @brief   Calculate similarity between two hypervectors
 * @param   a First hypervector
 * @param   b Second hypervector
 * @return  Similarity score (0-128, higher = more similar)
 */
uint8_t hdc_similarity(const hv_t a, const hv_t b);

/* =============================================================================
 * Function Declarations - Memory Operations
 * ========================================================================== */

/**
 * @brief   Clear a hypervector (set all bits to 0)
 * @param   hv Hypervector to clear
 */
void hdc_clear(hv_t hv);

/**
 * @brief   Fill a hypervector with a byte value
 * @param   hv Hypervector to fill
 * @param   value Byte value to fill with
 */
void hdc_fill(hv_t hv, uint8_t value);

/**
 * @brief   Copy a hypervector
 * @param   dest Destination hypervector
 * @param   src Source hypervector
 */
void hdc_copy(hv_t dest, const hv_t src);

/* =============================================================================
 * Function Declarations - Transformations
 * ========================================================================== */

/**
 * @brief   Circular permutation of a hypervector
 * @param   hv Hypervector to permute (modified in place)
 * @param   shifts Number of bit positions to shift
 * @note    Permutation creates orthogonal vectors for sequence encoding
 */
void hdc_permute(hv_t hv, uint8_t shifts);

#endif /* HDC_CORE_H */
