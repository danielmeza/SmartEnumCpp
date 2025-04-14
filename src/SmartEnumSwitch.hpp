/**
 * @file SmartEnumSwitch.hpp
 * @brief Implements a fluent switch interface for SmartEnums.
 *
 * This utility allows you to write code that chains conditions similarly
 * to C#'s fluent switch expressions.
 *
 * Example:
 * @code
 * SwitchOn(myEnum)
 *   .When(MyEnum::One).Then([](){ ... })
 *   .When(MyEnum::Two).Then([](){ ... })
 *   .Default([](){ ... });
 * @endcode
 */

#ifndef SMARTENUMSWITCH_HPP
#define SMARTENUMSWITCH_HPP

#include <functional>

template<typename EnumType>
class SmartEnumSwitchBuilder {
public:
    /**
     * @brief Constructs a builder for the given enum value.
     * @param value The enum value to switch on
     */
    explicit SmartEnumSwitchBuilder(const EnumType& value) 
        : value_(value), handled_(false), lastMatch_(false) {}

    /**
     * @brief Checks if the enum matches the candidate.
     */
    inline SmartEnumSwitchBuilder& When(const EnumType& candidate) {
        if (!handled_ && value_ == candidate) {
            lastMatch_ = true;
        } else {
            lastMatch_ = false;
        }
        return *this;
    }

    /**
     * @brief Executes the action if the previous When() matched.
     */
    inline SmartEnumSwitchBuilder& Then(const std::function<void()>& action) {
        if (!handled_ && lastMatch_) {
            action();
            handled_ = true;
        }
        lastMatch_ = false;
        return *this;
    }

    /**
     * @brief Executes the default action if no case was handled.
     */
    inline void Default(const std::function<void()>& action) {
        if (!handled_) {
            action();
        }
    }
private:
    const EnumType& value_;
    bool handled_;
    bool lastMatch_;
};

/**
 * @brief Creates a SmartEnumSwitchBuilder for the given enum value.
 */
template<typename EnumType>
inline SmartEnumSwitchBuilder<EnumType> SwitchOn(const EnumType& enumValue) {
    return SmartEnumSwitchBuilder<EnumType>(enumValue);
}

#endif // SMARTENUMSWITCH_HPP
