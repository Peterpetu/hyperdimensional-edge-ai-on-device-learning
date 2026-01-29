"""
PlatformIO Pre-Script: Coverage Link Configuration

This script ensures the gcov runtime library is properly linked
when building with code coverage instrumentation.

RATIONALE:
    The --coverage flag in GCC should automatically add -lgcov during
    linking, but PlatformIO's native platform sometimes needs explicit
    configuration. This script adds the necessary linker flags.

USAGE:
    Referenced in platformio.ini as:
    extra_scripts = pre:scripts/coverage_link.py

See: https://docs.platformio.org/en/latest/scripting/index.html
"""

Import("env")

# Add coverage linker flags
# --coverage should handle this, but we ensure it's explicit
env.Append(
    LINKFLAGS=[
        "--coverage",      # Enable coverage at link time
        "-fprofile-arcs",  # Generate profile arcs for gcov
    ]
)

# Print confirmation during build (verbose mode)
# Uncomment for debugging:
# print("Coverage link flags added: --coverage -fprofile-arcs")
