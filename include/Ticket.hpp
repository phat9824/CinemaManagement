#pragma once

#include <string>
#include "Seat.hpp"

class Ticket {
private:
    std::string id;
    std::string customerId;
    std::string showtimeId;
    Seat        seat;
    std::string ticketType;      // "Adult" | "Child" | "Student"
    double      basePrice;       // after ticket-type pricing applied to showtime base
    double      roomSurcharge;
    double      discountAmount;
    double      finalPrice;
    std::string discountCode;    // "NONE" if no discount used

public:
    /** Full constructor: use when selling a new ticket. */
    Ticket(const std::string& id,
           const std::string& customerId,
           const std::string& showtimeId,
           const Seat& seat,
           const std::string& ticketType,
           double basePrice,
           double roomSurcharge,
           double discountAmount,
           const std::string& discountCode = "NONE");

    /** Simple constructor: use when loading from file (only finalPrice known). */
    Ticket(const std::string& id,
           const std::string& customerId,
           const std::string& showtimeId,
           const Seat& seat,
           const std::string& ticketType,
           double finalPrice);

    // Getters
    std::string getId()             const;
    std::string getCustomerId()     const;
    std::string getShowtimeId()     const;
    Seat        getSeat()           const;
    std::string getTicketType()     const;
    double      getBasePrice()      const;
    double      getRoomSurcharge()  const;
    double      getDiscountAmount() const;
    double      getFinalPrice()     const;
    std::string getDiscountCode()   const;

    void printTicket(const std::string& customerName,
                     const std::string& movieName,
                     const std::string& roomInfo,
                     const std::string& showDatetime) const;
};
