# SmartEnumSwitch

## Overview

`SmartEnumSwitch` provides a fluent interface for handling SmartEnum values in a switch-like pattern:

- **Chainable API**: Expressive When().Then() syntax
- **Default cases**: Handles unmatched cases with Default()
- **Type safety**: Compile-time checking of enum types
- **Clean syntax**: Avoids verbose switch statements
- **Lambda support**: Use lambdas for each case

## Before and After

### Before (Using Traditional Switch)

```cpp
// Traditional switch with enums
void processStatus(Status status) {
    switch (status) {
        case Status::Pending:
            std::cout << "Status is pending" << std::endl;
            break;
        case Status::Active:
            std::cout << "Status is active" << std::endl;
            break;
        case Status::Suspended:
            std::cout << "Status is suspended" << std::endl;
            break;
        default:
            std::cout << "Status is unknown" << std::endl;
            break;
    }
}

// Problems:
// 1. Easy to forget 'break' statements
// 2. Limited to simple value comparisons
// 3. Switch only works with integral types, not custom objects
// 4. Requires separate enum-to-value conversion for custom enums
```

### After (Using SmartEnumSwitch)

```cpp
#include <SmartEnumCpp/SmartEnum.hpp>
#include <SmartEnumCpp/SmartEnumSwitch.hpp>

class Status : public SmartEnum<Status> {
public:
    static const Status Pending;
    static const Status Active;
    static const Status Suspended;
    static const Status Closed;
private:
    Status(const std::string& name, int value) : SmartEnum(name, value) {}
};

void processStatus(const Status& status) {
    SwitchOn(status)
        .When(Status::Pending).Then([]() {
            std::cout << "Status is pending" << std::endl;
        })
        .When(Status::Active).Then([]() {
            std::cout << "Status is active" << std::endl;
        })
        .When(Status::Suspended).Then([]() {
            std::cout << "Status is suspended" << std::endl;
        })
        .Default([]() {
            std::cout << "Status is unknown" << std::endl;
        });
}

// Benefits:
// 1. No missing breaks
// 2. Fluent, expressive API
// 3. Works with SmartEnum objects directly
// 4. Type-safe with compile-time checking
```

## Basic Usage

### Creating and Using SmartEnumSwitch

```cpp
#include <SmartEnumCpp/SmartEnum.hpp>
#include <SmartEnumCpp/SmartEnumSwitch.hpp>

// 1. Define your SmartEnum
class OrderStatus : public SmartEnum<OrderStatus> {
public:
    static const OrderStatus Created;
    static const OrderStatus Paid;
    static const OrderStatus Shipped;
    static const OrderStatus Delivered;
    
private:
    OrderStatus(const std::string& name, int value) : SmartEnum(name, value) {}
};

const OrderStatus OrderStatus::Created("Created", 1);
const OrderStatus OrderStatus::Paid("Paid", 2);
const OrderStatus OrderStatus::Shipped("Shipped", 3);
const OrderStatus OrderStatus::Delivered("Delivered", 4);

// 2. Use SwitchOn with the enum value
void processOrder(const OrderStatus& status) {
    SwitchOn(status)
        .When(OrderStatus::Created).Then([]() {
            std::cout << "New order created" << std::endl;
        })
        .When(OrderStatus::Paid).Then([]() {
            std::cout << "Payment received" << std::endl;
        })
        .When(OrderStatus::Shipped).Then([]() {
            std::cout << "Order shipped" << std::endl;
        })
        .When(OrderStatus::Delivered).Then([]() {
            std::cout << "Order delivered" << std::endl;
        })
        .Default([]() {
            std::cout << "Unknown status" << std::endl;
        });
}
```

### Using Lambdas with State

You can use lambdas that capture variables:

```cpp
bool sendNotification = true;

SwitchOn(status)
    .When(OrderStatus::Created).Then([&sendNotification]() {
        // Do something with created order
        if (sendNotification) {
            // Send notification
        }
    })
    .When(OrderStatus::Paid).Then([&sendNotification]() {
        // Process payment
        if (sendNotification) {
            // Send payment confirmation
        }
    });
```

### With Return Values

You can use the switch pattern to return values:

```cpp
std::string getStatusDescription(const OrderStatus& status) {
    std::string result;
    
    SwitchOn(status)
        .When(OrderStatus::Created).Then([&result]() {
            result = "Order has been created but not yet paid";
        })
        .When(OrderStatus::Paid).Then([&result]() {
            result = "Payment has been processed, awaiting shipment";
        })
        .When(OrderStatus::Shipped).Then([&result]() {
            result = "Order has been shipped to the customer";
        })
        .When(OrderStatus::Delivered).Then([&result]() {
            result = "Order has been delivered successfully";
        })
        .Default([&result]() {
            result = "Unknown status";
        });
        
    return result;
}
```

## Advanced Features

### State Modification in Switch Cases

You can modify state within the lambda functions:

```cpp
void processOrderWithStateChanges(Order& order) {
    SwitchOn(order.GetStatus())
        .When(OrderStatus::Created).Then([&order]() {
            // Validate and process payment
            order.SetStatus(OrderStatus::Paid);
        })
        .When(OrderStatus::Paid).Then([&order]() {
            // Prepare shipment
            order.SetStatus(OrderStatus::Shipped);
        });
}
```

### Complex Conditions

You can combine multiple conditions:

```cpp
bool isPremiumCustomer = true;

SwitchOn(status)
    .When(OrderStatus::Created).Then([&]() {
        if (isPremiumCustomer) {
            // Priority processing
        } else {
            // Standard processing
        }
    });
```