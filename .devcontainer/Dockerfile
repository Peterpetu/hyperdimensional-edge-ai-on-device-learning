# =============================================================================
# Embedded Development Container
# =============================================================================
# Professional embedded C development environment for ATmega328P
# Includes: AVR toolchain, static analysis, code coverage, formal verification
# =============================================================================

FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-22.04

LABEL maintainer="Arduino ML Project"
LABEL description="high-grade embedded development environment for ATmega328P"

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# =============================================================================
# Install all development tools
# =============================================================================
RUN apt-get update && apt-get install -y --no-install-recommends \
    # -------------------------------------------------------------------------
    # AVR Toolchain (for Arduino Uno / ATmega328P)
    # -------------------------------------------------------------------------
    avr-libc \
    gcc-avr \
    binutils-avr \
    avrdude \
    # -------------------------------------------------------------------------
    # Static Analysis Tools
    # -------------------------------------------------------------------------
    cppcheck \
    clang \
    clang-format \
    clang-tidy \
    # -------------------------------------------------------------------------
    # Code Coverage Tools
    # -------------------------------------------------------------------------
    lcov \
    # -------------------------------------------------------------------------
    # Formal Verification
    # -------------------------------------------------------------------------
    cbmc \
    # -------------------------------------------------------------------------
    # Documentation Generation
    # -------------------------------------------------------------------------
    doxygen \
    graphviz \
    # -------------------------------------------------------------------------
    # Python (for PlatformIO)
    # -------------------------------------------------------------------------
    python3 \
    python3-pip \
    python3-venv \
    # -------------------------------------------------------------------------
    # Utilities
    # -------------------------------------------------------------------------
    git \
    curl \
    wget \
    nano \
    vim \
    && rm -rf /var/lib/apt/lists/*

# =============================================================================
# Install Python packages
# =============================================================================
RUN pip3 install --no-cache-dir --upgrade pip && \
    pip3 install --no-cache-dir \
    platformio \
    gcovr

# =============================================================================
# Pre-install PlatformIO platforms (reduces first-build time)
# =============================================================================
RUN pio platform install atmelavr && \
    pio platform install native

# =============================================================================
# Set working directory
# =============================================================================
WORKDIR /workspace

# =============================================================================
# Configure PlatformIO home directory
# =============================================================================
ENV PLATFORMIO_CORE_DIR=/home/vscode/.platformio

# =============================================================================
# Default shell
# =============================================================================
CMD ["/bin/bash"]
