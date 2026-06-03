#include "Ticket.hpp"

#include <iostream>

// Full constructor — used when selling a new ticket
Ticket::Ticket(const std::string& id, const std::string& customerId,
               const std::string& showtimeId, const Seat& seat,
               const std::string& ticketType,
               double basePrice, double roomSurcharge, double discountAmount,
               const std::string& discountCode)
    : id(id), customerId(customerId), showtimeId(showtimeId), seat(seat),
      ticketType(ticketType), basePrice(basePrice),
      roomSurcharge(roomSurcharge), discountAmount(discountAmount),
      discountCode(discountCode)
{
    finalPrice = basePrice + roomSurcharge - discountAmount;
    if (finalPrice < 0.0) finalPrice = 0.0;
}

// Simple constructor — used when loading from file (only finalPrice available)
Ticket::Ticket(const std::string& id,
               const std::string& customerId,
               const std::string& showtimeId,
               const Seat& seat,
               const std::string& ticketType,
               double finalPrice)
    : id(id), customerId(customerId), showtimeId(showtimeId), seat(seat),
      ticketType(ticketType), basePrice(finalPrice),
      roomSurcharge(0.0), discountAmount(0.0),
      finalPrice(finalPrice), discountCode("NONE") {}

std::string Ticket::getId()             const { return id; }
std::string Ticket::getCustomerId()     const { return customerId; }
std::string Ticket::getShowtimeId()     const { return showtimeId; }
Seat        Ticket::getSeat()           const { return seat; }
std::string Ticket::getTicketType()     const { return ticketType; }
double      Ticket::getBasePrice()      const { return basePrice; }
double      Ticket::getRoomSurcharge()  const { return roomSurcharge; }
double      Ticket::getDiscountAmount() const { return discountAmount; }
double      Ticket::getFinalPrice()     const { return finalPrice; }
std::string Ticket::getDiscountCode()   const { return discountCode; }

void Ticket::printTicket(const std::string& customerName,
                          const std::string& movieName,
                          const std::string& roomInfo,
                          const std::string& showDatetime) const {
    std::cout << "========= TICKET =========\n";
    std::cout << "Ticket ID:      " << id            << "\n";
    std::cout << "Customer:       " << customerName  << "\n";
    std::cout << "Movie:          " << movieName     << "\n";
    std::cout << "Room:           " << roomInfo      << "\n";
    std::cout << "Showtime:       " << showDatetime  << "\n";
    std::cout << "Seat:           " << seat.getId()  << "\n";
    std::cout << "Ticket type:    " << ticketType    << "\n";
    std::cout << "Base price:     " << static_cast<long long>(basePrice)     << " VND\n";
    std::cout << "Room surcharge: " << static_cast<long long>(roomSurcharge) << " VND\n";
    std::cout << "Discount:       " << static_cast<long long>(discountAmount) << " VND";
    if (discountCode != "NONE") std::cout << " (" << discountCode << ")";
    std::cout << "\n";
    std::cout << "Final price:    " << static_cast<long long>(finalPrice)    << " VND\n";
    std::cout << "==========================\n";
}
