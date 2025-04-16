#include <SmartEnumCpp/SmartEnum.hpp>
#include <iostream>

// Define a basic SmartEnum for colors
class Color : public SmartEnum<Color> {
public:
    // Define enum constants
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Purple;
    
private:
    // Constructor is private, constants are defined outside the class
    Color(const std::string& name, int value) : SmartEnum(name, value) {}
};

// Initialize the constants
const Color Color::Red("Red", 1);
const Color Color::Green("Green", 2);
const Color Color::Blue("Blue", 3);
const Color Color::Yellow("Yellow", 4);
const Color Color::Purple("Purple", 5);

int main() {
    // Access enum values
    std::cout << "Color Red: " << Color::Red.Name() << " = " << Color::Red.Value() << std::endl;
    
    // Lookup by name (case sensitive and insensitive)
    try {
        const Color& foundColor = Color::FromName("Green");
        std::cout << "Found color by exact name: " << foundColor.Name() << std::endl;
        
        const Color& foundColorInsensitive = Color::FromName("blue", true); // case insensitive
        std::cout << "Found color by case insensitive name: " << foundColorInsensitive.Name() << std::endl;
        
        // This will throw an exception
        const Color& notFoundColor = Color::FromName("Orange");
    } catch (const SmartEnumNotFoundException& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    // Try lookup pattern
    const Color* tryColor = nullptr;
    if (Color::TryFromName("Purple", tryColor)) {
        std::cout << "Successfully found color: " << tryColor->Name() << std::endl;
    } else {
        std::cout << "Color not found" << std::endl;
    }
    
    // Lookup by value
    const Color& colorByValue = Color::FromValue(3);
    std::cout << "Color with value 3: " << colorByValue.Name() << std::endl;
    
    // List all defined values
    std::cout << "\nAll colors:" << std::endl;
    for (const Color* color : Color::List()) {
        std::cout << " - " << color->Name() << " (" << color->Value() << ")" << std::endl;
    }
    
    // Equality comparison
    if (Color::Red == Color::FromValue(1)) {
        std::cout << "\nColor::Red equals Color::FromValue(1)" << std::endl;
    }
    
    if (Color::Green != Color::Blue) {
        std::cout << "Color::Green does not equal Color::Blue" << std::endl;
    }
    
    return 0;
}