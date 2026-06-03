#pragma once

#include <string>

enum class MembershipLevel {
    NORMAL = 0,
    SILVER = 1,
    GOLD   = 2
};

class Customer {
private:
    std::string     id;
    std::string     name;
    int             age;
    std::string     phone;
    MembershipLevel membershipLevel;

public:
    Customer(const std::string& id, const std::string& name, int age,
             const std::string& phone,
             MembershipLevel level = MembershipLevel::NORMAL);

    // Getters
    std::string     getId()              const;
    std::string     getName()            const;
    int             getAge()             const;
    std::string     getPhone()           const;
    MembershipLevel getMembershipLevel() const;
    std::string     getMembershipString() const;

    void display() const;

    static MembershipLevel parseMembership(const std::string& text);
};
