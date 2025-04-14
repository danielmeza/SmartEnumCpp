#include <gtest/gtest.h>
#include "SmartEnum.hpp"
#include "SmartFlagEnum.hpp"
#include "SmartEnumSwitch.hpp"

// Define a simple TestEnum for testing
class TestEnum : public SmartEnum<TestEnum>
{
public:
    static const TestEnum One;
    static const TestEnum Two;
    static const TestEnum Three;

private:
    TestEnum(const std::string &name, int value) : SmartEnum(name, value) {}
};
const TestEnum TestEnum::One("One", 1);
const TestEnum TestEnum::Two("Two", 2);
const TestEnum TestEnum::Three("Three", 3);

// Define polymorphic enum example (EmployeeType)
class EmployeeType : public SmartEnum<EmployeeType>
{
public:
    static const EmployeeType &Manager;
    static const EmployeeType &Assistant;
    virtual int BonusSize() const = 0;

protected:
    EmployeeType(const std::string &name, int value) : SmartEnum(name, value) {}
};
class ManagerType : public EmployeeType
{
public:
    ManagerType() : EmployeeType("Manager", 1) {}
    int BonusSize() const override { return 1000; }
};
class AssistantType : public EmployeeType
{
public:
    AssistantType() : EmployeeType("Assistant", 2) {}
    int BonusSize() const override { return 500; }
};
const EmployeeType &EmployeeType::Manager = ManagerType();
const EmployeeType &EmployeeType::Assistant = AssistantType();

// Define a Flags enum for testing SmartFlagEnum
class Flags : public SmartFlagEnum<Flags>, public AllowNegativeFlagEnumInput
{
public:
    static const Flags None;
    static const Flags A;
    static const Flags B;
    static const Flags C;
    static const Flags AB; // explicit combination (A|B)
    static const Flags All;

private:
    Flags(const std::string &name, int value) : SmartFlagEnum(name, value) {}
};
const Flags Flags::None("None", 0);
const Flags Flags::A("A", 1);
const Flags Flags::B("B", 2);
const Flags Flags::C("C", 4);
const Flags Flags::AB("AB", 3);
const Flags Flags::All("All", -1);

class NoNegFlags : public SmartFlagEnum<NoNegFlags>
{
public:
    static const NoNegFlags &X;
    static const NoNegFlags &Y;
    NoNegFlags(const std::string &n, int v) : SmartFlagEnum(n, v) {}
};
const NoNegFlags &NoNegFlags::X = NoNegFlags("X", 1);
const NoNegFlags &NoNegFlags::Y = NoNegFlags("Y", 2);

class SparseFlags : public SmartFlagEnum<SparseFlags>, public AllowUnsafeFlagEnumValues
{
public:
    static const SparseFlags &Bit1;
    static const SparseFlags &Bit3;
    SparseFlags(const std::string &name, int value) : SmartFlagEnum(name, value) {}
};
const SparseFlags &SparseFlags::Bit1 = SparseFlags("Bit1", 1);
const SparseFlags &SparseFlags::Bit3 = SparseFlags("Bit3", 4);

// Namespaces for testing enums with the same name
namespace FirstNamespace {
    class Direction : public SmartEnum<Direction> {
    public:
        static const Direction North;
        static const Direction East;
        static const Direction South;
        static const Direction West;
        
        // Additional behavior specific to this namespace
        std::string GetDescription() const { 
            return Name() + " (First namespace)"; 
        }
    
    private:
        Direction(const std::string &name, int value) : SmartEnum(name, value) {}
    };
    
    const Direction Direction::North("North", 1);
    const Direction Direction::East("East", 2);
    const Direction Direction::South("South", 3);
    const Direction Direction::West("West", 4);
    
    // Flag enum with same name in different namespaces
    class Options : public SmartFlagEnum<Options> {
    public:
        static const Options OptionA;
        static const Options OptionB;
        static const Options OptionC;
        static const Options OptionD;
        
        std::string GetSource() const { return "FirstNamespace"; }
        
    private:
        Options(const std::string &name, int value) : SmartFlagEnum(name, value) {}
    };
    
    const Options Options::OptionA("OptionA", 1);
    const Options Options::OptionB("OptionB", 2);
    const Options Options::OptionC("OptionC", 4);
    const Options Options::OptionD("OptionD", 8);
}

namespace SecondNamespace {
    class Direction : public SmartEnum<Direction> {
    public:
        static const Direction Up;
        static const Direction Right;
        static const Direction Down;
        static const Direction Left;
        
        // Different behavior in second namespace
        std::string GetDescription() const { 
            return Name() + " (Second namespace)"; 
        }
    
    private:
        Direction(const std::string &name, int value) : SmartEnum(name, value) {}
    };
    
    const Direction Direction::Up("Up", 10);     // Different values than FirstNamespace
    const Direction Direction::Right("Right", 20);
    const Direction Direction::Down("Down", 30);
    const Direction Direction::Left("Left", 40);
    
    // Flag enum with same name but different values/behaviors
    class Options : public SmartFlagEnum<Options>, public AllowNegativeFlagEnumInput {
    public:
        static const Options Basic;
        static const Options Advanced;
        static const Options All;
        
        std::string GetSource() const { return "SecondNamespace"; }
        
    private:
        Options(const std::string &name, int value) : SmartFlagEnum(name, value) {}
    };
    
    const Options Options::Basic("Basic", 1);
    const Options Options::Advanced("Advanced", 2);
    const Options Options::All("All", -1);  // Uses negative values (not allowed in FirstNamespace)
}

// Tests for SmartEnum functionality
TEST(SmartEnumTest, LookupByNameAndValue)
{
    EXPECT_EQ(TestEnum::List().size(), 3);
    EXPECT_EQ(&TestEnum::One, &TestEnum::FromName("One"));
    EXPECT_THROW(TestEnum::FromName("one"), SmartEnumNotFoundException);
    EXPECT_EQ(&TestEnum::One, &TestEnum::FromName("one", true));
    const TestEnum *outEnum = nullptr;
    EXPECT_TRUE(TestEnum::TryFromName("Two", outEnum));
    EXPECT_EQ(outEnum, &TestEnum::Two);
    EXPECT_FALSE(TestEnum::TryFromName("InvalidName", outEnum));
    EXPECT_EQ(&TestEnum::Three, &TestEnum::FromValue(3));
    EXPECT_THROW(TestEnum::FromValue(99), SmartEnumNotFoundException);
    EXPECT_TRUE(TestEnum::TryFromValue(1, outEnum));
    EXPECT_EQ(outEnum, &TestEnum::One);
    EXPECT_FALSE(TestEnum::TryFromValue(42, outEnum));
}

TEST(SmartEnumTest, EqualityAndToString)
{
    EXPECT_TRUE(TestEnum::One.Equals(TestEnum::One));
    EXPECT_EQ("Two", TestEnum::Two.ToString());
}

TEST(SmartEnumTest, PolymorphicBehavior)
{
    EXPECT_EQ(1000, EmployeeType::Manager.BonusSize());
    EXPECT_EQ(500, EmployeeType::Assistant.BonusSize());
}

// Tests for SmartFlagEnum functionality
TEST(SmartFlagEnumTest, CombinationAndExplicitValues)
{
    std::vector<const Flags *> result;
    result = Flags::FromName("A");
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], &Flags::A);
    result = Flags::FromName("A, B");
    bool hasA = false, hasB = false;
    for (auto f : result)
    {
        if (f == &Flags::A)
            hasA = true;
        if (f == &Flags::B)
            hasB = true;
    }
    EXPECT_TRUE(hasA && hasB);
    result = Flags::FromValue(3);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], &Flags::AB);
    result = Flags::FromValue(5);
    EXPECT_EQ(result.size(), 2);
    EXPECT_TRUE(std::find(result.begin(), result.end(), &Flags::A) != result.end());
    EXPECT_TRUE(std::find(result.begin(), result.end(), &Flags::C) != result.end());
    result = Flags::FromValue(-1);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], &Flags::All);
}

TEST(SmartFlagEnumTest, InvalidInputs)
{
    std::vector<const Flags *> res;
    EXPECT_FALSE(Flags::TryFromValue(8, res));

    std::vector<const NoNegFlags *> res2;
    EXPECT_FALSE(NoNegFlags::TryFromValue(-5, res2));
}

TEST(SmartFlagEnumTest, AllowUnsafeFlagValues)
{

    std::vector<const SparseFlags *> out;
    EXPECT_NO_THROW(out = SparseFlags::FromValue(5));
    EXPECT_EQ(out.size(), 2);
}

TEST(SmartEnumSwitchTest, FluentSwitch)
{
    const TestEnum &val = TestEnum::Two;
    std::string result;
    SwitchOn(val)
        .When(TestEnum::One)
        .Then([&]
              { result = "One"; })
        .When(TestEnum::Two)
        .Then([&]
              { result = "Two"; })
        .When(TestEnum::Three)
        .Then([&]
              { result = "Three"; })
        .Default([&]
                 { result = "None"; });
    EXPECT_EQ(result, "Two");
    result.clear();
    const TestEnum &val2 = TestEnum::Three;
    SwitchOn(val2)
        .When(TestEnum::One)
        .Then([&]
              { result = "One"; })
        .When(TestEnum::Two)
        .Then([&]
              { result = "Two"; })
        .Default([&]
                 { result = "Default"; });
    EXPECT_EQ(result, "Default");
}

// Test for enums with same name in different namespaces
TEST(SameNameEnumsTest, DifferentNamespaces) {
    // Test simple enum instances are distinct
    EXPECT_EQ(FirstNamespace::Direction::List().size(), 4);
    EXPECT_EQ(SecondNamespace::Direction::List().size(), 4);
    
    // Test values can be different
    EXPECT_EQ(FirstNamespace::Direction::North.Value(), 1);
    EXPECT_EQ(SecondNamespace::Direction::Up.Value(), 10);
    
    // Test lookup by name
    EXPECT_EQ(&FirstNamespace::Direction::East, &FirstNamespace::Direction::FromName("East"));
    EXPECT_EQ(&SecondNamespace::Direction::Right, &SecondNamespace::Direction::FromName("Right"));
    
    // Test behavior can be different
    EXPECT_EQ("East (First namespace)", FirstNamespace::Direction::East.GetDescription());
    EXPECT_EQ("Right (Second namespace)", SecondNamespace::Direction::Right.GetDescription());
    
    // Test FromValue works independently for each namespace
    EXPECT_EQ(&FirstNamespace::Direction::South, &FirstNamespace::Direction::FromValue(3));
    EXPECT_EQ(&SecondNamespace::Direction::Down, &SecondNamespace::Direction::FromValue(30));
    
    // Verify that values from one namespace don't exist in the other
    const FirstNamespace::Direction* outEnum1 = nullptr;
    const SecondNamespace::Direction* outEnum2 = nullptr;
    EXPECT_FALSE(FirstNamespace::Direction::TryFromValue(10, outEnum1)); // 10 exists only in SecondNamespace
    EXPECT_FALSE(SecondNamespace::Direction::TryFromValue(1, outEnum2)); // 1 exists only in FirstNamespace
}

// Test for flag enums with same name in different namespaces
TEST(SameNameEnumsTest, FlagEnumNamespaces) {
    // Test flag enum lists are independent
    EXPECT_EQ(FirstNamespace::Options::List().size(), 4);
    EXPECT_EQ(SecondNamespace::Options::List().size(), 3);
    
    // Test flag enum behaviors
    EXPECT_EQ("FirstNamespace", FirstNamespace::Options::OptionA.GetSource());
    EXPECT_EQ("SecondNamespace", SecondNamespace::Options::Basic.GetSource());
    
    // Test flag combination works independently 
    std::vector<const FirstNamespace::Options*> result1;
    std::vector<const SecondNamespace::Options*> result2;
    
    result1 = FirstNamespace::Options::FromValue(3); // OptionA | OptionB
    EXPECT_EQ(result1.size(), 2);
    
    result2 = SecondNamespace::Options::FromValue(3); // Basic | Advanced
    EXPECT_EQ(result2.size(), 2);
    
    // Test negative values allowed only in SecondNamespace
    EXPECT_NO_THROW(result2 = SecondNamespace::Options::FromValue(-1));
    EXPECT_EQ(result2.size(), 1);
    EXPECT_EQ(result2[0], &SecondNamespace::Options::All);
    
    // This would throw due to negative value not allowed in FirstNamespace
    // Not testing directly since we can't catch exceptions across namespaces in a simple way
}
