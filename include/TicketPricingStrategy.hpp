#pragma once

#include <memory>
#include <string>

/**
 * Abstract strategy for ticket pricing.
 * Demonstrates OOP polymorphism requirement.
 */
class TicketPricingStrategy {
public:
    virtual double      calculatePrice(double basePrice) const = 0;
    virtual std::string getName()                        const = 0;
    virtual ~TicketPricingStrategy() = default;
};

/** Adult: full base price (no discount) */
class AdultPricing : public TicketPricingStrategy {
public:
    double      calculatePrice(double basePrice) const override { return basePrice; }
    std::string getName()                        const override { return "Adult"; }
};

/** Child: 30% discount -> pays 70% of base price */
class ChildPricing : public TicketPricingStrategy {
public:
    double      calculatePrice(double basePrice) const override { return basePrice * 0.7; }
    std::string getName()                        const override { return "Child"; }
};

/** Student: 20% discount -> pays 80% of base price */
class StudentPricing : public TicketPricingStrategy {
public:
    double      calculatePrice(double basePrice) const override { return basePrice * 0.8; }
    std::string getName()                        const override { return "Student"; }
};

/** Factory: create a pricing strategy by ticket type name */
inline std::unique_ptr<TicketPricingStrategy>
createPricingStrategy(const std::string& typeName) {
    if (typeName == "Child")   return std::make_unique<ChildPricing>();
    if (typeName == "Student") return std::make_unique<StudentPricing>();
    return std::make_unique<AdultPricing>();
}
