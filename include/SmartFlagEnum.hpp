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
     const std::string& Name() const { return name_; }
     /**
      * @brief Gets the flag instance's underlying value.
      */
     const ValueType& Value() const { return value_; }
 
     /**
      * @brief Converts this flag instance to its string representation.
      */
     std::string ToString() const { return name_; }
     bool Equals(const TEnum& other) const { return value_ == other.Value(); }
     bool operator==(const SmartFlagEnum& other) const { return value_ == other.value_; }
     bool operator!=(const SmartFlagEnum& other) const { return !(*this == other); }
 
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
     static std::vector<const TEnum*> FromName(const std::string& names, bool ignoreCase = false) {
         std::vector<const TEnum*> result;
         if (!TryFromName(names, result, ignoreCase)) {
             throw SmartEnumNotFoundException("No " + std::string(typeid(TEnum).name()) +
                                              " with names \"" + names + "\" found");
         }
         return result;
     }
     /**
      * @brief Tries to parse comma-separated flag names.
      */
     static bool TryFromName(const std::string& names, std::vector<const TEnum*>& outResult, bool ignoreCase = false) {
         outResult.clear();
         if (names.empty()) return false;
         enforceFlagDefinitions();
         size_t start = 0;
         bool fail = false;
         while (start < names.size()) {
             size_t end = names.find(',', start);
             std::string oneName = (end == std::string::npos ? names.substr(start)
                                                            : names.substr(start, end - start));
             while (!oneName.empty() && isspace((unsigned char)oneName.front())) oneName.erase(0, 1);
             while (!oneName.empty() && isspace((unsigned char)oneName.back())) oneName.pop_back();
             if (!oneName.empty()) {
                 const TEnum* inst = (ignoreCase ? findByNameCaseInsensitive(oneName)
                                                 : findByName(oneName));
                 if (!inst) { fail = true; break; }
                 outResult.push_back(inst);
             }
             if (end == std::string::npos) break;
             start = end + 1;
         }
         if (fail || outResult.empty()) {
             outResult.clear();
             return false;
         }
         return true;
     }
 
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
     static std::vector<const TEnum*> FromValue(const ValueType& value) {
         std::vector<const TEnum*> result;
         if (!TryFromValue(value, result)) {
             throw SmartEnumNotFoundException("No " + std::string(typeid(TEnum).name()) +
                                              " with value \"" + std::to_string(static_cast<long long>(value)) + "\" found");
         }
         return result;
     }
     /**
      * @brief Tries to interpret a combined flag value.
      */
     static bool TryFromValue(const ValueType& value, std::vector<const TEnum*>& outResult) {
         outResult.clear();
         enforceFlagDefinitions();
         bool allowNegative = std::is_base_of<AllowNegativeFlagEnumInput, TEnum>::value;
         long long valInt = (long long) value;
         if (valInt < 0 && valInt != -1) {
             if (!allowNegative) {
                 return false;
             }
         }
         auto itExact = valueMap().find(value);
         if (itExact != valueMap().end()) {
             outResult.push_back(itExact->second);
             return true;
         }
         if (valInt == -1) {
             for (const TEnum* e : instances()) {
                 if (e->Value() != ValueType(0)) {
                     outResult.push_back(e);
                 }
             }
             return !outResult.empty();
         }
         if (!fitsInDefinedFlags(value)) {
             return false;
         }
         for (const TEnum* flag : instances()) {
             ValueType flagVal = flag->Value();
             if (flagVal == ValueType(0) || flagVal == ValueType(-1)) continue;
             if (isPowerOfTwo(flagVal)) {
                 if ((value & flagVal) == flagVal) {
                     outResult.push_back(flag);
                 }
             }
         }
         if (outResult.empty()) {
             return false;
         }
         return true;
     }
 
     /**
      * @brief Converts a combined flag value into a comma-separated string of flag names.
      */
     static std::string FromValueToString(const ValueType& value) {
         std::vector<const TEnum*> flags = FromValue(value);
         std::string result;
         for (size_t i = 0; i < flags.size(); ++i) {
             result += flags[i]->Name();
             if (i < flags.size() - 1) result += ", ";
         }
         return result;
     }
     /**
      * @brief Tries to convert a combined flag value into its string representation.
      */
     static bool TryFromValueToString(const ValueType& value, std::string& outStr) {
         std::vector<const TEnum*> flags;
         if (!TryFromValue(value, flags)) return false;
         outStr.clear();
         for (size_t i = 0; i < flags.size(); ++i) {
             outStr += flags[i]->Name();
             if (i < flags.size() - 1) outStr += ", ";
         }
         return true;
     }
 
 protected:
     /**
      * @brief Protected constructor. Registers the flag instance.
      *
      * @param name The unique flag name.
      * @param value The flag’s value.
      */
     SmartFlagEnum(const std::string& name, const ValueType& value) : name_(name), value_(value) {
         if (name.empty()) {
             throw std::invalid_argument("SmartFlagEnum name cannot be empty");
         }
         registerInstance(static_cast<TEnum*>(this));
     }
     ~SmartFlagEnum() = default;
 
 private:
     std::string name_;
     ValueType value_;
 
     static std::vector<const TEnum*>& instances() {
         static std::vector<const TEnum*> instanceList;
         return instanceList;
     }
     static std::map<std::string, const TEnum*>& nameMap() {
         static std::map<std::string, const TEnum*> m;
         return m;
     }
     static std::map<std::string, const TEnum*>& nameMapIgnoreCase() {
         static std::map<std::string, const TEnum*> m;
         return m;
     }
     static std::map<ValueType, const TEnum*>& valueMap() {
         static std::map<ValueType, const TEnum*> m;
         return m;
     }
     static bool& definitionsValidated() {
         static bool validated = false;
         return validated;
     }
 
     static void registerInstance(const TEnum* instance) {
         instances().push_back(instance);
         const std::string& nm = instance->Name();
         if (nameMap().count(nm)) {
             throw std::runtime_error("Duplicate SmartFlagEnum name \"" + nm + "\"");
         }
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
         definitionsValidated() = false;
     }
 
     static void enforceFlagDefinitions() {
         if (definitionsValidated()) return;
         std::sort(instances().begin(), instances().end(),
                   [](const TEnum* a, const TEnum* b) {
                       return (long long)a->Value() < (long long)b->Value();
                   });
         bool allowUnsafe = std::is_base_of<AllowUnsafeFlagEnumValues, TEnum>::value;
         std::vector<ValueType> baseFlags;
         for (const TEnum* e : instances()) {
             ValueType val = e->Value();
             if (val == ValueType(0) || val == ValueType(-1)) continue;
             if (isPowerOfTwo(val)) {
                 baseFlags.push_back(val);
             } else {
                 if (!allowUnsafe) {
                     throw SmartFlagEnumNotPowerOfTwoException(
                         "Value " + std::to_string((long long)val) +
                         " is not a power of two in " + std::string(typeid(TEnum).name()));
                 }
             }
         }
         if (!baseFlags.empty()) {
             std::sort(baseFlags.begin(), baseFlags.end());
             ValueType expected = ValueType(1);
             for (ValueType flag : baseFlags) {
                 if (flag != expected) {
                     if (!allowUnsafe) {
                         throw SmartFlagEnumNotPowerOfTwoException(
                             "Missing power-of-two flag value " +
                             std::to_string((long long)expected) + " in " +
                             std::string(typeid(TEnum).name()));
                     }
                 }
                 if (expected > 0) expected <<= 1;
             }
         }
         definitionsValidated() = true;
     }
 
     static const TEnum* findByName(const std::string& name) {
         auto it = nameMap().find(name);
         return it != nameMap().end() ? it->second : nullptr;
     }
     static const TEnum* findByNameCaseInsensitive(const std::string& name) {
         std::string lower = name;
         std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
         auto it = nameMapIgnoreCase().find(lower);
         return it != nameMapIgnoreCase().end() ? it->second : nullptr;
     }
     static bool isPowerOfTwo(ValueType v) {
         unsigned long long x = (unsigned long long)v;
         return x != 0 && (x & (x - 1)) == 0;
     }
     static bool fitsInDefinedFlags(ValueType input) {
         unsigned long long maxFlag = 0ULL;
         for (const TEnum* e : instances()) {
             ValueType val = e->Value();
             if (val == ValueType(0) || val == ValueType(-1)) continue;
             if (isPowerOfTwo(val) && (unsigned long long)val > maxFlag) {
                 maxFlag = (unsigned long long)val;
             }
         }
         if (maxFlag == 0ULL) return true;
         unsigned long long mask = (maxFlag << 1) - 1ULL;
         unsigned long long bits = (unsigned long long) input;
         return (bits & ~mask) == 0ULL;
     }
 };
 
 template<typename TEnum, typename TValue,
          typename = std::enable_if_t<std::is_integral<TValue>::value>>
 TValue operator|(const SmartFlagEnum<TEnum, TValue>& a, const SmartFlagEnum<TEnum, TValue>& b) {
     return a.Value() | b.Value();
 }
 
 #endif // SMARTFLAGENUM_HPP
 