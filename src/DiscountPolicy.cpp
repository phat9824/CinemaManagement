#include "DiscountPolicy.hpp"

#include <stdexcept>

// ============================================================
// PercentageDiscount
// ============================================================

PercentageDiscount::PercentageDiscount(const std::string& code, double percentage)
    : code(code), percentage(percentage) {}

double PercentageDiscount::applyDiscount(double currentPrice,
                                          const Customer& /*customer*/) const {
    return currentPrice * (1.0 - percentage);
}

std::string PercentageDiscount::getCode() const { return code; }

std::string PercentageDiscount::getDescription() const {
    return std::to_string(static_cast<int>(percentage * 100)) + "% off";
}

// ============================================================
// FixedAmountDiscount
// ============================================================

FixedAmountDiscount::FixedAmountDiscount(const std::string& code, double amount)
    : code(code), amount(amount) {}

double FixedAmountDiscount::applyDiscount(double currentPrice,
                                           const Customer& /*customer*/) const {
    double result = currentPrice - amount;
    return (result < 0.0) ? 0.0 : result;
}

std::string FixedAmountDiscount::getCode() const { return code; }

std::string FixedAmountDiscount::getDescription() const {
    return std::to_string(static_cast<long long>(amount)) + " VND off";
}

// ============================================================
// MembershipDiscount
// ============================================================

MembershipDiscount::MembershipDiscount(const std::string& code, double percentage,
                                        MembershipLevel requiredLevel)
    : code(code), percentage(percentage), requiredLevel(requiredLevel) {}

double MembershipDiscount::applyDiscount(double currentPrice,
                                          const Customer& customer) const {
    if (static_cast<int>(customer.getMembershipLevel()) <
        static_cast<int>(requiredLevel)) {
        throw std::runtime_error(
            "Customer membership level is insufficient for discount code: " + code);
    }
    return currentPrice * (1.0 - percentage);
}

std::string MembershipDiscount::getCode() const { return code; }

std::string MembershipDiscount::getDescription() const {
    return std::to_string(static_cast<int>(percentage * 100))
           + "% off (membership required)";
}

MembershipLevel MembershipDiscount::getRequiredLevel() const { return requiredLevel; }
