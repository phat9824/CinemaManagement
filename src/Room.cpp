#include "Room.hpp"
#include <iostream>
#include <cctype>
#include <stdexcept>

Room::Room(const std::string& id, const std::string& name, int rows, int seatsPerRow)
    : id(id), name(name), rows(rows), seatsPerRow(seatsPerRow) {}

std::string Room::getId() const {
    return id;
}

std::string Room::getName() const {
    return name;
}

int Room::getRows() const {
    return rows;
}

int Room::getSeatsPerRow() const {
    return seatsPerRow;
}

int Room::getTotalSeats() const {
    return rows * seatsPerRow;
}

void Room::updateDetails(const std::string& name, int rows, int seatsPerRow) {
    if (rows <= 0 || seatsPerRow <= 0)
        throw std::runtime_error("Rows and seats per row must be positive.");
    if (rows > 26)
        throw std::runtime_error("Maximum 26 rows (A-Z) allowed.");
    this->name = name;
    this->rows = rows;
    this->seatsPerRow = seatsPerRow;
}

// NormalRoom
NormalRoom::NormalRoom(const std::string& id, const std::string& name, int rows, int seatsPerRow)
    : Room(id, name, rows, seatsPerRow) {}

RoomType NormalRoom::getType() const { return RoomType::NORMAL; }
std::string NormalRoom::getTypeString() const { return "Normal"; }
double NormalRoom::getSurcharge() const { return 0.0; }

// VIPRoom
VIPRoom::VIPRoom(const std::string& id, const std::string& name, int rows, int seatsPerRow)
    : Room(id, name, rows, seatsPerRow) {}

RoomType VIPRoom::getType() const { return RoomType::VIP; }
std::string VIPRoom::getTypeString() const { return "VIP"; }
double VIPRoom::getSurcharge() const { return 20000.0; }

// IMAXRoom
IMAXRoom::IMAXRoom(const std::string& id, const std::string& name, int rows, int seatsPerRow)
    : Room(id, name, rows, seatsPerRow) {}

RoomType IMAXRoom::getType() const { return RoomType::IMAX; }
std::string IMAXRoom::getTypeString() const { return "IMAX"; }
double IMAXRoom::getSurcharge() const { return 40000.0; }

bool Room::isValidSeat(const std::string& seat) const {
    if (seat.length() < 2) return false;
    
    // First char must be a letter (row)
    if (!std::isalpha(static_cast<unsigned char>(seat[0]))) return false;
    char row = std::toupper(seat[0]);
    if (row < 'A' || row >= 'A' + rows) return false;
    
    // Remaining chars must all be digits (column number)
    for (size_t i = 1; i < seat.length(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(seat[i]))) return false;
    }
    
    int col = std::stoi(seat.substr(1));
    if (col < 1 || col > seatsPerRow) return false;
    
    return true;
}

void Room::display() const {
    std::cout << "Room ID: " << id << " | Name: " << name 
              << " | Type: " << getTypeString()
              << " | Layout: " << rows << "x" << seatsPerRow
              << " | Capacity: " << getTotalSeats() << "\n";
}
