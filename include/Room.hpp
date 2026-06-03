#pragma once

#include <string>

enum class RoomType {
    NORMAL,
    VIP,
    IMAX
};

class Room {
protected:
    std::string id;
    std::string name;
    int rows;
    int seatsPerRow;
public:
    Room(const std::string& id, const std::string& name, int rows, int seatsPerRow);
    virtual ~Room() = default;

    // Getters
    std::string getId() const;
    std::string getName() const;
    int getRows() const;
    int getSeatsPerRow() const;
    int getTotalSeats() const;

    // Polymorphic Getters
    virtual RoomType getType() const = 0;
    virtual std::string getTypeString() const = 0;
    virtual double getSurcharge() const = 0;

    // Setters
    void updateDetails(const std::string& name, int rows, int seatsPerRow);

    // Validation
    bool isValidSeat(const std::string& seat) const;

    void display() const;
};

class NormalRoom : public Room {
public:
    NormalRoom(const std::string& id, const std::string& name, int rows, int seatsPerRow);
    RoomType getType() const override;
    std::string getTypeString() const override;
    double getSurcharge() const override;
};

class VIPRoom : public Room {
public:
    VIPRoom(const std::string& id, const std::string& name, int rows, int seatsPerRow);
    RoomType getType() const override;
    std::string getTypeString() const override;
    double getSurcharge() const override;
};

class IMAXRoom : public Room {
public:
    IMAXRoom(const std::string& id, const std::string& name, int rows, int seatsPerRow);
    RoomType getType() const override;
    std::string getTypeString() const override;
    double getSurcharge() const override;
};
