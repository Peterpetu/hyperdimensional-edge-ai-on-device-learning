/**
 * @file    test_hdc_core.c
 * @brief   Unit Tests for HDC Core and Encoding Operations
 * @version 2.0.0
 *
 * @details Comprehensive tests for hypervector operations:
 *          - Core: XOR, OR, AND, bundle, popcount, permute
 *          - Distance: hamming, similarity
 *          - Encoding: thermometer, ADC, bipolar, multi-channel
 *
 * @note    Uses Unity test framework. Run with: pio test -e native
 *          For coverage: pio test -e coverage
 *
 * @see     Engineer/JPL testing standards require 80%+ code coverage
 */

#include <unity.h>
#include <stdint.h>
#include <string.h>

/* Include the HDC headers (they're portable - no AVR dependencies) */
#include "hdc/hdc_core.h"
#include "hdc/hdc_encode.h"

/* ============================================================================
 * Test Setup and Teardown
 * ============================================================================ */

void setUp(void)
{
    /* Called before each test */
}

void tearDown(void)
{
    /* Called after each test */
}

/* ============================================================================
 * Popcount Tests
 * ============================================================================ */

void test_popcount8_zero(void)
{
    TEST_ASSERT_EQUAL_UINT8(0U, hdc_popcount8(0x00U));
}

void test_popcount8_all_ones(void)
{
    TEST_ASSERT_EQUAL_UINT8(8U, hdc_popcount8(0xFFU));
}

void test_popcount8_alternating(void)
{
    /* 10101010 = 4 bits */
    TEST_ASSERT_EQUAL_UINT8(4U, hdc_popcount8(0xAAU));
    /* 01010101 = 4 bits */
    TEST_ASSERT_EQUAL_UINT8(4U, hdc_popcount8(0x55U));
}

void test_popcount8_single_bit(void)
{
    TEST_ASSERT_EQUAL_UINT8(1U, hdc_popcount8(0x01U));
    TEST_ASSERT_EQUAL_UINT8(1U, hdc_popcount8(0x80U));
    TEST_ASSERT_EQUAL_UINT8(1U, hdc_popcount8(0x10U));
}

void test_popcount_full_hypervector_zeros(void)
{
    hv_t hv;
    hdc_clear(hv);
    TEST_ASSERT_EQUAL_UINT8(0U, hdc_popcount(hv));
}

void test_popcount_full_hypervector_ones(void)
{
    hv_t hv;
    hdc_fill(hv, 0xFFU);
    TEST_ASSERT_EQUAL_UINT8(128U, hdc_popcount(hv));
}

/* ============================================================================
 * XOR Tests
 * ============================================================================ */

void test_xor_identical_vectors_gives_zero(void)
{
    hv_t a, b, result;
    hdc_fill(a, 0xAAU);
    hdc_fill(b, 0xAAU);

    hdc_xor(result, a, b);

    /* XOR of identical vectors = all zeros */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00U, result[i]);
    }
}

void test_xor_with_zero_gives_same(void)
{
    hv_t a, zero, result;
    hdc_fill(a, 0xABU);
    hdc_clear(zero);

    hdc_xor(result, a, zero);

    /* XOR with zero = same vector */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xABU, result[i]);
    }
}

void test_xor_with_ones_inverts(void)
{
    hv_t a, ones, result;
    hdc_fill(a, 0xAAU);     /* 10101010 */
    hdc_fill(ones, 0xFFU);  /* 11111111 */

    hdc_xor(result, a, ones);

    /* XOR with all-ones = inverted */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x55U, result[i]);  /* 01010101 */
    }
}

/* ============================================================================
 * OR Tests
 * ============================================================================ */

void test_or_with_zero_gives_same(void)
{
    hv_t a, zero, result;
    hdc_fill(a, 0xABU);
    hdc_clear(zero);

    hdc_or(result, a, zero);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xABU, result[i]);
    }
}

void test_or_combines_bits(void)
{
    hv_t a, b, result;
    hdc_fill(a, 0xF0U);  /* 11110000 */
    hdc_fill(b, 0x0FU);  /* 00001111 */

    hdc_or(result, a, b);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xFFU, result[i]);  /* 11111111 */
    }
}

/* ============================================================================
 * AND Tests (NEW - was untested)
 * ============================================================================ */

/**
 * @brief Test AND with zeros gives zeros
 * @details A AND 0 = 0 for all bits (identity property)
 */
void test_and_with_zero_gives_zero(void)
{
    hv_t a, zero, result;
    hdc_fill(a, 0xFFU);
    hdc_clear(zero);

    hdc_and(result, a, zero);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00U, result[i]);
    }
}

/**
 * @brief Test AND with ones gives same
 * @details A AND 1 = A (identity property)
 */
void test_and_with_ones_gives_same(void)
{
    hv_t a, ones, result;
    hdc_fill(a, 0xABU);
    hdc_fill(ones, 0xFFU);

    hdc_and(result, a, ones);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xABU, result[i]);
    }
}

/**
 * @brief Test AND masks bits correctly
 * @details Verify AND selects only common bits
 */
void test_and_masks_bits(void)
{
    hv_t a, b, result;
    hdc_fill(a, 0xF0U);  /* 11110000 */
    hdc_fill(b, 0x0FU);  /* 00001111 */

    hdc_and(result, a, b);

    /* No common bits = all zeros */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00U, result[i]);
    }
}

/**
 * @brief Test AND is commutative
 * @details A AND B = B AND A
 */
void test_and_is_commutative(void)
{
    hv_t a, b, result1, result2;
    hdc_fill(a, 0xAAU);
    hdc_fill(b, 0xCCU);

    hdc_and(result1, a, b);
    hdc_and(result2, b, a);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(result1[i], result2[i]);
    }
}

/**
 * @brief Test AND extracts common bits
 * @details 0xAA AND 0xCC = 0x88 (10001000)
 */
void test_and_extracts_common_bits(void)
{
    hv_t a, b, result;
    hdc_fill(a, 0xAAU);  /* 10101010 */
    hdc_fill(b, 0xCCU);  /* 11001100 */

    hdc_and(result, a, b);

    /* Common bits: 10001000 = 0x88 */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x88U, result[i]);
    }
}

/* ============================================================================
 * Bundle Tests
 * ============================================================================ */

void test_bundle_accumulates(void)
{
    hv_t memory, pattern1, pattern2;
    hdc_clear(memory);
    hdc_fill(pattern1, 0xF0U);  /* 11110000 */
    hdc_fill(pattern2, 0x0FU);  /* 00001111 */

    hdc_bundle(memory, pattern1);
    hdc_bundle(memory, pattern2);

    /* Memory should have both patterns */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xFFU, memory[i]);
    }
}

/* ============================================================================
 * Hamming Distance Tests
 * ============================================================================ */

void test_hamming_identical_is_zero(void)
{
    hv_t a, b;
    hdc_fill(a, 0xAAU);
    hdc_fill(b, 0xAAU);

    TEST_ASSERT_EQUAL_UINT8(0U, hdc_hamming(a, b));
}

void test_hamming_opposite_is_max(void)
{
    hv_t a, b;
    hdc_clear(a);
    hdc_fill(b, 0xFFU);

    TEST_ASSERT_EQUAL_UINT8(128U, hdc_hamming(a, b));
}

void test_hamming_half_different(void)
{
    hv_t a, b;
    hdc_fill(a, 0xF0U);  /* 11110000 - 4 bits per byte */
    hdc_fill(b, 0x0FU);  /* 00001111 - 4 bits per byte, all different */

    /* Each byte differs in 8 bits, 16 bytes = 128 */
    TEST_ASSERT_EQUAL_UINT8(128U, hdc_hamming(a, b));
}

void test_hamming_single_byte_different(void)
{
    hv_t a, b;
    hdc_clear(a);
    hdc_clear(b);
    b[0] = 0xFFU;  /* Only first byte different */

    TEST_ASSERT_EQUAL_UINT8(8U, hdc_hamming(a, b));
}

/* ============================================================================
 * Similarity Tests
 * ============================================================================ */

void test_similarity_identical_is_max(void)
{
    hv_t a, b;
    hdc_fill(a, 0xABU);
    hdc_fill(b, 0xABU);

    TEST_ASSERT_EQUAL_UINT8(128U, hdc_similarity(a, b));
}

void test_similarity_opposite_is_zero(void)
{
    hv_t a, b;
    hdc_clear(a);
    hdc_fill(b, 0xFFU);

    TEST_ASSERT_EQUAL_UINT8(0U, hdc_similarity(a, b));
}

/* ============================================================================
 * Permute Tests (NEW - was untested)
 * ============================================================================ */

/**
 * @brief Test permute by 0 returns same vector
 * @details No shift should leave vector unchanged
 */
void test_permute_zero_gives_same(void)
{
    hv_t hv, original;

    /* Create a pattern */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        hv[i] = i;
        original[i] = i;
    }

    hdc_permute(hv, 0U);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(original[i], hv[i]);
    }
}

/**
 * @brief Test permute by 8 shifts by one byte
 * @details Shifting by 8 bits should rotate bytes
 */
void test_permute_by_8_shifts_one_byte(void)
{
    hv_t hv;
    hdc_clear(hv);
    hv[0] = 0xFFU;  /* Only first byte set */

    hdc_permute(hv, 8U);

    /* First byte should now be at position 1 */
    TEST_ASSERT_EQUAL_HEX8(0x00U, hv[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFFU, hv[1]);
}

/**
 * @brief Test permute by 128 wraps around to same
 * @details Full rotation should return to original
 */
void test_permute_full_rotation_gives_same(void)
{
    hv_t hv, original;

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        hv[i] = i * 17U;  /* Arbitrary pattern */
        original[i] = i * 17U;
    }

    hdc_permute(hv, 128U);  /* Full rotation */

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(original[i], hv[i]);
    }
}

/**
 * @brief Test permute creates orthogonal vectors
 * @details Permuted vectors should be dissimilar (HDC property)
 *
 * RATIONALE: In HDC, permutation is used for position encoding.
 * For this to work, permuted vectors must be quasi-orthogonal
 * (approximately 50% bit overlap = similarity ~64 for 128-bit vectors).
 *
 * IMPORTANT: Using a periodic pattern like 0xAA fails because rotation
 * of a periodic pattern creates anti-correlation (similarity = 0).
 * We need a non-periodic pattern for realistic orthogonality testing.
 */
void test_permute_creates_orthogonal_vectors(void)
{
    hv_t original, permuted;

    /*
     * Non-periodic pattern: each byte is different
     * This simulates realistic random hypervectors used in HDC
     * Pattern has ~50% bit density (good for similarity testing)
     */
    original[0]  = 0x12U; original[1]  = 0x34U; original[2]  = 0x56U; original[3]  = 0x78U;
    original[4]  = 0x9AU; original[5]  = 0xBCU; original[6]  = 0xDEU; original[7]  = 0xF0U;
    original[8]  = 0x21U; original[9]  = 0x43U; original[10] = 0x65U; original[11] = 0x87U;
    original[12] = 0xA9U; original[13] = 0xCBU; original[14] = 0xEDU; original[15] = 0x0FU;

    hdc_copy(permuted, original);
    hdc_permute(permuted, 37U);  /* Odd shift for good scrambling */

    /*
     * For non-periodic patterns, permutation should yield quasi-orthogonality
     * Expected similarity: approximately 64 ± 32 (25%-75% overlap)
     * We use wider bounds to account for pattern-dependent variance
     */
    uint8_t hamming = hdc_hamming(original, permuted);
    uint8_t similarity = hdc_similarity(original, permuted);

    /*
     * Verify vectors are different (non-zero Hamming distance)
     * and neither perfectly correlated nor anti-correlated
     */
    TEST_ASSERT_NOT_EQUAL_UINT8(0U, hamming);      /* Not identical */
    TEST_ASSERT_NOT_EQUAL_UINT8(128U, hamming);    /* Not anti-correlated */

    /* Similarity should be in orthogonal range (not too similar, not too different) */
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(16U, similarity);  /* At least 12.5% overlap */
    TEST_ASSERT_LESS_OR_EQUAL_UINT8(112U, similarity);    /* At most 87.5% overlap */
}

/**
 * @brief Test permute with small bit shift
 * @details Verify sub-byte shifting works correctly
 */
void test_permute_small_shift(void)
{
    hv_t hv;
    hdc_clear(hv);
    hv[0] = 0x01U;  /* Single bit at position 0 */

    hdc_permute(hv, 1U);

    /* Bit should now be at position 1 */
    TEST_ASSERT_EQUAL_HEX8(0x02U, hv[0]);
}

/**
 * @brief Test permute preserves popcount
 * @details Rotation shouldn't change number of set bits
 */
void test_permute_preserves_popcount(void)
{
    hv_t hv;
    hdc_fill(hv, 0xAAU);  /* 64 bits set */

    uint8_t before = hdc_popcount(hv);
    hdc_permute(hv, 47U);
    uint8_t after = hdc_popcount(hv);

    TEST_ASSERT_EQUAL_UINT8(before, after);
}

/* ============================================================================
 * Thermometer Encoding Tests
 * ============================================================================ */

void test_thermo_zero_gives_empty(void)
{
    hv_t hv;
    hdc_encode_thermometer(hv, 0U, 1024U);

    TEST_ASSERT_EQUAL_UINT8(0U, hdc_popcount(hv));
}

void test_thermo_max_gives_full(void)
{
    hv_t hv;
    hdc_encode_thermometer(hv, 1024U, 1024U);

    TEST_ASSERT_EQUAL_UINT8(128U, hdc_popcount(hv));
}

void test_thermo_half_gives_half(void)
{
    hv_t hv;
    hdc_encode_thermometer(hv, 512U, 1024U);

    /* Should be approximately 64 bits (half of 128) */
    uint8_t count = hdc_popcount(hv);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(60U, count);
    TEST_ASSERT_LESS_OR_EQUAL_UINT8(68U, count);
}

void test_thermo_preserves_order(void)
{
    hv_t hv_low, hv_mid, hv_high;

    hdc_encode_thermometer(hv_low, 100U, 1024U);
    hdc_encode_thermometer(hv_mid, 500U, 1024U);
    hdc_encode_thermometer(hv_high, 900U, 1024U);

    uint8_t count_low = hdc_popcount(hv_low);
    uint8_t count_mid = hdc_popcount(hv_mid);
    uint8_t count_high = hdc_popcount(hv_high);

    /* Higher values should have more bits set */
    TEST_ASSERT_LESS_THAN_UINT8(count_mid, count_low);
    TEST_ASSERT_LESS_THAN_UINT8(count_high, count_mid);
}

void test_thermo_similar_values_are_close(void)
{
    hv_t hv_a, hv_b;

    /* Values close together */
    hdc_encode_thermometer(hv_a, 500U, 1024U);
    hdc_encode_thermometer(hv_b, 510U, 1024U);

    uint8_t distance = hdc_hamming(hv_a, hv_b);

    /* Should be very similar (small distance) */
    TEST_ASSERT_LESS_THAN_UINT8(10U, distance);
}

void test_thermo_distant_values_are_far(void)
{
    hv_t hv_a, hv_b;

    /* Values far apart */
    hdc_encode_thermometer(hv_a, 100U, 1024U);
    hdc_encode_thermometer(hv_b, 900U, 1024U);

    uint8_t distance = hdc_hamming(hv_a, hv_b);

    /* Should be very different (large distance) */
    TEST_ASSERT_GREATER_THAN_UINT8(80U, distance);
}

/* ============================================================================
 * ADC Encoding Tests (NEW - was untested)
 * ============================================================================ */

/**
 * @brief Test ADC encoding of minimum value (0)
 * @details ADC 0 should give empty hypervector
 */
void test_adc_zero_gives_empty(void)
{
    hv_t hv;
    hdc_encode_adc(hv, 0U);

    TEST_ASSERT_EQUAL_UINT8(0U, hdc_popcount(hv));
}

/**
 * @brief Test ADC encoding of maximum value (1023)
 * @details ADC 1023 should give full hypervector
 */
void test_adc_max_gives_full(void)
{
    hv_t hv;
    hdc_encode_adc(hv, 1023U);

    TEST_ASSERT_EQUAL_UINT8(128U, hdc_popcount(hv));
}

/**
 * @brief Test ADC encoding of mid-range value
 * @details ADC ~512 should give approximately half bits set
 */
void test_adc_mid_gives_half(void)
{
    hv_t hv;
    hdc_encode_adc(hv, 512U);

    uint8_t count = hdc_popcount(hv);
    /* Should be approximately 64 bits */
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(58U, count);
    TEST_ASSERT_LESS_OR_EQUAL_UINT8(70U, count);
}

/**
 * @brief Test ADC encoding preserves similarity
 * @details Close ADC values should produce similar hypervectors
 */
void test_adc_close_values_are_similar(void)
{
    hv_t hv_a, hv_b;

    hdc_encode_adc(hv_a, 500U);
    hdc_encode_adc(hv_b, 510U);

    uint8_t distance = hdc_hamming(hv_a, hv_b);
    TEST_ASSERT_LESS_THAN_UINT8(10U, distance);
}

/* ============================================================================
 * Bipolar Encoding Tests (NEW - was untested)
 * ============================================================================ */

/**
 * @brief Test bipolar encoding of minimum value
 * @details Min value should give empty hypervector
 */
void test_bipolar_min_gives_empty(void)
{
    hv_t hv;
    hdc_encode_bipolar(hv, -100, -100, 100);

    TEST_ASSERT_EQUAL_UINT8(0U, hdc_popcount(hv));
}

/**
 * @brief Test bipolar encoding of maximum value
 * @details Max value should give full hypervector
 */
void test_bipolar_max_gives_full(void)
{
    hv_t hv;
    hdc_encode_bipolar(hv, 100, -100, 100);

    TEST_ASSERT_EQUAL_UINT8(128U, hdc_popcount(hv));
}

/**
 * @brief Test bipolar encoding of zero (midpoint)
 * @details Zero in symmetric range should give approximately half bits
 */
void test_bipolar_zero_gives_half(void)
{
    hv_t hv;
    hdc_encode_bipolar(hv, 0, -100, 100);

    uint8_t count = hdc_popcount(hv);
    /* Should be approximately 64 bits */
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(58U, count);
    TEST_ASSERT_LESS_OR_EQUAL_UINT8(70U, count);
}

/**
 * @brief Test bipolar encoding with asymmetric range
 * @details Verify correct handling of non-symmetric ranges
 */
void test_bipolar_asymmetric_range(void)
{
    hv_t hv;

    /* Range from -50 to 150, test value at midpoint (50) */
    hdc_encode_bipolar(hv, 50, -50, 150);

    uint8_t count = hdc_popcount(hv);
    /* 50 is exactly halfway between -50 and 150 */
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(58U, count);
    TEST_ASSERT_LESS_OR_EQUAL_UINT8(70U, count);
}

/**
 * @brief Test bipolar encoding preserves similarity
 * @details Close values should produce similar hypervectors
 */
void test_bipolar_close_values_are_similar(void)
{
    hv_t hv_a, hv_b;

    hdc_encode_bipolar(hv_a, 10, -100, 100);
    hdc_encode_bipolar(hv_b, 15, -100, 100);

    uint8_t distance = hdc_hamming(hv_a, hv_b);
    TEST_ASSERT_LESS_THAN_UINT8(10U, distance);
}

/* ============================================================================
 * Multi-Channel Encoding Tests (NEW - was untested)
 * ============================================================================ */

/**
 * @brief Test multi-channel encoding with single channel
 * @details Single channel should produce valid encoded result
 */
void test_multi_channel_single(void)
{
    hv_t result;
    hv_t basis[1];
    uint16_t values[1] = {512U};

    /* Create a basis vector */
    hdc_fill(basis[0], 0xAAU);

    hdc_encode_multi_channel(result, values, 1U, basis);

    /* Result should have bits set (not empty) */
    uint8_t count = hdc_popcount(result);
    TEST_ASSERT_GREATER_THAN_UINT8(0U, count);
}

/**
 * @brief Test multi-channel encoding produces combined result
 * @details Two channels should produce result with more information
 */
void test_multi_channel_two_channels(void)
{
    hv_t result;
    hv_t basis[2];
    uint16_t values[2] = {256U, 768U};

    /* Create orthogonal-ish basis vectors */
    hdc_fill(basis[0], 0xAAU);
    hdc_fill(basis[1], 0x55U);

    hdc_encode_multi_channel(result, values, 2U, basis);

    /* Result should have bits set */
    uint8_t count = hdc_popcount(result);
    TEST_ASSERT_GREATER_THAN_UINT8(0U, count);
}

/**
 * @brief Test multi-channel encoding with maximum values
 * @details All max values should produce mostly full hypervector
 */
void test_multi_channel_max_values(void)
{
    hv_t result;
    hv_t basis[2];
    uint16_t values[2] = {1023U, 1023U};

    hdc_fill(basis[0], 0xF0U);
    hdc_fill(basis[1], 0x0FU);

    hdc_encode_multi_channel(result, values, 2U, basis);

    /* With bundling (OR), max values XORed with basis should be near full */
    uint8_t count = hdc_popcount(result);
    TEST_ASSERT_GREATER_THAN_UINT8(64U, count);
}

/**
 * @brief Test multi-channel encoding with zero values
 * @details All zero values should produce hypervector based on basis XOR empty
 */
void test_multi_channel_zero_values(void)
{
    hv_t result;
    hv_t basis[2];
    uint16_t values[2] = {0U, 0U};

    hdc_fill(basis[0], 0xAAU);
    hdc_fill(basis[1], 0x55U);

    hdc_encode_multi_channel(result, values, 2U, basis);

    /* Zero value thermometer gives empty, XOR with basis gives basis */
    /* Bundle of two complementary bases = all ones */
    uint8_t count = hdc_popcount(result);
    TEST_ASSERT_EQUAL_UINT8(128U, count);
}

/* ============================================================================
 * Copy and Clear Tests
 * ============================================================================ */

void test_clear_sets_all_zeros(void)
{
    hv_t hv;
    hdc_fill(hv, 0xFFU);  /* Start with all ones */
    hdc_clear(hv);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00U, hv[i]);
    }
}

void test_copy_duplicates_exactly(void)
{
    hv_t src, dest;

    /* Set up source with pattern */
    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        src[i] = i;
    }

    hdc_copy(dest, src);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(src[i], dest[i]);
    }
}

/**
 * @brief Test fill sets all bytes to value
 */
void test_fill_sets_all_bytes(void)
{
    hv_t hv;
    hdc_clear(hv);
    hdc_fill(hv, 0xABU);

    for (uint8_t i = 0U; i < HV_BYTES; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xABU, hv[i]);
    }
}

/* ============================================================================
 * Main Test Runner
 * ============================================================================ */

int main(void)
{
    UNITY_BEGIN();

    /* Popcount tests */
    RUN_TEST(test_popcount8_zero);
    RUN_TEST(test_popcount8_all_ones);
    RUN_TEST(test_popcount8_alternating);
    RUN_TEST(test_popcount8_single_bit);
    RUN_TEST(test_popcount_full_hypervector_zeros);
    RUN_TEST(test_popcount_full_hypervector_ones);

    /* XOR tests */
    RUN_TEST(test_xor_identical_vectors_gives_zero);
    RUN_TEST(test_xor_with_zero_gives_same);
    RUN_TEST(test_xor_with_ones_inverts);

    /* OR tests */
    RUN_TEST(test_or_with_zero_gives_same);
    RUN_TEST(test_or_combines_bits);

    /* AND tests (NEW) */
    RUN_TEST(test_and_with_zero_gives_zero);
    RUN_TEST(test_and_with_ones_gives_same);
    RUN_TEST(test_and_masks_bits);
    RUN_TEST(test_and_is_commutative);
    RUN_TEST(test_and_extracts_common_bits);

    /* Bundle tests */
    RUN_TEST(test_bundle_accumulates);

    /* Hamming distance tests */
    RUN_TEST(test_hamming_identical_is_zero);
    RUN_TEST(test_hamming_opposite_is_max);
    RUN_TEST(test_hamming_half_different);
    RUN_TEST(test_hamming_single_byte_different);

    /* Similarity tests */
    RUN_TEST(test_similarity_identical_is_max);
    RUN_TEST(test_similarity_opposite_is_zero);

    /* Permute tests (NEW) */
    RUN_TEST(test_permute_zero_gives_same);
    RUN_TEST(test_permute_by_8_shifts_one_byte);
    RUN_TEST(test_permute_full_rotation_gives_same);
    RUN_TEST(test_permute_creates_orthogonal_vectors);
    RUN_TEST(test_permute_small_shift);
    RUN_TEST(test_permute_preserves_popcount);

    /* Thermometer encoding tests */
    RUN_TEST(test_thermo_zero_gives_empty);
    RUN_TEST(test_thermo_max_gives_full);
    RUN_TEST(test_thermo_half_gives_half);
    RUN_TEST(test_thermo_preserves_order);
    RUN_TEST(test_thermo_similar_values_are_close);
    RUN_TEST(test_thermo_distant_values_are_far);

    /* ADC encoding tests (NEW) */
    RUN_TEST(test_adc_zero_gives_empty);
    RUN_TEST(test_adc_max_gives_full);
    RUN_TEST(test_adc_mid_gives_half);
    RUN_TEST(test_adc_close_values_are_similar);

    /* Bipolar encoding tests (NEW) */
    RUN_TEST(test_bipolar_min_gives_empty);
    RUN_TEST(test_bipolar_max_gives_full);
    RUN_TEST(test_bipolar_zero_gives_half);
    RUN_TEST(test_bipolar_asymmetric_range);
    RUN_TEST(test_bipolar_close_values_are_similar);

    /* Multi-channel encoding tests (NEW) */
    RUN_TEST(test_multi_channel_single);
    RUN_TEST(test_multi_channel_two_channels);
    RUN_TEST(test_multi_channel_max_values);
    RUN_TEST(test_multi_channel_zero_values);

    /* Copy, clear, and fill tests */
    RUN_TEST(test_clear_sets_all_zeros);
    RUN_TEST(test_copy_duplicates_exactly);
    RUN_TEST(test_fill_sets_all_bytes);

    return UNITY_END();
}
