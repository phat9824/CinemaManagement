#pragma once

#include <string>

class Seat {
private:
    std::string id;

public:
    explicit Seat(const std::string& id);

    std::string getId() const;
    
    // Validates basic format like "A1", "C5"
    static bool isValidFormat(const std::string& seatId);
};
