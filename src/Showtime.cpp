#include "Showtime.hpp"

#include <iostream>
#include <stdexcept>

Showtime::Showtime(const std::string& id, const std::string& movieId, const std::string& roomId,
                   const std::string& date, const std::string& startTime, double basePrice)
    : id(id), movieId(movieId), roomId(roomId),
      date(date), startTime(startTime), basePrice(basePrice) {}

std::string                  Showtime::getId()          const { return id; }
std::string                  Showtime::getMovieId()     const { return movieId; }
std::string                  Showtime::getRoomId()      const { return roomId; }
std::string                  Showtime::getDate()        const { return date; }
std::string                  Showtime::getStartTime()   const { return startTime; }
double                       Showtime::getBasePrice()   const { return basePrice; }
const std::set<std::string>& Showtime::getBookedSeats() const { return bookedSeats; }

bool Showtime::isSeatBooked(const std::string& seat) const {
    return bookedSeats.count(seat) > 0;
}

bool Showtime::bookSeat(const std::string& seat) {
    if (isSeatBooked(seat)) return false;
    bookedSeats.insert(seat);
    return true;
}

void Showtime::cancelSeatBooking(const std::string& seat) {
    bookedSeats.erase(seat);
}

int Showtime::getBookedSeatCount() const {
    return static_cast<int>(bookedSeats.size());
}

int Showtime::getStartMinutes() const {
    // Parse "HH:MM"
    int hours = std::stoi(startTime.substr(0, 2));
    int mins  = std::stoi(startTime.substr(3, 2));
    return hours * 60 + mins;
}

int Showtime::getEndMinutes(int movieDurationMinutes) const {
    return getStartMinutes() + movieDurationMinutes + 15; // 15-min cleanup buffer
}

bool Showtime::conflictsWith(const Showtime& other,
                              int thisMovieDuration,
                              int otherMovieDuration) const {
    // Must be same room AND same date to conflict
    if (roomId != other.roomId || date != other.date) return false;

    int thisStart  = getStartMinutes();
    int thisEnd    = getEndMinutes(thisMovieDuration);
    int otherStart = other.getStartMinutes();
    int otherEnd   = other.getEndMinutes(otherMovieDuration);

    // Intervals [thisStart, thisEnd) and [otherStart, otherEnd) overlap
    return thisStart < otherEnd && thisEnd > otherStart;
}

void Showtime::display(const std::string& movieName,
                        const std::string& roomName,
                        int movieDuration) const {
    std::cout << "[" << id << "] "
              << movieName
              << " | Room: "    << roomName
              << " | "          << date << " " << startTime
              << " | "          << movieDuration << " min"
              << " | Price: "   << static_cast<long long>(basePrice) << " VND"
              << " | Booked: "  << bookedSeats.size() << " seat(s)\n";
}
