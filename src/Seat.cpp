#include "Seat.hpp"
#include <cctype>

Seat::Seat(const std::string& id) : id(id) {}

std::string Seat::getId() const {
    return id;
}

bool Seat::isValidFormat(const std::string& seatId) {
    if (seatId.length() < 2 || seatId.length() > 3) return false;
    
    char row = seatId[0];
    if (!std::isalpha(row)) return false;
    
    for (size_t i = 1; i < seatId.length(); ++i) {
        if (!std::isdigit(seatId[i])) return false;
    }
    
    return true;
}
