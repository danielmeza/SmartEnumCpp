# SmartEnumProject

This project is a C++ port of the Ardalis.SmartEnum C# library providing type-safe "smart" enum types and flag enums.

## Features

- **SmartEnum**: Supports name/value registration, lookup (by name and value), and equality comparison.
- **Polymorphic Enums**: Allows custom behavior per enum instance.
- **SmartFlagEnum**: Implements flags with combination logic and validates power-of-two flag definitions.
- **Fluent Switch Interface**: Provides a chainable switch-like API for enums.
- **Unit Tests**: GoogleTest-based tests verify the behavior.

## Build Instructions

This project uses CMake. From the root directory:

```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest
