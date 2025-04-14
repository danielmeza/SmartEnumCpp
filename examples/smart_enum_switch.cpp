#include <SmartEnumCpp/SmartEnum.hpp>
#include <SmartEnumCpp/SmartEnumSwitch.hpp>
#include <iostream>
#include <string>

// Define an OrderStatus SmartEnum
class OrderStatus : public SmartEnum<OrderStatus> {
public:
    static const OrderStatus Created;
    static const OrderStatus Paid;
    static const OrderStatus Processing;
    static const OrderStatus Shipped;
    static const OrderStatus Delivered;
    static const OrderStatus Canceled;
    
private:
    OrderStatus(const std::string& name, int value) : SmartEnum(name, value) {}
};

// Initialize the order status constants
const OrderStatus OrderStatus::Created("Created", 1);
const OrderStatus OrderStatus::Paid("Paid", 2);
const OrderStatus OrderStatus::Processing("Processing", 3);
const OrderStatus OrderStatus::Shipped("Shipped", 4);
const OrderStatus OrderStatus::Delivered("Delivered", 5);
const OrderStatus OrderStatus::Canceled("Canceled", 6);

// A class representing an order
class Order {
public:
    Order(int id, const OrderStatus& status) : id_(id), status_(status) {}
    
    int GetId() const { return id_; }
    const OrderStatus& GetStatus() const { return status_; }
    void SetStatus(const OrderStatus& status) { status_ = status; }
    
    // Process the order based on its status
    void Process() {
        std::cout << "Processing Order #" << id_ << " (Status: " << status_.Name() << ")..." << std::endl;
        
        // Using the fluent switch interface
        SwitchOn(status_)
            .When(OrderStatus::Created).Then([this]() {
                std::cout << " - New order created, awaiting payment" << std::endl;
                std::cout << " - Sending confirmation email to customer" << std::endl;
            })
            .When(OrderStatus::Paid).Then([this]() {
                std::cout << " - Payment received, preparing to fulfill order" << std::endl;
                std::cout << " - Moving to processing queue" << std::endl;
                this->SetStatus(OrderStatus::Processing);
            })
            .When(OrderStatus::Processing).Then([this]() {
                std::cout << " - Picking items from warehouse" << std::endl;
                std::cout << " - Packaging items" << std::endl;
                std::cout << " - Order ready for shipping" << std::endl;
                this->SetStatus(OrderStatus::Shipped);
            })
            .When(OrderStatus::Shipped).Then([this]() {
                std::cout << " - Order has been shipped" << std::endl;
                std::cout << " - Tracking information sent to customer" << std::endl;
            })
            .When(OrderStatus::Delivered).Then([this]() {
                std::cout << " - Order successfully delivered" << std::endl;
                std::cout << " - Requesting customer feedback" << std::endl;
            })
            .When(OrderStatus::Canceled).Then([this]() {
                std::cout << " - Order was canceled" << std::endl;
                std::cout << " - Processing refund if applicable" << std::endl;
            })
            .Default([]() {
                std::cout << " - Unknown status, review required" << std::endl;
            });
            
        std::cout << "Processing complete. Current status: " << status_.Name() << std::endl << std::endl;
    }
    
private:
    int id_;
    OrderStatus status_;
};

// Function demonstrating the use of SwitchOn with a return value
std::string GetOrderStatusDescription(const OrderStatus& status) {
    std::string result;
    
    // Build a string description based on status
    SwitchOn(status)
        .When(OrderStatus::Created).Then([&result]() {
            result = "Order has been created but not yet paid";
        })
        .When(OrderStatus::Paid).Then([&result]() {
            result = "Payment received, awaiting processing";
        })
        .When(OrderStatus::Processing).Then([&result]() {
            result = "Order is being prepared for shipping";
        })
        .When(OrderStatus::Shipped).Then([&result]() {
            result = "Order has been shipped and is in transit";
        })
        .When(OrderStatus::Delivered).Then([&result]() {
            result = "Order has been successfully delivered";
        })
        .When(OrderStatus::Canceled).Then([&result]() {
            result = "Order was canceled";
        })
        .Default([&result]() {
            result = "Unknown order status";
        });
    
    return result;
}

int main() {
    std::cout << "==== SmartEnumSwitch Example ====\n" << std::endl;
    
    // Show all possible order statuses
    std::cout << "Available Order Statuses:" << std::endl;
    for (const OrderStatus* status : OrderStatus::List()) {
        std::cout << " - " << status->Name() << ": " << GetOrderStatusDescription(*status) << std::endl;
    }
    std::cout << std::endl;
    
    // Create an order
    Order order1(1001, OrderStatus::Created);
    order1.Process();
    
    // Update and process the order
    order1.SetStatus(OrderStatus::Paid);
    order1.Process();
    
    // The Process method will automatically move this to Shipped
    order1.Process();
    
    // Create another order that gets canceled
    Order order2(1002, OrderStatus::Created);
    order2.Process();
    
    order2.SetStatus(OrderStatus::Canceled);
    order2.Process();
    
    return 0;
}