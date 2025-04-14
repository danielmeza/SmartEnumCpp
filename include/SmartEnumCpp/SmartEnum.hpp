/**
 * @file SmartEnum.hpp
 * @brief Provides the base template for creating SmartEnum types.
 *
 * This header defines the SmartEnum template that allows you to create
 * type-safe enums with named instances, underlying values, and custom behavior.
 *
 * Example:
 * @code
 * class TestEnum : public SmartEnum<TestEnum> {
 * public:
 *     static const TestEnum One;
 *     static const TestEnum Two;
 * private:
 *     TestEnum(const std::string& name, int value) : SmartEnum(name, value) {}
 * };
 * const TestEnum TestEnum::One("One", 1);
 * const TestEnum TestEnum::Two("Two", 2);
 * @endcode
 */

#ifndef SMARTENUM_HPP
#define SMARTENUM_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <mutex>
#include <stdexcept>
#include <typeinfo>

/**
 * @brief Exception thrown when a SmartEnum lookup fails.
 */
class SmartEnumNotFoundException : public std::runtime_error {
public:
    explicit SmartEnumNotFoundException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Template base class for creating SmartEnum types.
 * 
 * @tparam TEnum The derived SmartEnum type.
 * @tparam TValue The underlying value type (default is int).
 */
template <typename TEnum, typename TValue = int>
class SmartEnum {
public:
    using ValueType = TValue;
    using EnumType = TEnum;

    SmartEnum(const SmartEnum&) = delete;
    SmartEnum& operator=(const SmartEnum&) = delete;

    /**
     * @brief Gets the name of the enum instance.
     */
    inline const std::string& Name() const { return name_; }
    
    /**
     * @brief Gets the underlying value of the enum instance.
     */
    inline const ValueType& Value() const { return value_; }

    /**
     * @brief Equality operator compares underlying values.
     */
    inline bool operator==(const SmartEnum& other) const { return value_ == other.value_; }
    inline bool operator!=(const SmartEnum& other) const { return !(*this == other); }
    
    /**
     * @brief Implicit conversion to the underlying value type.
     */
    inline operator TValue () const { return value_; }

    /**
     * @brief Compares equality based on the underlying value.
     */
    inline bool Equals(const TEnum& other) const { return value_ == other.Value(); }

    /**
     * @brief Returns the string representation (the name).
     */
    inline std::string ToString() const { return name_; }
    inline operator std::string() const { return ToString(); }

    /**
     * @brief Returns a list of all defined enum instances.
     */
    static const std::vector<const TEnum*>& List() { return instances(); }

    /**
     * @brief Returns an enum instance by name.
     * 
     * @param name The name of the enum instance.
     * @param ignoreCase If true, perform a case-insensitive search.
     * @return The matching enum instance.
     * @throws SmartEnumNotFoundException if not found.
     */
    static const TEnum& FromName(const std::string& name, bool ignoreCase = false);
    
    /**
     * @brief Tries to get an enum instance by name.
     * 
     * @param name The name to search.
     * @param outResult Pointer to the found enum instance.
     * @param ignoreCase If true, perform a case-insensitive search.
     * @return true if found; false otherwise.
     */
    static bool TryFromName(const std::string& name, const TEnum*& outResult, bool ignoreCase = false);
    
    /**
     * @brief Returns an enum instance by its underlying value.
     * 
     * @param value The underlying value.
     * @return The matching enum instance.
     * @throws SmartEnumNotFoundException if not found.
     */
    static const TEnum& FromValue(const ValueType& value);
    
    /**
     * @brief Tries to get an enum instance by its value.
     * 
     * @param value The underlying value.
     * @param outResult Pointer to the found enum instance.
     * @return true if found; false otherwise.
     */
    static bool TryFromValue(const ValueType& value, const TEnum*& outResult);

protected:
    /**
     * @brief Protected constructor. Registers this instance.
     *
     * @param name The unique name of the enum instance.
     * @param value The underlying value.
     */
    SmartEnum(const std::string& name, const ValueType& value);
    ~SmartEnum() = default;

private:
    std::string name_;
    ValueType value_;

    static std::vector<const TEnum*>& instances();
    static std::map<std::string, const TEnum*>& nameMap();
    static std::map<std::string, const TEnum*>& nameMapIgnoreCase();
    static std::map<ValueType, const TEnum*>& valueMap();
    static std::once_flag listInitFlag_;

    static std::string valueToString(const ValueType& val);
    static void registerInstance(const TEnum* instance);
    static const TEnum* TryFromNameInternal(const std::string& name, bool ignoreCase);
    static const TEnum* TryFromValueInternal(const ValueType& value);
};

// Template method implementations for SmartEnum

template <typename TEnum, typename TValue>
const TEnum& SmartEnum<TEnum, TValue>::FromName(const std::string& name, bool ignoreCase) {
    const TEnum* result = nullptr;
    if (!TryFromName(name, result, ignoreCase)) {
        throw SmartEnumNotFoundException("No " + std::string(typeid(TEnum).name()) +
                                         " with name \"" + name + "\" found");
    }
    return *result;
}

template <typename TEnum, typename TValue>
bool SmartEnum<TEnum, TValue>::TryFromName(const std::string& name, const TEnum*& outResult, bool ignoreCase) {
    outResult = TryFromNameInternal(name, ignoreCase);
    return outResult != nullptr;
}

template <typename TEnum, typename TValue>
const TEnum& SmartEnum<TEnum, TValue>::FromValue(const ValueType& value) {
    const TEnum* result = nullptr;
    if (!TryFromValue(value, result)) {
        throw SmartEnumNotFoundException("No " + std::string(typeid(TEnum).name()) +
                                         " with value \"" + valueToString(value) + "\" found");
    }
    return *result;
}

template <typename TEnum, typename TValue>
bool SmartEnum<TEnum, TValue>::TryFromValue(const ValueType& value, const TEnum*& outResult) {
    outResult = TryFromValueInternal(value);
    return outResult != nullptr;
}

template <typename TEnum, typename TValue>
SmartEnum<TEnum, TValue>::SmartEnum(const std::string& name, const ValueType& value) : name_(name), value_(value) {
    if (name.empty()) {
        throw std::invalid_argument("SmartEnum name cannot be empty");
    }
    registerInstance(static_cast<const TEnum*>(this));
}

template <typename TEnum, typename TValue>
std::vector<const TEnum*>& SmartEnum<TEnum, TValue>::instances() {
    static std::vector<const TEnum*> instanceList;
    return instanceList;
}

template <typename TEnum, typename TValue>
std::map<std::string, const TEnum*>& SmartEnum<TEnum, TValue>::nameMap() {
    static std::map<std::string, const TEnum*> m;
    return m;
}

template <typename TEnum, typename TValue>
std::map<std::string, const TEnum*>& SmartEnum<TEnum, TValue>::nameMapIgnoreCase() {
    static std::map<std::string, const TEnum*> m;
    return m;
}

template <typename TEnum, typename TValue>
std::map<typename SmartEnum<TEnum, TValue>::ValueType, const TEnum*>& SmartEnum<TEnum, TValue>::valueMap() {
    static std::map<ValueType, const TEnum*> m;
    return m;
}

template <typename TEnum, typename TValue>
std::once_flag SmartEnum<TEnum, TValue>::listInitFlag_;

template <typename TEnum, typename TValue>
std::string SmartEnum<TEnum, TValue>::valueToString(const ValueType& val) {
    return std::to_string(static_cast<long long>(val));
}

template <typename TEnum, typename TValue>
void SmartEnum<TEnum, TValue>::registerInstance(const TEnum* instance) {
    if (!instance) return;
    
    const std::string& nm = instance->Name();
    if (nameMap().count(nm)) {
        throw std::runtime_error("Duplicate SmartEnum name \"" + nm + "\"");
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
const TEnum* SmartEnum<TEnum, TValue>::TryFromNameInternal(const std::string& name, bool ignoreCase) {
    if (ignoreCase) {
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        auto it = nameMapIgnoreCase().find(lower);
        return it != nameMapIgnoreCase().end() ? it->second : nullptr;
    } else {
        auto it = nameMap().find(name);
        return it != nameMap().end() ? it->second : nullptr;
    }
}

template <typename TEnum, typename TValue>
const TEnum* SmartEnum<TEnum, TValue>::TryFromValueInternal(const ValueType& value) {
    auto it = valueMap().find(value);
    return it != valueMap().end() ? it->second : nullptr;
}

#endif // SMARTENUM_HPP
