#include <SmartEnumCpp/SmartFlagEnum.hpp>
#include <iostream>

// Define a flag enum for file permissions
class FilePermissions : public SmartFlagEnum<FilePermissions, unsigned int> {
public:
    static const FilePermissions None;
    static const FilePermissions Read;
    static const FilePermissions Write;
    static const FilePermissions Execute;
    static const FilePermissions Delete;
    
private:
    FilePermissions(const std::string& name, unsigned int value) : SmartFlagEnum(name, value) {}
};

// Initialize the flag values (must be powers of 2)
const FilePermissions FilePermissions::None("None", 0);
const FilePermissions FilePermissions::Read("Read", 1);      // 1 = 2^0
const FilePermissions FilePermissions::Write("Write", 2);    // 2 = 2^1
const FilePermissions FilePermissions::Execute("Execute", 4); // 4 = 2^2
const FilePermissions FilePermissions::Delete("Delete", 8);  // 8 = 2^3

// Helper function to print permissions
void printPermissions(unsigned int permValue) {
    std::cout << "Permissions value: " << permValue << std::endl;
    
    try {
        std::vector<const FilePermissions*> flags = FilePermissions::FromValue(permValue);
        std::string permStr = FilePermissions::FromValueToString(permValue);
        
        std::cout << "  As string: " << permStr << std::endl;
        std::cout << "  Individual flags:" << std::endl;
        for (const FilePermissions* flag : flags) {
            std::cout << "    - " << flag->Name() << " (" << flag->Value() << ")" << std::endl;
        }
    } catch (const InvalidFlagEnumValueParseException& e) {
        std::cout << "  Error: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "==== FilePermissions SmartFlagEnum Example ====\n" << std::endl;
    
    // Display all defined flags
    std::cout << "Defined flags:" << std::endl;
    for (const FilePermissions* flag : FilePermissions::List()) {
        std::cout << "  " << flag->Name() << " = " << flag->Value() << std::endl;
    }
    std::cout << std::endl;
    
    // Test single flags
    printPermissions(FilePermissions::Read);
    printPermissions(FilePermissions::Write);
    
    // Combine flags using the | operator
    unsigned int rwPermissions = FilePermissions::Read | FilePermissions::Write;
    printPermissions(rwPermissions);
    
    // More complex combination
    unsigned int fullAccess = FilePermissions::Read | FilePermissions::Write | 
                              FilePermissions::Execute | FilePermissions::Delete;
    printPermissions(fullAccess);
    
    // Parse from a comma-separated string
    try {
        std::vector<const FilePermissions*> parsedFlags = 
            FilePermissions::FromName("Read, Write, Execute");
        
        unsigned int combinedValue = 0;
        for (const FilePermissions* flag : parsedFlags) {
            combinedValue |= flag->Value();
        }
        
        std::cout << "Parsed from string 'Read, Write, Execute':" << std::endl;
        printPermissions(combinedValue);
    } catch (const InvalidFlagEnumValueParseException& e) {
        std::cout << "Parse error: " << e.what() << std::endl;
    }
    
    // Try to parse an invalid value (not a combination of defined flags)
    try {
        printPermissions(16); // Not a defined flag value
    } catch (const std::exception& e) {
        std::cout << "Expected error: " << e.what() << std::endl;
    }
    
    return 0;
}