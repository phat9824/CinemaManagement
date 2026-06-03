#pragma once

#include "Customer.hpp"
#include "DiscountPolicy.hpp"
#include "Movie.hpp"
#include "Room.hpp"
#include "Showtime.hpp"
#include "Ticket.hpp"
#include "TicketPricingStrategy.hpp"

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class CinemaSystem {
private:
    std::vector<Movie>                                           movies;
    std::vector<std::shared_ptr<Room>>                          rooms;
    std::vector<Customer>                                        customers;
    std::vector<Showtime>                                        showtimes;
    std::vector<Ticket>                                          tickets;
    std::unordered_map<std::string, std::shared_ptr<DiscountPolicy>> discountPolicies;

public:
    CinemaSystem(); // registers default discount codes: SALE10, FIX20K, GOLD

    // ---- Movie management ----
    std::string addMovie(const std::string& name, const std::string& genre, int duration, int ageRestriction);
    bool updateMovie(const std::string& movieId, const std::string& name,
                     const std::string& genre, int duration, int ageRestriction);
    bool stopMovie(const std::string& movieId);
    std::vector<Movie> searchMoviesByName(const std::string& keyword) const;
    std::vector<Movie> filterMoviesByGenre(const std::string& genre) const;
    std::vector<Movie> getShowingMovies() const;
    void displayMovies() const;
    void displayShowingMovies() const;
    bool loadMovies(const std::string& filePath);
    bool saveMovies(const std::string& filePath) const;

    // ---- Room management ----
    std::string addRoom(const std::string& name, RoomType type, int rows, int seatsPerRow);
    bool updateRoom(const std::string& roomId, const std::string& name,
                    int typeInt, int rows, int seatsPerRow);
    std::shared_ptr<Room>       getRoomById(const std::string& roomId);
    std::shared_ptr<const Room> getRoomById(const std::string& roomId) const;
    bool roomIdExists(const std::string& roomId) const;
    std::vector<std::shared_ptr<Room>> getAllRooms() const;
    void displayRooms() const;
    void displayShowtimeSeatLayout(const std::string& showtimeId) const;
    bool loadRooms(const std::string& filePath);
    bool saveRooms(const std::string& filePath) const;

    // ---- Customer management ----
    std::string addCustomer(const std::string& name, int age, const std::string& phone, MembershipLevel level);
    std::optional<Customer> findCustomerByPhone(const std::string& phone) const;
    const Customer* findCustomerById(const std::string& id) const;
    Customer*       findCustomerById(const std::string& id);
    void displayCustomers() const;
    void displayCustomerTickets(const std::string& customerId) const;
    double getCustomerTotalSpending(const std::string& customerId) const;
    bool loadCustomers(const std::string& filePath);
    bool saveCustomers(const std::string& filePath) const;

    // ---- Showtime management ----
    std::string addShowtime(const std::string& movieId,
                            const std::string& roomId, const std::string& date,
                            const std::string& startTime, double basePrice);
    Showtime*       findShowtimeById(const std::string& id);
    const Showtime* findShowtimeById(const std::string& id) const;
    void displayShowtimes() const;
    void displayShowtimesByDate(const std::string& date) const;
    void displayShowtimesByMovie(const std::string& movieId) const;
    bool loadShowtimes(const std::string& filePath);
    bool saveShowtimes(const std::string& filePath) const;

    // ---- Ticket selling ----
    void sellTicket(const std::string& showtimeId, const std::string& customerId,
                    const std::string& seat, const std::string& ticketType,
                    const std::string& discountCode);
    void displayTickets() const;
    void displayTicket(const std::string& ticketId) const;
    bool loadTickets(const std::string& filePath);
    bool saveTickets(const std::string& filePath) const;

    // ---- Revenue statistics ----
    void displayRevenueReport() const;

    // ---- Discount management ----
    void addDiscount(std::shared_ptr<DiscountPolicy> policy);
    void displayDiscounts() const;
    std::shared_ptr<DiscountPolicy> findDiscount(const std::string& code) const;

    // ---- Batch save / load ----
    bool saveAll(const std::string& dataDir) const;
    bool loadAll(const std::string& dataDir);

private:
    bool movieIdExists(const std::string& movieId)       const;
    bool customerIdExists(const std::string& customerId) const;
    bool showtimeIdExists(const std::string& showtimeId) const;
    
    std::string generateMovieId() const;
    std::string generateRoomId() const;
    std::string generateCustomerId() const;
    std::string generateShowtimeId() const;
    std::string generateTicketId() const;

    void reconcileShowtimeSeats();       // re-apply booked seats from tickets after load

    const Movie* findMovieById(const std::string& movieId) const;

    static MovieStatus parseMovieStatus(const std::string& text);
    static std::string toLowerCopy(std::string text);
    static std::string toUpperCopy(std::string text);
    int findRoomIndex(const std::string& roomId) const;
};