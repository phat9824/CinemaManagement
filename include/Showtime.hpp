#pragma once

#include <set>
#include <string>

class Showtime {
private:
    std::string           id;
    std::string           movieId;
    std::string           roomId;
    std::string           date;        // "YYYY-MM-DD"
    std::string           startTime;   // "HH:MM"
    double                basePrice;
    std::set<std::string> bookedSeats; // booked per-showtime (not per-room)

public:
    Showtime(const std::string& id, const std::string& movieId, const std::string& roomId,
             const std::string& date, const std::string& startTime, double basePrice);

    // Getters
    std::string                   getId()          const;
    std::string                   getMovieId()     const;
    std::string                   getRoomId()      const;
    std::string                   getDate()        const;
    std::string                   getStartTime()   const;
    double                        getBasePrice()   const;
    const std::set<std::string>&  getBookedSeats() const;

    // Seat management
    bool isSeatBooked(const std::string& seat) const;
    bool bookSeat(const std::string& seat);
    void cancelSeatBooking(const std::string& seat);
    int  getBookedSeatCount() const;

    // Time helpers
    int getStartMinutes()                          const; // "HH:MM" -> minutes since midnight
    int getEndMinutes(int movieDurationMinutes)     const; // start + duration + 15-min cleanup

    // Conflict detection: same room, same date, overlapping interval
    bool conflictsWith(const Showtime& other,
                       int thisMovieDuration,
                       int otherMovieDuration) const;

    void display(const std::string& movieName,
                 const std::string& roomName,
                 int movieDuration) const;
};
