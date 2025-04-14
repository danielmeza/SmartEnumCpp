/**
 * @file SmartFlagEnum.hpp
 * @brief Defines a SmartFlagEnum template for bitfield-style enums.
 *
 * This header provides a template to define enums that behave as bit flags.
 * It includes functionality for combining flags, validating power-of-two values,
 * and converting between combined values and their corresponding flag instances.
 */

#ifndef SMARTFLAGENUM_HPP
#define SMARTFLAGENUM_HPP

#include <vector>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <cctype>
#include <typeinfo>

// Marker types to modify behavior of flag enums.
struct AllowNegativeFlagEnumInput {};
struct AllowUnsafeFlagEnumValues {};

/**
 * @brief Exception for invalid flag enum parsing.
 */
class InvalidFlagEnumValueParseException : public std::runtime_error {
public:
    explicit InvalidFlagEnumValueParseException(const std::string& msg) : std::runtime_error(msg) {}
};
/**
 * @brief Exception for negative flag input when not allowed.
 */
class NegativeFlagValueNotAllowedException : public std::runtime_error {
public:
    explicit NegativeFlagValueNotAllowedException(const std::string& msg) : std::runtime_error(msg) {}
};
/**
 * @brief Exception when flag definitions do not follow required power-of-two rules.
 */
class SmartFlagEnumNotPowerOfTwoException : public std::runtime_error {
public:
    explicit SmartFlagEnumNotPowerOfTwoException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Base template for creating flag (bitfield) SmartEnums.
 *
 * @tparam TEnum The derived flag enum type.
 * @tparam TValue The underlying integral type (default is int).
 */
template <typename TEnum, typename TValue = int>
class SmartFlagEnum {
    static_assert(std::is_integral<TValue>::value || std::is_enum<TValue>::value,
                  "SmartFlagEnum requires an integral underlying type");
public:
    using ValueType = TValue;
    using EnumType = TEnum;

    SmartFlagEnum(const SmartFlagEnum&) = delete;
    SmartFlagEnum& operator=(const SmartFlagEnum&) = delete;

    /**
     * @brief Gets the flag instance's name.
     */
    inline const std::string& Name() const { return name_; }
    
    /**
     * @brief Gets the flag instance's underlying value.
     */
    inline const ValueType& Value() const { return value_; }

    /**
     * @brief Converts this flag instance to its string representation.
     */
    inline std::string ToString() const { return name_; }
    inline bool Equals(const TEnum& other) const { return value_ == other.Value(); }
    inline bool operator==(const SmartFlagEnum& other) const { return value_ == other.value_; }
    inline bool operator!=(const SmartFlagEnum& other) const { return !(*this == other); }
    inline operator TValue () const { return value; }
    /**
     * @brief Returns a list of all flag instances.
     */
    static const std::vector<const TEnum*>& List() {
        enforceFlagDefinitions();
        return instances();
    }

    /**
     * @brief Returns flag instances by a comma-separated list of names.
     * 
     * @param names Comma-separated flag names.
     * @param ignoreCase If true, the lookup is case-insensitive.
     * @return A vector of matching flag instances.
     * @throws SmartEnumNotFoundException if any name is not found.
     */
    static std::vector<const TEnum*> FromName(const std::string& names, bool ignoreCase = false);
    
    /**
     * @brief Tries to parse comma-separated flag names.
     */
    static bool TryFromName(const std::string& names, std::vector<const TEnum*>& outResult, bool ignoreCase = false);

    /**
     * @brief Returns flag instances corresponding to a combined value.
     *
     * If the value exactly matches an explicit flag instance, that instance is returned.
     * Otherwise, the method computes the individual flags set in the value.
     * Special handling exists for -1 (representing "All").
     *
     * @param value The combined flag value.
     * @return A vector of flag instances.
     * @throws SmartEnumNotFoundException if the value cannot be matched.
     */
    static std::vector<const TEnum*> FromValue(const ValueType& value);
    
    /**
     * @brief Tries to interpret a combined flag value.
     */
    static bool TryFromValue(const ValueType& value, std::vector<const TEnum*>& outResult);

    /**
     * @brief Converts a combined flag value into a comma-separated string of flag names.
     */
    static std::string FromValueToString(const ValueType& value);
    
    /**
     * @brief Tries to convert a combined flag value into its string representation.
     */
    static bool TryFromValueToString(const ValueType& value, std::string& outStr);

protected:
    /**
     * @brief Protected constructor. Registers the flag instance.
     *
     * @param name The unique flag name.
     * @param value The flag's value.
     */
    SmartFlagEnum(const std::string& name, const ValueType& value);
    ~SmartFlagEnum() = default;

private:
    std::string name_;
    ValueType value_;

    static std::vector<const TEnum*>& instances();
    static std::map<std::string, const TEnum*>& nameMap();
    static std::map<std::string, const TEnum*>& nameMapIgnoreCase();
    static std::map<ValueType, const TEnum*>& valueMap();
    static bool& definitionsValidated();

    static void registerInstance(const TEnum* instance);
    static void enforceFlagDefinitions();
    static const TEnum* findByName(const std::string& name);
    static const TEnum* findByNameCaseInsensitive(const std::string& name);
    static inline bool isPowerOfTwo(ValueType v) { return v > 0 && (v & (v - 1)) == 0; }
    static bool fitsInDefinedFlags(ValueType input);
};

template<typename TEnum, typename TValue,
         typename = std::enable_if_t<std::is_integral<TValue>::value>>
inline TValue operator|(const SmartFlagEnum<TEnum, TValue>& a, const SmartFlagEnum<TEnum, TValue>& b) {
    return static_cast<TValue>(a.Value() | b.Value());
}

// Template implementations for SmartFlagEnum
template <typename TEnum, typename TValue>
std::vector<const TEnum*> SmartFlagEnum<TEnum, TValue>::FromName(const std::string& names, bool ignoreCase) {
    std::vector<const TEnum*> result;
    if (!TryFromName(names, result, ignoreCase)) {
        throw InvalidFlagEnumValueParseException(
            "Failed to parse one or more flags in \"" + names + "\" for type " + std::string(typeid(TEnum).name()));
    }
    return result;
}

template <typename TEnum, typename TValue>
bool SmartFlagEnum<TEnum, TValue>::TryFromName(
    const std::string& names, std::vector<const TEnum*>& outResult, bool ignoreCase) {
    
    outResult.clear();
    if (names.empty()) {
        return true;
    }

    size_t start = 0;
    size_t end;
    
    do {
        end = names.find(",", start);
        std::string part = names.substr(start, end == std::string::npos ? end : end - start);
        
        // Trim whitespace
        part.erase(0, part.find_first_not_of(" \t"));
        part.erase(part.find_last_not_of(" \t") + 1);
        
        if (!part.empty()) {
            const TEnum* foundFlag = ignoreCase ? 
                findByNameCaseInsensitive(part) : findByName(part);
                
            if (!foundFlag) {
                return false;
            }
            
            outResult.push_back(foundFlag);
        }
        
        start = end + 1;
    } while (end != std::string::npos);
    
    return true;
}

template <typename TEnum, typename TValue>
std::vector<const TEnum*> SmartFlagEnum<TEnum, TValue>::FromValue(const ValueType& value) {
    std::vector<const TEnum*> result;
    if (!TryFromValue(value, result)) {
        throw InvalidFlagEnumValueParseException(
            "Value " + std::to_string(static_cast<long long>(value)) + 
            " could not be converted to a valid flag for " + std::string(typeid(TEnum).name()));
    }
    return result;
}

template <typename TEnum, typename TValue>
bool SmartFlagEnum<TEnum, TValue>::TryFromValue(const ValueType& value, std::vector<const TEnum*>& outResult) {
    enforceFlagDefinitions();
    outResult.clear();
    
    // Check if this is an exact match for an existing flag
    auto it = valueMap().find(value);
    if (it != valueMap().end()) {
        outResult.push_back(it->second);
        return true;
    }
    
    // Special case for negative values (often used for "All")
    if (value < 0) {
        try {
            using AllowNeg = AllowNegativeFlagEnumInput;
            AllowNeg* p = nullptr;
            delete p; // This will fail at runtime - we only use it to trigger linking error
            // This will never execute - the line above will throw
            return false;
        }
        catch (...) {
            throw NegativeFlagValueNotAllowedException(
                "Negative flag value " + std::to_string(static_cast<long long>(value)) + 
                " not allowed for type " + std::string(typeid(TEnum).name()));
        }
    }
    
    // Handle combined flags
    if (fitsInDefinedFlags(value)) {
        ValueType remainingValue = value;
        const auto& allFlags = List();
        
        // Start with the largest value flags and work down
        std::vector<const TEnum*> sortedFlags = allFlags;
        std::sort(sortedFlags.begin(), sortedFlags.end(), 
            [](const TEnum* a, const TEnum* b) {
                return a->Value() > b->Value();
            });
        
        for (const TEnum* flag : sortedFlags) {
            ValueType flagValue = flag->Value();
            if ((remainingValue & flagValue) == flagValue) {
                outResult.push_back(flag);
                remainingValue &= ~flagValue;
            }
            
            if (remainingValue == 0) {
                break;
            }
        }
        
        return remainingValue == 0;
    }
    
    return false;
}

template <typename TEnum, typename TValue>
std::string SmartFlagEnum<TEnum, TValue>::FromValueToString(const ValueType& value) {
    std::string result;
    if (!TryFromValueToString(value, result)) {
        throw InvalidFlagEnumValueParseException(
            "Value " + std::to_string(static_cast<long long>(value)) + 
            " could not be converted to a valid flag string for " + std::string(typeid(TEnum).name()));
    }
    return result;
}

template <typename TEnum, typename TValue>
bool SmartFlagEnum<TEnum, TValue>::TryFromValueToString(const ValueType& value, std::string& outStr) {
    std::vector<const TEnum*> flags;
    if (!TryFromValue(value, flags)) {
        return false;
    }
    
    outStr.clear();
    for (size_t i = 0; i < flags.size(); ++i) {
        if (i > 0) {
            outStr += ", ";
        }
        outStr += flags[i]->Name();
    }
    return true;
}

template <typename TEnum, typename TValue>
SmartFlagEnum<TEnum, TValue>::SmartFlagEnum(const std::string& name, const ValueType& value) 
    : name_(name), value_(value) {
    if (name.empty()) {
        throw std::invalid_argument("SmartFlagEnum name cannot be empty");
    }
    registerInstance(static_cast<const TEnum*>(this));
}

template <typename TEnum, typename TValue>
std::vector<const TEnum*>& SmartFlagEnum<TEnum, TValue>::instances() {
    static std::vector<const TEnum*> instanceList;
    return instanceList;
}

template <typename TEnum, typename TValue>
std::map<std::string, const TEnum*>& SmartFlagEnum<TEnum, TValue>::nameMap() {
    static std::map<std::string, const TEnum*> m;
    return m;
}

template <typename TEnum, typename TValue>
std::map<std::string, const TEnum*>& SmartFlagEnum<TEnum, TValue>::nameMapIgnoreCase() {
    static std::map<std::string, const TEnum*> m;
    return m;
}

template <typename TEnum, typename TValue>
std::map<typename SmartFlagEnum<TEnum, TValue>::ValueType, const TEnum*>& SmartFlagEnum<TEnum, TValue>::valueMap() {
    static std::map<ValueType, const TEnum*> m;
    return m;
}

template <typename TEnum, typename TValue>
bool& SmartFlagEnum<TEnum, TValue>::definitionsValidated() {
    static bool validated = false;
    return validated;
}

template <typename TEnum, typename TValue>
void SmartFlagEnum<TEnum, TValue>::registerInstance(const TEnum* instance) {
    if (!instance) return;
    
    const std::string& nm = instance->Name();
    if (nameMap().count(nm)) {
        throw std::runtime_error("Duplicate SmartFlagEnum name \"" + nm + "\"");
    }
    
    instances().push_back(instance);
    nameMap()[nm] = instance;
    
    std::string lower = nm;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (!nameMapIgnoreCase().count(lower)) {
        nameMapIgnoreCase()[lower] = instance;
    }
    
    const ValueType& val = instance->Value();
    if (!valueMap().count(val)) {
        valueMap()[val] = instance;
    }
}

template <typename TEnum, typename TValue>
void SmartFlagEnum<TEnum, TValue>::enforceFlagDefinitions() {
    if (definitionsValidated()) {
        return;
    }
    
    for (const TEnum* instance : instances()) {
        ValueType value = instance->Value();
        if (!isPowerOfTwo(value)) {
            try {
                using AllowUnsafe = AllowUnsafeFlagEnumValues;
                AllowUnsafe* p = nullptr;
                delete p; // This will fail at runtime - trigger linking error
                break;
            }
            catch (...) {
                throw SmartFlagEnumNotPowerOfTwoException(
                    "Flag value " + std::to_string(static_cast<long long>(value)) + 
                    " for flag \"" + instance->Name() + "\" is not a power of two");
            }
        }
    }
    
    definitionsValidated() = true;
}

template <typename TEnum, typename TValue>
const TEnum* SmartFlagEnum<TEnum, TValue>::findByName(const std::string& name) {
    auto it = nameMap().find(name);
    return it != nameMap().end() ? it->second : nullptr;
}

template <typename TEnum, typename TValue>
const TEnum* SmartFlagEnum<TEnum, TValue>::findByNameCaseInsensitive(const std::string& name) {
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    auto it = nameMapIgnoreCase().find(lower);
    return it != nameMapIgnoreCase().end() ? it->second : nullptr;
}

template <typename TEnum, typename TValue>
bool SmartFlagEnum<TEnum, TValue>::fitsInDefinedFlags(ValueType input) {
    ValueType allFlags = 0;
    for (const TEnum* instance : instances()) {
        allFlags |= instance->Value();
    }
    
    // Check if input has any bits set that aren't in allFlags
    return (input & ~allFlags) == 0;
}

#endif // SMARTFLAGENUM_HPP
