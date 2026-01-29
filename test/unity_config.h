/**
 * @file    unity_config.h
 * @brief   Unity Test Framework Configuration for HDC Project
 * @version 1.0.0
 * @note    Configures Unity for native PC testing of ATmega328P-targeted code
 *
 * @details This project uses a dual-platform testing strategy:
 *
 *          TARGET PLATFORM: ATmega328P (Arduino Uno)
 *            - 8-bit microcontroller
 *            - 2KB SRAM, 32KB Flash
 *            - No floating point unit
 *            - Integer widths: int=16bit, long=32bit, pointer=16bit
 *
 *          TEST PLATFORM: Native PC (Linux x86_64 in container)
 *            - 64-bit processor
 *            - Integer widths: int=32bit, long=64bit, pointer=64bit
 *
 *          The HDC module (src/hdc/) is written to be platform-portable:
 *            - Uses only fixed-width types (uint8_t, uint16_t, uint32_t)
 *            - No pointer arithmetic dependent on pointer size
 *            - No floating point operations
 *            - All operations are deterministic across platforms
 *
 *          This allows us to run fast unit tests on PC while ensuring
 *          the same code compiles correctly for the AVR target.
 *
 * @see     CLAUDE.md for project coding standards
 * @see     platformio.ini for build configurations
 */

#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

/* =============================================================================
 * Platform Detection
 * ========================================================================== */

/**
 * @brief Verify we're building for the native test platform
 * @note  This config should only be used in native/coverage builds
 */
#ifndef UNIT_TEST
    #error "unity_config.h included in non-test build. Check platformio.ini."
#endif

/* =============================================================================
 * Required Includes
 * ========================================================================== */

#include <stdint.h>  /* Fixed-width integer types (project standard) */
#include <stdio.h>   /* printf/putchar for test output */

/* =============================================================================
 * Output Configuration
 * ========================================================================== */

/**
 * @brief Direct test output to stdout
 * @note  Required for Unity to display test results in terminal
 *
 * On the actual ATmega328P target, we would redirect to UART.
 * For native PC testing, stdout is appropriate.
 */
#define UNITY_OUTPUT_CHAR(c)    putchar(c)
#define UNITY_OUTPUT_FLUSH()    fflush(stdout)

/* =============================================================================
 * Integer Width Configuration
 * ========================================================================== */

/**
 * @brief Configure Unity's integer handling for our test environment
 *
 * RATIONALE: Our HDC code uses fixed-width types exclusively:
 *   - hv_t        = uint8_t[16]   (128-bit hypervector)
 *   - ADC values  = uint16_t      (0-1023)
 *   - Distances   = uint8_t       (0-128)
 *
 * Unity's TEST_ASSERT_EQUAL_UINT8/16/32 macros work correctly regardless
 * of these settings. These widths configure Unity's internal handling
 * of generic integer comparisons (TEST_ASSERT_EQUAL_INT, etc.).
 *
 * We match the native PC platform (x86_64 Linux):
 */
#define UNITY_INT_WIDTH         32  /* sizeof(int) * 8 on x86_64 */
#define UNITY_LONG_WIDTH        64  /* sizeof(long) * 8 on x86_64 */
#define UNITY_POINTER_WIDTH     64  /* sizeof(void*) * 8 on x86_64 */

/**
 * @brief Enable 64-bit integer support in Unity
 * @note  Not used by our HDC code (max is uint32_t), but allows
 *        Unity's full assertion library to function correctly
 */
#define UNITY_SUPPORT_64

/* =============================================================================
 * Feature Configuration
 * ========================================================================== */

/**
 * @brief Enable floating point assertions
 * @note  Our HDC code is integer-only (MISRA-C compliant), but Unity
 *        needs this for its internal math. Disabled to match target.
 */
/* #define UNITY_EXCLUDE_FLOAT */
/* #define UNITY_EXCLUDE_DOUBLE */

/**
 * @brief Memory constraints
 * @note  On ATmega328P we have 2KB SRAM. Native PC has no such limits.
 *        Unity's default buffer sizes are acceptable for PC testing.
 */
/* UNITY_MAX_OUTPUT_STRING_SIZE defaults to 256, acceptable for our tests */

/* =============================================================================
 * Verification
 * ========================================================================== */

/**
 * @brief Compile-time verification of expected types
 * @note  Ensures our fixed-width type assumptions are correct
 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    /* C11 static assertions available */
    #include <stddef.h>
    _Static_assert(sizeof(uint8_t)  == 1, "uint8_t must be 1 byte");
    _Static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes");
    _Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
#endif

#endif /* UNITY_CONFIG_H */
