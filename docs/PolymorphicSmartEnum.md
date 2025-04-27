# Polymorphic SmartEnum

## Overview

Polymorphic SmartEnums extend the basic SmartEnum to provide instance-specific behavior through inheritance:

- **Custom behavior per instance**: Different enum values can have different implementations
- **Virtual methods**: Define abstract methods that each enum instance implements differently
- **Maintains SmartEnum benefits**: All the lookup and type-safety features of SmartEnum
- **Full OOP capabilities**: Leverage the full power of C++ polymorphism

## Before and After

### Before (Workarounds with Traditional Enums)

```cpp
// Traditional approach with switch statements
enum PaymentType {
    CREDIT_CARD = 1,
    DEBIT_CARD = 2,
    CASH = 3,
    CHECK = 4
};

// Have to use switch statements everywhere
float calculateFee(PaymentType type, float amount) {
    switch(type) {
        case CREDIT_CARD: return amount * 0.03f;
        case DEBIT_CARD: return amount * 0.01f;
        case CASH: return 0.0f;
        case CHECK: return 1.0f;
        default: throw std::runtime_error("Unknown payment type");
    }
}

int getProcessingDays(PaymentType type) {
    switch(type) {
        case CREDIT_CARD: return 1;
        case DEBIT_CARD: return 1;
        case CASH: return 0;
        case CHECK: return 5;
        default: throw std::runtime_error("Unknown payment type");
    }
}

// Problems:
// 1. Behavior is separated from the enum values
// 2. Switch statements needed everywhere the behavior is used
// 3. Adding a new enum value requires updating all switch statements
// 4. Difficult to maintain as the number of behaviors increases
```

### After (Using Polymorphic SmartEnum)

```cpp
#include <SmartEnumCpp/SmartEnum.hpp>

class PaymentMethod : public SmartEnum<PaymentMethod> {
public:
    static const PaymentMethod CreditCard;
    static const PaymentMethod DebitCard;
    static const PaymentMethod Cash;
    static const PaymentMethod Check;
    
    // Behavior defined right on the enum
    virtual float calculateFee(float amount) const = 0;
    virtual int getProcessingDays() const = 0;
    
protected:
    PaymentMethod(const std::string& name, int value) : SmartEnum(name, value) {}
};

// Each implementation defines its own behavior
class CreditCardPayment : public PaymentMethod {
public:
    CreditCardPayment() : PaymentMethod("CreditCard", 1) {}
    
    float calculateFee(float amount) const override {
        return amount * 0.03f; // 3% fee
    }
    
    int getProcessingDays() const override {
        return 1; // 1 day processing
    }
};

// Initialize the constants with specific implementations
const PaymentMethod PaymentMethod::CreditCard = CreditCardPayment();
const PaymentMethod PaymentMethod::DebitCard = DebitCardPayment();
const PaymentMethod PaymentMethod::Cash = CashPayment();
const PaymentMethod PaymentMethod::Check = CheckPayment();

// Benefits:
// 1. Behavior is encapsulated with each enum value
// 2. No switch statements needed
// 3. Adding a new value only requires adding its implementation
// 4. Full OOP inheritance and polymorphism capabilities
```

## Basic Usage

### Creating a Polymorphic SmartEnum

```cpp
#include <SmartEnumCpp/SmartEnum.hpp>

// 1. Define the base class with virtual methods
class PaymentMethod : public SmartEnum<PaymentMethod> {
public:
    static const PaymentMethod CreditCard;
    static const PaymentMethod DebitCard;
    static const PaymentMethod Cash;
    static const PaymentMethod Check;
    
    // Define abstract behaviors
    virtual float calculateFee(float amount) const = 0;
    virtual int getProcessingDays() const = 0;
    
protected:
    // Protected constructor for derived classes
    PaymentMethod(const std::string& name, int value) : SmartEnum(name, value) {}
};

// 2. Create derived classes for each enum value
class CreditCardPayment : public PaymentMethod {
public:
    CreditCardPayment() : PaymentMethod("CreditCard", 1) {}
    
    float calculateFee(float amount) const override {
        return amount * 0.03f; // 3% fee
    }
    
    int getProcessingDays() const override {
        return 1; // 1 day processing
    }
};

class DebitCardPayment : public PaymentMethod {
public:
    DebitCardPayment() : PaymentMethod("DebitCard", 2) {}
    
    float calculateFee(float amount) const override {
        return amount * 0.01f; // 1% fee
    }
    
    int getProcessingDays() const override {
        return 1; // 1 day processing
    }
};

class CashPayment : public PaymentMethod {
public:
    CashPayment() : PaymentMethod("Cash", 3) {}
    
    float calculateFee(float amount) const override {
        return 0.0f; // No fee
    }
    
    int getProcessingDays() const override {
        return 0; // Immediate processing
    }
};

class CheckPayment : public PaymentMethod {
public:
    CheckPayment() : PaymentMethod("Check", 4) {}
    
    float calculateFee(float amount) const override {
        return 1.0f; // Fixed $1 fee
    }
    
    int getProcessingDays() const override {
        return 5; // 5 business days to clear
    }
};

// 3. Initialize the constants with specific implementations
const PaymentMethod PaymentMethod::CreditCard = CreditCardPayment();
const PaymentMethod PaymentMethod::DebitCard = DebitCardPayment();
const PaymentMethod PaymentMethod::Cash = CashPayment();
const PaymentMethod PaymentMethod::Check = CheckPayment();
```

### Using Polymorphic SmartEnum Values

```cpp
// Call the polymorphic methods directly on the enum values
float creditCardFee = PaymentMethod::CreditCard.calculateFee(100.0f);  // 3.0
float cashFee = PaymentMethod::Cash.calculateFee(100.0f);  // 0.0

// Use with standard SmartEnum features
const PaymentMethod& method = PaymentMethod::FromName("Check");
float checkFee = method.calculateFee(100.0f);  // 1.0
int processingTime = method.getProcessingDays();  // 5

// Iterate through all methods
for (const PaymentMethod* method : PaymentMethod::List()) {
    std::cout << method->Name() << ": "
              << "Fee: $" << method->calculateFee(100.0f)
              << ", Processing days: " << method->getProcessingDays()
              << std::endl;
}
```

## Advanced Features

### Multiple Inheritance

You can combine polymorphic behavior with other interfaces:

```cpp
class ISecurityCheck {
public:
    virtual bool requiresVerification() const = 0;
};

class PaymentMethod : public SmartEnum<PaymentMethod> {
    // Base SmartEnum functionality
};

class CreditCardPayment : public PaymentMethod, public ISecurityCheck {
public:
    bool requiresVerification() const override {
        return true;
    }
};
```

### Adding Internal State

Polymorphic enum instances can maintain internal state:

```cpp
class ConfigurablePayment : public PaymentMethod {
private:
    float feePercentage;
    
public:
    ConfigurablePayment(const std::string& name, int value, float feePercent)
        : PaymentMethod(name, value), feePercentage(feePercent) {}
        
    float calculateFee(float amount) const override {
        return amount * feePercentage;
    }
};

// Create with different configurations
const PaymentMethod CustomPayment = ConfigurablePayment("Custom", 10, 0.05f);
```