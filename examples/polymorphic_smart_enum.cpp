#include <SmartEnumCpp/SmartEnum.hpp>
#include <iostream>
#include <iomanip>

// Define a polymorphic SmartEnum with custom behavior
class PaymentMethod : public SmartEnum<PaymentMethod> {
public:
    static const PaymentMethod CreditCard;
    static const PaymentMethod DebitCard;
    static const PaymentMethod Cash;
    static const PaymentMethod Check;
    
    // Custom behavior methods
    virtual float CalculateProcessingFee(float amount) const = 0;
    virtual int GetProcessingDays() const = 0;
    
protected:
    // Protected constructor for derived classes
    PaymentMethod(const std::string& name, int value) : SmartEnum(name, value) {}
};

// Implement specific behavior for each payment method
class CreditCardPayment : public PaymentMethod {
public:
    CreditCardPayment() : PaymentMethod("CreditCard", 1) {}
    
    float CalculateProcessingFee(float amount) const override {
        return amount * 0.03f; // 3% fee
    }
    
    int GetProcessingDays() const override {
        return 1; // Processes in 1 day
    }
};

class DebitCardPayment : public PaymentMethod {
public:
    DebitCardPayment() : PaymentMethod("DebitCard", 2) {}
    
    float CalculateProcessingFee(float amount) const override {
        return amount * 0.01f; // 1% fee
    }
    
    int GetProcessingDays() const override {
        return 1; // Processes in 1 day
    }
};

class CashPayment : public PaymentMethod {
public:
    CashPayment() : PaymentMethod("Cash", 3) {}
    
    float CalculateProcessingFee(float amount) const override {
        return 0.0f; // No fee
    }
    
    int GetProcessingDays() const override {
        return 0; // Immediate processing
    }
};

class CheckPayment : public PaymentMethod {
public:
    CheckPayment() : PaymentMethod("Check", 4) {}
    
    float CalculateProcessingFee(float amount) const override {
        return 1.0f; // Fixed $1 fee
    }
    
    int GetProcessingDays() const override {
        return 5; // 5 business days to clear
    }
};

// Initialize the payment method constants
const PaymentMethod PaymentMethod::CreditCard = CreditCardPayment();
const PaymentMethod PaymentMethod::DebitCard = DebitCardPayment();
const PaymentMethod PaymentMethod::Cash = CashPayment();
const PaymentMethod PaymentMethod::Check = CheckPayment();

int main() {
    float purchaseAmount = 100.0f;
    
    std::cout << std::fixed << std::setprecision(2); // Format output
    std::cout << "Purchase amount: $" << purchaseAmount << std::endl << std::endl;
    
    std::cout << "Payment Method | Processing Fee | Processing Days" << std::endl;
    std::cout << "---------------|---------------|----------------" << std::endl;
    
    for (const PaymentMethod* method : PaymentMethod::List()) {
        float fee = method->CalculateProcessingFee(purchaseAmount);
        int days = method->GetProcessingDays();
        
        std::cout << std::left << std::setw(15) << method->Name() 
                  << "| $" << std::right << std::setw(12) << fee 
                  << " | " << std::setw(15) << days << std::endl;
    }
    
    // Use lookup to find a specific payment method
    const PaymentMethod& selectedMethod = PaymentMethod::FromName("Check");
    std::cout << "\nSelected payment method: " << selectedMethod.Name() << std::endl;
    std::cout << "Fee: $" << selectedMethod.CalculateProcessingFee(purchaseAmount) << std::endl;
    std::cout << "Processing time: " << selectedMethod.GetProcessingDays() << " days" << std::endl;
    
    return 0;
}