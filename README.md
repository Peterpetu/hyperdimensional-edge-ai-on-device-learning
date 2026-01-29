# Nano-Edge AI: On-Device Machine Learning with Hyperdimensional Computing

[![Platform](https://img.shields.io/badge/Platform-ATmega328P-blue.svg)](https://www.microchip.com/en-us/product/ATmega328P)
[![Language](https://img.shields.io/badge/Language-C99-green.svg)](https://en.wikipedia.org/wiki/C99)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build](https://github.com/Peterpetu/hyperdimensional-edge-ai-on-device-learning/actions/workflows/build.yml/badge.svg)](https://github.com/Peterpetu/hyperdimensional-edge-ai-on-device-learning/actions)

## Overview

An embedded machine learning system implementing **Hyperdimensional Computing (HDC)** for autonomous on-device learning on severely resource-constrained hardware.

**Target Platform:**
- MCU: ATmega328P (Arduino Uno R3)
- Flash: 32 KB
- SRAM: 2 KB
- Clock: 16 MHz

**Key Features:**
- Autonomous learning without human intervention
- Single-pass training (no backpropagation)
- 128-bit binary hypervectors
- Thermometer encoding for analog sensors
- Real-time inference using Hamming distance
- Engineer/JPL compliant timeout guards on all blocking operations

---

## Project Structure

```
arduino-ml-project/
├── src/                        # Source code
│   ├── app/                    # Application layer
│   │   └── main.c              # Program entry point
│   ├── hal/                    # Hardware Abstraction Layer
│   │   ├── hal.h               # Master HAL include
│   │   ├── hal_gpio.h          # GPIO interface
│   │   ├── hal_uart.h          # UART interface (with timeout)
│   │   └── hal_adc.h           # ADC interface (with timeout)
│   └── hdc/                    # HDC algorithm modules
│       ├── hdc.h               # Master HDC include
│       ├── hdc_core.h          # Core operations (XOR, bundle, hamming)
│       └── hdc_encode.h        # Thermometer encoding
│
├── test/                       # Test suites
│   ├── unit/                   # Unit tests (Unity framework)
│   │   └── test_hdc_core.c     # HDC operation tests
│   └── mocks/                  # Mock implementations
│       └── mock_hal.h          # Mock HAL for PC testing
│
├── config/                     # Tool configuration
│   ├── cppcheck.cfg            # Static analysis config
│   ├── .clang-format           # Code formatting rules
│   └── Doxyfile                # Documentation generator
│
├── .github/workflows/          # CI/CD
│   └── build.yml               # GitHub Actions workflow
│
├── project_notes_and_data/     # Learning materials (gitignored)
│   ├── CODING_STANDARDS.md     # Coding rules explained
│   ├── EMBEDDED_PROJECT_STANDARDS.md
│   └── learning_*.md           # Educational documents
│
├── platformio.ini              # Build configuration
├── CLAUDE.md                   # AI assistant instructions
└── README.md                   # This file
```

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      APPLICATION LAYER                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐ │
│  │  Learning   │  │  Inference  │  │    Control Logic        │ │
│  │    Loop     │  │    Engine   │  │    (Actions/Rewards)    │ │
│  └──────┬──────┘  └──────┬──────┘  └────────────┬────────────┘ │
└─────────┼────────────────┼──────────────────────┼───────────────┘
          │                │                      │
┌─────────┼────────────────┼──────────────────────┼───────────────┐
│         ▼                ▼                      ▼               │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │               HDC ALGORITHM LAYER                        │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐ │   │
│  │  │ Encoder  │  │ Bundler  │  │ Hamming  │  │ Memory   │ │   │
│  │  │ (Thermo) │  │   (OR)   │  │ Distance │  │ (Policy) │ │   │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘ │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
          │                │                      │
┌─────────┼────────────────┼──────────────────────┼───────────────┐
│         ▼                ▼                      ▼               │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │           HARDWARE ABSTRACTION LAYER (HAL)               │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐ │   │
│  │  │   GPIO   │  │   UART   │  │   ADC    │  │   PWM    │ │   │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘ │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
          │                │                      │
          ▼                ▼                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    HARDWARE (ATmega328P)                        │
│     GPIO Pins        USART         ADC          Timer/PWM       │
└─────────────────────────────────────────────────────────────────┘
```

---

## Building

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)
- USB cable for Arduino
- (Optional) Serial terminal for debugging

### Commands

```bash
# Build for Arduino Uno
pio run

# Upload to Arduino
pio run --target upload

# Serial Monitor (9600 baud)
pio device monitor

# Clean build
pio run --target clean
```

---

## Testing

```bash
# Run unit tests on PC (no Arduino needed)
pio test -e native

# Run static analysis
pio check -e check
```

### Test Coverage

- HDC core operations (XOR, OR, bundle, popcount)
- Hamming distance calculation
- Thermometer encoding
- Similarity metrics

---

## Memory Budget

| Component | RAM Usage | Notes |
|-----------|-----------|-------|
| Stack | ~256 bytes | Function calls, locals |
| State HV | 16 bytes | Current encoded state |
| GOOD_POLICY | 16 bytes | Bundled successful patterns |
| BAD_POLICY | 16 bytes | Bundled failure patterns |
| Buffers | ~256 bytes | UART, temporary storage |
| **Total** | **~560 bytes** | Of 2048 available |

---

## Coding Standards

This project follows embedded industry standards:
- **MISRA-C:2012** - Motor Industry Software Reliability Association
- **Engineer/JPL** - Jet Propulsion Laboratory Coding Standard
- **BARR-C:2018** - Barr Group Embedded C Coding Standard

Key practices:
- All blocking loops have timeout guards
- Fixed-width types (`uint8_t`, not `int`)
- No dynamic memory allocation
- HAL abstracts all hardware access

---

## Documentation

Generate API documentation with Doxygen:

```bash
doxygen config/Doxyfile
# Output: docs/doxygen/html/index.html
```

---

## References

1. FSL-HDnn: Few-Shot Learning with Hyperdimensional Neural Networks
2. Engineer/JPL Institutional Coding Standard for C
3. MISRA-C:2012 Guidelines
4. ATmega328P Datasheet