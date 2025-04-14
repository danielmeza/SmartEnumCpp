#include <gtest/gtest.h>
#include "SmartEnum.hpp"
#include "SmartFlagEnum.hpp"
#include "SmartEnumSwitch.hpp"

// Define a simple TestEnum for testing
class TestEnum : public SmartEnum<TestEnum> {
public:
    static const TestEnum One;
    static const TestEnum Two;
    static const TestEnum Three;
private:
    TestEnum(const std::string& name, int value) : SmartEnum(name, value) {}
};
const TestEnum TestEnum::One("One", 1);
const TestEnum TestEnum::Two("Two", 2);
const TestEnum TestEnum::Three("Three", 3);

// Define polymorphic enum example (EmployeeType)
class EmployeeType : public SmartEnum<EmployeeType> {
public:
    static const EmployeeType Manager;
    static const EmployeeType Assistant;
    virtual int BonusSize() const = 0;
protected:
    EmployeeType(const std::string& name, int value) : SmartEnum(name, value) {}
};
class ManagerType : public EmployeeType {
public:
    ManagerType() : EmployeeType("Manager", 1) {}
    int BonusSize() const override { return 1000; }
};
class AssistantType : public EmployeeType {
public:
    AssistantType() : EmployeeType("Assistant", 2) {}
    int BonusSize() const override { return 500; }
};
const EmployeeType EmployeeType::Manager = ManagerType();
const EmployeeType EmployeeType::Assistant = AssistantType();

// Define a Flags enum for testing SmartFlagEnum
class Flags : public SmartFlagEnum<Flags>, public AllowNegativeFlagEnumInput {
public:
    static const Flags None;
    static const Flags A;
    static const Flags B;
    static const Flags C;
    static const Flags AB;  // explicit combination (A|B)
    static const Flags All;
private:
    Flags(const std::string& name, int value) : SmartFlagEnum(name, value) {}
};
const Flags Flags::None("None", 0);
const Flags Flags::A("A", 1);
const Flags Flags::B("B", 2);
const Flags Flags::C("C", 4);
const Flags Flags::AB("AB", 3);
const Flags Flags::All("All", -1);

// Tests for SmartEnum functionality
TEST(SmartEnumTest, LookupByNameAndValue) {
    EXPECT_EQ(TestEnum::List().size(), 3);
    EXPECT_EQ(&TestEnum::One, &TestEnum::FromName("One"));
    EXPECT_THROW(TestEnum::FromName("one"), SmartEnumNotFoundException);
    EXPECT_EQ(&TestEnum::One, &TestEnum::FromName("one", true));
    const TestEnum* outEnum = nullptr;
    EXPECT_TRUE(TestEnum::TryFromName("Two", outEnum));
    EXPECT_EQ(outEnum, &TestEnum::Two);
    EXPECT_FALSE(TestEnum::TryFromName("InvalidName", outEnum));
    EXPECT_EQ(&TestEnum::Three, &TestEnum::FromValue(3));
    EXPECT_THROW(TestEnum::FromValue(99), SmartEnumNotFoundException);
    EXPECT_TRUE(TestEnum::TryFromValue(1, outEnum));
    EXPECT_EQ(outEnum, &TestEnum::One);
    EXPECT_FALSE(TestEnum::TryFromValue(42, outEnum));
}

TEST(SmartEnumTest, EqualityAndToString) {
    EXPECT_TRUE(TestEnum::One.Equals(TestEnum::One));
    EXPECT_EQ("Two", TestEnum::Two.ToString());
}

TEST(SmartEnumTest, PolymorphicBehavior) {
    EXPECT_EQ(1000, EmployeeType::Manager.BonusSize());
    EXPECT_EQ(500,  EmployeeType::Assistant.BonusSize());
}

// Tests for SmartFlagEnum functionality
TEST(SmartFlagEnumTest, CombinationAndExplicitValues) {
    std::vector<const Flags*> result;
    result = Flags::FromName("A");
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], &Flags::A);
    result = Flags::FromName("A, B");
    bool hasA = false, hasB = false;
    for(auto f: result) {
        if(f == &Flags::A) hasA = true;
        if(f == &Flags::B) hasB = true;
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

TEST(SmartFlagEnumTest, InvalidInputs) {
    std::vector<const Flags*> res;
    EXPECT_FALSE(Flags::TryFromValue(8, res));
    class NoNegFlags : public SmartFlagEnum<NoNegFlags> {
    public:
        static const NoNegFlags X;
        static const NoNegFlags Y;
    private:
        NoNegFlags(const std::string& n, int v): SmartFlagEnum(n,v) {}
    };
    const NoNegFlags NoNegFlags::X("X",1);
    const NoNegFlags NoNegFlags::Y("Y",2);
    std::vector<const NoNegFlags*> res2;
    EXPECT_FALSE(NoNegFlags::TryFromValue(-5, res2));
}

TEST(SmartFlagEnumTest, AllowUnsafeFlagValues) {
    class SparseFlags : public SmartFlagEnum<SparseFlags>, public AllowUnsafeFlagEnumValues {
    public:
        static const SparseFlags Bit1;
        static const SparseFlags Bit3;
    private:
        SparseFlags(const std::string& name, int value): SmartFlagEnum(name, value) {}
    };
    const SparseFlags SparseFlags::Bit1("Bit1", 1);
    const SparseFlags SparseFlags::Bit3("Bit3", 4);
    std::vector<const SparseFlags*> out;
    EXPECT_NO_THROW(out = SparseFlags::FromValue(5));
    EXPECT_EQ(out.size(), 2);
}

TEST(SmartEnumSwitchTest, FluentSwitch) {
    const TestEnum& val = TestEnum::Two;
    std::string result;
    SwitchOn(val)
      .When(TestEnum::One).Then([&]{ result = "One"; })
      .When(TestEnum::Two).Then([&]{ result = "Two"; })
      .When(TestEnum::Three).Then([&]{ result = "Three"; })
      .Default([&]{ result = "None"; });
    EXPECT_EQ(result, "Two");
    result.clear();
    const TestEnum& val2 = TestEnum::Three;
    SwitchOn(val2)
      .When(TestEnum::One).Then([&]{ result = "One"; })
      .When(TestEnum::Two).Then([&]{ result = "Two"; })
      .Default([&]{ result = "Default"; });
    EXPECT_EQ(result, "Default");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
