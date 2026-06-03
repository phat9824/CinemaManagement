#include "Customer.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>

Customer::Customer(const std::string& id, const std::string& name, int age,
                   const std::string& phone, MembershipLevel level)
    : id(id), name(name), age(age), phone(phone), membershipLevel(level) {}

std::string     Customer::getId()              const { return id; }
std::string     Customer::getName()            const { return name; }
int             Customer::getAge()             const { return age; }
std::string     Customer::getPhone()           const { return phone; }
MembershipLevel Customer::getMembershipLevel() const { return membershipLevel; }

std::string Customer::getMembershipString() const {
    switch (membershipLevel) {
        case MembershipLevel::SILVER: return "Silver";
        case MembershipLevel::GOLD:   return "Gold";
        default:                      return "Normal";
    }
}

void Customer::display() const {
    std::cout << "[" << id << "] " << name
              << " | Age: "    << age
              << " | Phone: "  << phone
              << " | Member: " << getMembershipString() << "\n";
}

MembershipLevel Customer::parseMembership(const std::string& text) {
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (lower == "silver") return MembershipLevel::SILVER;
    if (lower == "gold")   return MembershipLevel::GOLD;
    return MembershipLevel::NORMAL;
}
