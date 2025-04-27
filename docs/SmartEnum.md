# SmartEnum

## Overview

`SmartEnum` is a type-safe alternative to traditional C++ enums that provides:

- **String representation**: Each enum value has a name accessible at runtime
- **Value lookup**: Find enum instances by name or value
- **Type safety**: Compiler errors for invalid enum values
- **Reflection capabilities**: Enumerate all possible values at runtime

## Before and After

### Before (Traditional C++ Enum)

```cpp
// Traditional enum approach
enum Color {
    RED = 1,
    GREEN = 2,
    BLUE = 3
};

// Problems:
// 1. No built-in string representation
// 2. Cannot iterate all values
// 3. No built-in validation for values
// 4. Runtime lookup requires manual switch statements

std::string colorToString(Color color) {
    switch(color) {
        case RED: return "Red";
        case GREEN: return "Green";
        case BLUE: return "Blue";
        default: return "Unknown";
    }
}

Color stringToColor(const std::string& name) {
    if (name == "Red") return RED;
    if (name == "Green") return GREEN;
    if (name == "Blue") return BLUE;
    throw std::runtime_error("Invalid color name");
}
```

### After (Using SmartEnum)

```cpp
#include <SmartEnumCpp/SmartEnum.hpp>

class Color : public SmartEnum<Color> {
public:
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    
private:
    Color(const std::string& name, int value) : SmartEnum(name, value) {}
};

const Color Color::Red("Red", 1);
const Color Color::Green("Green", 2);
const Color Color::Blue("Blue", 3);

// Benefits:
// 1. Built-in name/value access: Color::Red.Name(), Color::Red.Value()
// 2. Lookup by name: Color::FromName("Red")
// 3. Lookup by value: Color::FromValue(1)
// 4. Enumeration of all values: Color::List()
// 5. Case-insensitive lookup: Color::FromName("red", true)
```

## Basic Usage

### Creating a SmartEnum

```cpp
#include <SmartEnumCpp/SmartEnum.hpp>

// 1. Define your enum class
class Color : public SmartEnum<Color> {
public:
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    
private:
    Color(const std::string& name, int value) : SmartEnum(name, value) {}
};

// 2. Initialize the static constants
const Color Color::Red("Red", 1);
const Color Color::Green("Green", 2);
const Color Color::Blue("Blue", 3);
```

### Working with SmartEnum values

```cpp
// Get name and value
std::string name = Color::Red.Name();       // "Red"
int value = Color::Red.Value();             // 1
std::string str = Color::Green.ToString();  // "Green"

// Equality comparison
bool isEqual = (Color::Red == Color::Red);  // true
bool isNotEqual = (Color::Red != Color::Blue); // true
bool equals = Color::Green.Equals(Color::Green); // true

// Finding by name (with optional case insensitivity)
const Color& byName = Color::FromName("Blue");
const Color& byNameIgnoreCase = Color::FromName("blue", true);

// Try-pattern for safer lookup
const Color* result = nullptr;
if (Color::TryFromName("Purple", result)) {
    // Use result
}

// Finding by value
const Color& byValue = Color::FromValue(2); // Green

// Enumerating all values
for (const Color* color : Color::List()) {
    std::cout << color->Name() << ": " << color->Value() << std::endl;
}
```

## Advanced Features

### Custom Value Types

You can use different integer types for the underlying value:

```cpp
class Status : public SmartEnum<Status, uint8_t> {
    // Using uint8_t instead of int
};
```

### Exception Handling

```cpp
try {
    const Color& notFound = Color::FromName("Purple");
} catch (const SmartEnumNotFoundException& e) {
    std::cout << e.what() << std::endl;
}
```