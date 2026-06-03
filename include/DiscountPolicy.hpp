#pragma once

#include "Customer.hpp"

#include <memory>
#include <string>

/**
 * Abstract discount policy.
 * Demonstrates OOP polymorphism requirement.
 */
class DiscountPolicy {
public:
    virtual double      applyDiscount(double currentPrice, const Customer& customer) const = 0;
    virtual std::string getCode()        const = 0;
    virtual std::string getDescription() const = 0;
    virtual ~DiscountPolicy() = default;
};

/** Percentage off — e.g. SALE10 -> 10% off */
class PercentageDiscount : public DiscountPolicy {
private:
    std::string code;
    double      percentage; // 0.10 = 10%

public:
    PercentageDiscount(const std::string& code, double percentage);
    double      applyDiscount(double currentPrice, const Customer& customer) const override;
    std::string getCode()        const override;
    std::string getDescription() const override;
};

/** Fixed VND amount off — e.g. FIX20K -> 20,000 VND off */
class FixedAmountDiscount : public DiscountPolicy {
private:
    std::string code;
    double      amount; // VND

public:
    FixedAmountDiscount(const std::string& code, double amount);
    double      applyDiscount(double currentPrice, const Customer& customer) const override;
    std::string getCode()        const override;
    std::string getDescription() const override;
};

/** Membership-based discount — e.g. GOLD -> 15% off for Gold members */
class MembershipDiscount : public DiscountPolicy {
private:
    std::string     code;
    double          percentage;
    MembershipLevel requiredLevel;

public:
    MembershipDiscount(const std::string& code, double percentage, MembershipLevel requiredLevel);
    double          applyDiscount(double currentPrice, const Customer& customer) const override;
    std::string     getCode()          const override;
    std::string     getDescription()   const override;
    MembershipLevel getRequiredLevel() const;
};
