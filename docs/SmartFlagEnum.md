# SmartFlagEnum

## Overview

SmartFlagEnum extends the base SmartEnum concept to provide bitflag functionality:

- **Flag combination**: Combine multiple flags using the bitwise OR operator
- **Flag decomposition**: Convert combined flag values back to individual flags
- **Power-of-two validation**: Ensures flag values follow proper bit flag conventions
- **String conversion**: Easily convert between flag names and values

## Before and After

### Before (Traditional Bit Flags)

```cpp
// Traditional enum flags approach
enum FilePermission {
    NONE = 0,
    READ = 1,
    WRITE = 2,
    EXECUTE = 4,
    DELETE = 8
};

// Problems:
// 1. No type safety for combinations
// 2. Cannot easily convert flags back to names
// 3. No validation for proper power-of-two values
// 4. Difficult to enumerate all flags in a combined value

// Manual flag checking
bool hasReadWrite(int permissions) {
    return (permissions & READ) && (permissions & WRITE);
}

// Manual flag to string conversion
std::string permissionsToString(int permissions) {
    std::string result;
    if (permissions & READ) result += "Read ";
    if (permissions & WRITE) result += "Write ";
    if (permissions & EXECUTE) result += "Execute ";
    if (permissions & DELETE) result += "Delete ";
    return result.empty() ? "None" : result;
}
```

### After (Using SmartFlagEnum)

```cpp
#include <SmartEnumCpp/SmartFlagEnum.hpp>

class FilePermission : public SmartFlagEnum<FilePermission> {
public:
    static const FilePermission None;
    static const FilePermission Read;
    static const FilePermission Write;
    static const FilePermission Execute;
    static const FilePermission Delete;
    
private:
    FilePermission(const std::string& name, int value) : SmartFlagEnum(name, value) {}
};

const FilePermission FilePermission::None("None", 0);
const FilePermission FilePermission::Read("Read", 1);      // 2^0
const FilePermission FilePermission::Write("Write", 2);    // 2^1
const FilePermission FilePermission::Execute("Execute", 4); // 2^2
const FilePermission FilePermission::Delete("Delete", 8);  // 2^3

// Benefits:
// 1. Type-safe flag combination: FilePermission::Read | FilePermission::Write
// 2. Automatic conversion to individual flags: FilePermission::FromValue(3)
// 3. String conversion: FilePermission::FromValueToString(3) => "Read, Write"
// 4. Validation of proper power-of-two values
```

## Basic Usage

### Creating a SmartFlagEnum

```cpp
#include <SmartEnumCpp/SmartFlagEnum.hpp>

// 1. Define your flag enum class
class FilePermission : public SmartFlagEnum<FilePermission> {
public:
    static const FilePermission None;
    static const FilePermission Read;
    static const FilePermission Write;
    static const FilePermission Execute;
    static const FilePermission Delete;
    
private:
    FilePermission(const std::string& name, int value) : SmartFlagEnum(name, value) {}
};

// 2. Initialize with power-of-2 values
const FilePermission FilePermission::None("None", 0);
const FilePermission FilePermission::Read("Read", 1);      // 2^0
const FilePermission FilePermission::Write("Write", 2);    // 2^1
const FilePermission FilePermission::Execute("Execute", 4); // 2^2
const FilePermission FilePermission::Delete("Delete", 8);  // 2^3
```

### Working with SmartFlagEnum values

```cpp
// Combine flags using bitwise OR
int readWrite = FilePermission::Read | FilePermission::Write;  // Value: 3

// Get the string representation of a combined value
std::string flagsStr = FilePermission::FromValueToString(readWrite);  // "Read, Write"

// Get individual flags from a combined value
std::vector<const FilePermission*> flags = FilePermission::FromValue(readWrite);
// flags contains pointers to FilePermission::Read and FilePermission::Write

// Checking if a value contains specific flags
int permissions = FilePermission::Read | FilePermission::Execute;  // Value: 5
bool hasRead = (permissions & FilePermission::Read) != 0;  // true
bool hasWrite = (permissions & FilePermission::Write) != 0;  // false

// Parse flags from a string
std::vector<const FilePermission*> parsedFlags = 
    FilePermission::FromName("Read, Execute");
// Equivalent to FilePermission::Read | FilePermission::Execute
```

## Advanced Features

### Using Different Value Types

```cpp
class NetworkFlags : public SmartFlagEnum<NetworkFlags, uint16_t> {
    // Using uint16_t instead of int
};
```

### Special Flag Values

#### Allowing Negative Flag Values

By default, negative values are not allowed in flag enums. To allow them:

```cpp
class SpecialFlags : public SmartFlagEnum<SpecialFlags>, public AllowNegativeFlagEnumInput {
public:
    static const SpecialFlags None;
    static const SpecialFlags Flag1;
    static const SpecialFlags Flag2;
    static const SpecialFlags All;  // Using -1 to represent all flags
    
private:
    SpecialFlags(const std::string& name, int value) : SmartFlagEnum(name, value) {}
};

const SpecialFlags SpecialFlags::None("None", 0);
const SpecialFlags SpecialFlags::Flag1("Flag1", 1);
const SpecialFlags SpecialFlags::Flag2("Flag2", 2);
const SpecialFlags SpecialFlags::All("All", -1);  // Special value -1
```

#### Allowing Non-Power-of-Two Values

By default, flag values must be powers of 2. To allow arbitrary values:

```cpp
class UnsafeFlags : public SmartFlagEnum<UnsafeFlags>, public AllowUnsafeFlagEnumValues {
    // Can use non-power-of-two values
};
```

### Exception Handling

```cpp
try {
    // Will throw if value doesn't match defined flags
    std::vector<const FilePermission*> invalid = FilePermission::FromValue(16);
} catch (const InvalidFlagEnumValueParseException& e) {
    std::cout << e.what() << std::endl;
}
```