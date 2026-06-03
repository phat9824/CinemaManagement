#include "CinemaSystem.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <regex>

// ============================================================
// Constructor — registers default discount codes
// ============================================================

CinemaSystem::CinemaSystem() {
    addDiscount(std::make_shared<PercentageDiscount>("SALE10", 0.10));
    addDiscount(std::make_shared<FixedAmountDiscount>("FIX20K", 20000.0));
    addDiscount(std::make_shared<MembershipDiscount>("SILVER", 0.05, MembershipLevel::SILVER));
    addDiscount(std::make_shared<MembershipDiscount>("GOLD", 0.15, MembershipLevel::GOLD));
}

// ============================================================
// Private helpers
// ============================================================

bool CinemaSystem::movieIdExists(const std::string& movieId) const {
    return std::any_of(movies.begin(), movies.end(),
                       [&movieId](const Movie& m) { return m.getId() == movieId; });
}

bool CinemaSystem::customerIdExists(const std::string& customerId) const {
    return std::any_of(customers.begin(), customers.end(),
                       [&customerId](const Customer& c) { return c.getId() == customerId; });
}

bool CinemaSystem::showtimeIdExists(const std::string& id) const {
    return std::any_of(showtimes.begin(), showtimes.end(),
                       [&id](const Showtime& s) { return s.getId() == id; });
}

std::string CinemaSystem::generateMovieId() const {
    int maxNum = 0;
    for (const auto& m : movies) {
        const std::string& mid = m.getId();
        if (mid.length() > 1 && mid[0] == 'M') {
            try {
                int num = std::stoi(mid.substr(1));
                if (num > maxNum) maxNum = num;
            } catch (...) {}
        }
    }
    std::ostringstream oss;
    oss << "M" << std::setw(3) << std::setfill('0') << (maxNum + 1);
    return oss.str();
}

std::string CinemaSystem::generateRoomId() const {
    int maxNum = 0;
    for (const auto& r : rooms) {
        const std::string& rid = r->getId();
        if (rid.length() > 1 && rid[0] == 'P') {
            try {
                int num = std::stoi(rid.substr(1));
                if (num > maxNum) maxNum = num;
            } catch (...) {}
        }
    }
    std::ostringstream oss;
    oss << "P" << std::setw(2) << std::setfill('0') << (maxNum + 1);
    return oss.str();
}

std::string CinemaSystem::generateCustomerId() const {
    int maxNum = 0;
    for (const auto& c : customers) {
        const std::string& cid = c.getId();
        if (cid.length() > 1 && cid[0] == 'C') {
            try {
                int num = std::stoi(cid.substr(1));
                if (num > maxNum) maxNum = num;
            } catch (...) {}
        }
    }
    std::ostringstream oss;
    oss << "C" << std::setw(3) << std::setfill('0') << (maxNum + 1);
    return oss.str();
}

std::string CinemaSystem::generateShowtimeId() const {
    int maxNum = 0;
    for (const auto& s : showtimes) {
        const std::string& sid = s.getId();
        if (sid.length() > 2 && sid.substr(0, 2) == "ST") {
            try {
                int num = std::stoi(sid.substr(2));
                if (num > maxNum) maxNum = num;
            } catch (...) {}
        }
    }
    std::ostringstream oss;
    oss << "ST" << std::setw(2) << std::setfill('0') << (maxNum + 1);
    return oss.str();
}

std::string CinemaSystem::generateTicketId() const {
    int maxNum = 0;
    for (const auto& t : tickets) {
        const std::string& tid = t.getId();
        if (tid.length() > 1 && tid[0] == 'T') {
            try {
                int num = std::stoi(tid.substr(1));
                if (num > maxNum) maxNum = num;
            } catch (...) {}
        }
    }
    std::ostringstream oss;
    oss << "T" << std::setw(3) << std::setfill('0') << (maxNum + 1);
    return oss.str();
}

MovieStatus CinemaSystem::parseMovieStatus(const std::string& text) {
    std::string lower = toLowerCopy(text);
    return (lower == "showing") ? MovieStatus::SHOWING : MovieStatus::STOPPED;
}

std::string CinemaSystem::toLowerCopy(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

std::string CinemaSystem::toUpperCopy(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return text;
}

int CinemaSystem::findRoomIndex(const std::string& roomId) const {
    std::string queryId = toUpperCopy(roomId);
    for (int i = 0; i < static_cast<int>(rooms.size()); ++i)
        if (rooms[i]->getId() == queryId) return i;
    return -1;
}

const Movie* CinemaSystem::findMovieById(const std::string& movieId) const {
    std::string queryId = toUpperCopy(movieId);
    auto it = std::find_if(movies.begin(), movies.end(),
                           [&queryId](const Movie& m) { return m.getId() == queryId; });
    return (it == movies.end()) ? nullptr : &(*it);
}

void CinemaSystem::reconcileShowtimeSeats() {
    // After loading, re-apply booked seats from tickets into showtimes
    for (const auto& ticket : tickets) {
        Showtime* st = findShowtimeById(ticket.getShowtimeId());
        if (st) st->bookSeat(ticket.getSeat().getId());
    }
}

// ============================================================
// Movie Management
// ============================================================

std::string CinemaSystem::addMovie(const std::string& name, const std::string& genre, int duration, int ageRestriction) {
    if (duration <= 0)
        throw std::runtime_error("Movie duration must be positive.");
    std::string newId = generateMovieId();
    movies.push_back(Movie(newId, name, genre, duration, ageRestriction));
    return newId;
}

bool CinemaSystem::updateMovie(const std::string& movieId, const std::string& name,
                                const std::string& genre, int duration, int ageRestriction) {
    auto it = std::find_if(movies.begin(), movies.end(),
                           [&movieId](const Movie& m) { return m.getId() == movieId; });
    if (it == movies.end()) return false;
    
    std::string finalName = name.empty() ? it->getName() : name;
    std::string finalGenre = genre.empty() ? it->getGenre() : genre;
    int finalDuration = (duration <= 0) ? it->getDuration() : duration;
    int finalAge = (ageRestriction < 0) ? it->getAgeRestriction() : ageRestriction;
    
    it->updateDetails(finalName, finalGenre, finalDuration, finalAge);
    return true;
}

bool CinemaSystem::stopMovie(const std::string& movieId) {
    auto it = std::find_if(movies.begin(), movies.end(),
                           [&movieId](const Movie& m) { return m.getId() == movieId; });
    if (it == movies.end()) return false;
    it->setStatus(MovieStatus::STOPPED);
    return true;
}

std::vector<Movie> CinemaSystem::searchMoviesByName(const std::string& keyword) const {
    std::vector<Movie> result;
    const std::string lk = toLowerCopy(keyword);
    std::copy_if(movies.begin(), movies.end(), std::back_inserter(result),
                 [&lk](const Movie& m) {
                     return toLowerCopy(m.getName()).find(lk) != std::string::npos;
                 });
    return result;
}

std::vector<Movie> CinemaSystem::filterMoviesByGenre(const std::string& genre) const {
    std::vector<Movie> result;
    const std::string lg = toLowerCopy(genre);
    std::copy_if(movies.begin(), movies.end(), std::back_inserter(result),
                 [&lg](const Movie& m) { return toLowerCopy(m.getGenre()) == lg; });
    return result;
}

std::vector<Movie> CinemaSystem::getShowingMovies() const {
    std::vector<Movie> result;
    std::copy_if(movies.begin(), movies.end(), std::back_inserter(result),
                 [](const Movie& m) { return m.getStatus() == MovieStatus::SHOWING; });
    return result;
}

void CinemaSystem::displayMovies() const {
    if (movies.empty()) { std::cout << "No movies.\n"; return; }
    auto sorted = movies;
    std::sort(sorted.begin(), sorted.end(),
              [](const Movie& a, const Movie& b) { return a.getId() < b.getId(); });
    std::cout << "\n===== ALL MOVIES (" << sorted.size() << ") =====\n";
    for (const auto& m : sorted) m.display();
}

void CinemaSystem::displayShowingMovies() const {
    auto showing = getShowingMovies();
    if (showing.empty()) { std::cout << "No showing movies.\n"; return; }
    std::sort(showing.begin(), showing.end(),
              [](const Movie& a, const Movie& b) { return a.getId() < b.getId(); });
    std::cout << "\n===== SHOWING MOVIES (" << showing.size() << ") =====\n";
    for (const auto& m : showing) m.display();
    std::cout << "Total: " << showing.size() << " movies\n";
}

bool CinemaSystem::loadMovies(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    movies.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        // Remove trailing '\r' (Windows CRLF)
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream ss(line);
        std::string id, name, genre, durStr, ageStr, statusStr;
        if (!std::getline(ss, id,       '|')) continue;
        if (!std::getline(ss, name,     '|')) continue;
        if (!std::getline(ss, genre,    '|')) continue;
        if (!std::getline(ss, durStr,   '|')) continue;
        if (!std::getline(ss, ageStr,   '|')) continue;
        if (!std::getline(ss, statusStr))     continue;
        try {
            movies.emplace_back(id, name, genre,
                                std::stoi(durStr), std::stoi(ageStr),
                                parseMovieStatus(statusStr));
        } catch (...) { continue; }
    }
    return true;
}

bool CinemaSystem::saveMovies(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& m : movies)
        file << m.getId()           << '|' << m.getName()          << '|'
             << m.getGenre()        << '|' << m.getDuration()       << '|'
             << m.getAgeRestriction()<< '|' << m.getStatusString()  << '\n';
    return true;
}

// ============================================================
// Room Management
// ============================================================

std::string CinemaSystem::addRoom(const std::string& name,
                             RoomType type, int rows, int seatsPerRow) {
    if (rows <= 0 || seatsPerRow <= 0)
        throw std::runtime_error("Rows and seats per row must be positive.");
    if (rows > 26)
        throw std::runtime_error("Maximum 26 rows (A-Z) allowed.");
    
    std::string newId = generateRoomId();
    if (type == RoomType::VIP) {
        rooms.push_back(std::make_shared<VIPRoom>(newId, name, rows, seatsPerRow));
    } else if (type == RoomType::IMAX) {
        rooms.push_back(std::make_shared<IMAXRoom>(newId, name, rows, seatsPerRow));
    } else {
        rooms.push_back(std::make_shared<NormalRoom>(newId, name, rows, seatsPerRow));
    }
    return newId;
}

bool CinemaSystem::updateRoom(const std::string& roomId, const std::string& name,
                               int typeInt, int rows, int seatsPerRow) {
    int idx = findRoomIndex(roomId);
    if (idx == -1) return false;

    // Resolve final values: -1 or empty means "keep current"
    std::string finalName = name.empty() ? rooms[idx]->getName() : name;
    RoomType finalType    = (typeInt < 0) ? rooms[idx]->getType()
                          : (typeInt == 2) ? RoomType::VIP
                          : (typeInt == 3) ? RoomType::IMAX
                          :                  RoomType::NORMAL;
    int finalRows = (rows <= 0)         ? rooms[idx]->getRows()       : rows;
    int finalCols = (seatsPerRow <= 0)  ? rooms[idx]->getSeatsPerRow(): seatsPerRow;

    if (rooms[idx]->getType() != finalType) {
        // Re-instantiate if type changed
        if (finalType == RoomType::VIP) {
            rooms[idx] = std::make_shared<VIPRoom>(roomId, finalName, finalRows, finalCols);
        } else if (finalType == RoomType::IMAX) {
            rooms[idx] = std::make_shared<IMAXRoom>(roomId, finalName, finalRows, finalCols);
        } else {
            rooms[idx] = std::make_shared<NormalRoom>(roomId, finalName, finalRows, finalCols);
        }
    } else {
        rooms[idx]->updateDetails(finalName, finalRows, finalCols);
    }
    return true;
}

std::shared_ptr<Room> CinemaSystem::getRoomById(const std::string& roomId) {
    auto it = std::find_if(rooms.begin(), rooms.end(),
                           [&roomId](const std::shared_ptr<Room>& r) {
                               return r->getId() == roomId; });
    return (it == rooms.end()) ? nullptr : *it;
}

std::shared_ptr<const Room> CinemaSystem::getRoomById(const std::string& roomId) const {
    auto it = std::find_if(rooms.begin(), rooms.end(),
                           [&roomId](const std::shared_ptr<Room>& r) {
                               return r->getId() == roomId; });
    return (it == rooms.end()) ? nullptr : *it;
}

bool CinemaSystem::roomIdExists(const std::string& roomId) const {
    return std::any_of(rooms.begin(), rooms.end(),
                       [&roomId](const std::shared_ptr<Room>& r) {
                           return r->getId() == roomId; });
}

std::vector<std::shared_ptr<Room>> CinemaSystem::getAllRooms() const { return rooms; }

void CinemaSystem::displayRooms() const {
    if (rooms.empty()) { std::cout << "No rooms.\n"; return; }
    auto sorted = rooms;
    std::sort(sorted.begin(), sorted.end(),
              [](const std::shared_ptr<Room>& a, const std::shared_ptr<Room>& b) {
                  return a->getId() < b->getId(); });
    std::cout << "\n===== ALL ROOMS (" << sorted.size() << ") =====\n";
    for (const auto& r : sorted) r->display();
}

void CinemaSystem::displayShowtimeSeatLayout(const std::string& showtimeId) const {
    const Showtime* st = findShowtimeById(showtimeId);
    if (!st) { std::cout << "Showtime not found.\n"; return; }

    auto room = getRoomById(st->getRoomId());
    if (!room) { std::cout << "Room not found.\n"; return; }

    const auto& bookedSeats = st->getBookedSeats();
    int rows = room->getRows();
    int cols = room->getSeatsPerRow();

    std::cout << "\nSeat Layout for " << room->getName()
              << " (" << room->getTypeString() << ") - Showtime " << showtimeId << ":\n";
    std::cout << "Total Seats: " << room->getTotalSeats()
              << " | Booked: " << bookedSeats.size()
              << " | Available: " << (room->getTotalSeats() - static_cast<int>(bookedSeats.size())) << "\n\n";

    // Column headers
    std::cout << "   ";
    for (int c = 1; c <= cols; ++c) {
        std::cout << c << " ";
    }
    std::cout << "\n";

    // Seat grid
    for (int r = 0; r < rows; ++r) {
        char rowLabel = 'A' + r;
        std::cout << rowLabel << "  ";
        for (int c = 1; c <= cols; ++c) {
            std::string seat;
            seat += rowLabel;
            seat += std::to_string(c);

            if (bookedSeats.count(seat) > 0) {
                std::cout << "X ";
            } else {
                std::cout << "O ";
            }
            if (c >= 10) std::cout << " "; // extra space for alignment
        }
        std::cout << "\n";
    }
    std::cout << "(O = Available, X = Booked)\n\n";
}

bool CinemaSystem::loadRooms(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    rooms.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream ss(line);
        std::string id, name, typeStr, rowsStr, seatStr;
        if (!std::getline(ss, id,      '|')) continue;
        if (!std::getline(ss, name,    '|')) continue;
        if (!std::getline(ss, typeStr, '|')) continue;
        if (!std::getline(ss, rowsStr, '|')) continue;
        if (!std::getline(ss, seatStr))      continue;
        try {
            RoomType type;
            std::string lt = toLowerCopy(typeStr);
            if      (lt == "vip")  type = RoomType::VIP;
            else if (lt == "imax") type = RoomType::IMAX;
            else                   type = RoomType::NORMAL;
            if (type == RoomType::VIP) {
                rooms.push_back(std::make_shared<VIPRoom>(id, name, std::stoi(rowsStr), std::stoi(seatStr)));
            } else if (type == RoomType::IMAX) {
                rooms.push_back(std::make_shared<IMAXRoom>(id, name, std::stoi(rowsStr), std::stoi(seatStr)));
            } else {
                rooms.push_back(std::make_shared<NormalRoom>(id, name, std::stoi(rowsStr), std::stoi(seatStr)));
            }
        } catch (...) { continue; }
    }
    return true;
}

bool CinemaSystem::saveRooms(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& r : rooms)
        file << r->getId()   << '|' << r->getName()    << '|'
             << r->getTypeString() << '|' << r->getRows() << '|'
             << r->getSeatsPerRow() << '\n';
    return true;
}

// ============================================================
// Customer Management
// ============================================================

std::string CinemaSystem::addCustomer(const std::string& name, int age, const std::string& phone, MembershipLevel level) {
    if (age < 0)
        throw std::runtime_error("Customer age cannot be negative.");
    // Check duplicate phone
    auto existing = findCustomerByPhone(phone);
    if (existing.has_value())
        throw std::runtime_error("Customer with phone " + phone + " already exists.");
        
    std::string newId = generateCustomerId();
    customers.push_back(Customer(newId, name, age, phone, level));
    return newId;
}

std::optional<Customer> CinemaSystem::findCustomerByPhone(const std::string& phone) const {
    auto it = std::find_if(customers.begin(), customers.end(),
                           [&phone](const Customer& c) { return c.getPhone() == phone; });
    if (it == customers.end()) return std::nullopt;
    return *it;
}

const Customer* CinemaSystem::findCustomerById(const std::string& id) const {
    std::string queryId = toUpperCopy(id);
    auto it = std::find_if(customers.begin(), customers.end(),
                           [&queryId](const Customer& c) { return c.getId() == queryId; });
    return (it == customers.end()) ? nullptr : &(*it);
}

Customer* CinemaSystem::findCustomerById(const std::string& id) {
    std::string queryId = toUpperCopy(id);
    auto it = std::find_if(customers.begin(), customers.end(),
                           [&queryId](const Customer& c) { return c.getId() == queryId; });
    return (it == customers.end()) ? nullptr : &(*it);
}

void CinemaSystem::displayCustomers() const {
    if (customers.empty()) { std::cout << "No customers.\n"; return; }
    std::cout << "\n===== ALL CUSTOMERS (" << customers.size() << ") =====\n";
    for (const auto& c : customers) c.display();
}

void CinemaSystem::displayCustomerTickets(const std::string& customerId) const {
    const Customer* c = findCustomerById(customerId);
    if (!c) { std::cout << "[ERROR] Customer not found.\n"; return; }

    std::cout << "\n--- Ticket history for " << c->getName() << " ---\n";
    double total = 0.0;
    int    count = 0;
    for (const auto& t : tickets) {
        if (t.getCustomerId() != customerId) continue;
        const Showtime* st = findShowtimeById(t.getShowtimeId());
        const Movie*    mv = st ? findMovieById(st->getMovieId()) : nullptr;
        std::cout << "  [" << t.getId() << "] "
                  << (mv ? mv->getName() : t.getShowtimeId())
                  << " | Seat: "  << t.getSeat().getId()
                  << " | "        << t.getTicketType()
                  << " | "        << static_cast<long long>(t.getFinalPrice()) << " VND\n";
        total += t.getFinalPrice();
        ++count;
    }
    if (count == 0) std::cout << "  No tickets found.\n";
    else {
        std::cout << "  Total: " << count << " ticket(s) | "
                  << "Spending: " << static_cast<long long>(total) << " VND\n";
    }
}

double CinemaSystem::getCustomerTotalSpending(const std::string& customerId) const {
    double total = 0.0;
    for (const auto& t : tickets)
        if (t.getCustomerId() == customerId) total += t.getFinalPrice();
    return total;
}

bool CinemaSystem::loadCustomers(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    customers.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream ss(line);
        std::string id, name, ageStr, phone, levelStr;
        if (!std::getline(ss, id,       '|')) continue;
        if (!std::getline(ss, name,     '|')) continue;
        if (!std::getline(ss, ageStr,   '|')) continue;
        if (!std::getline(ss, phone,    '|')) continue;
        if (!std::getline(ss, levelStr))      continue;
        try {
            customers.emplace_back(id, name, std::stoi(ageStr), phone,
                                   Customer::parseMembership(levelStr));
        } catch (...) { continue; }
    }
    return true;
}

bool CinemaSystem::saveCustomers(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& c : customers)
        file << c.getId()   << '|' << c.getName() << '|'
             << c.getAge()  << '|' << c.getPhone() << '|'
             << c.getMembershipString() << '\n';
    return true;
}

// ============================================================
// Showtime Management
// ============================================================

std::string CinemaSystem::addShowtime(const std::string& movieId,
                                const std::string& roomId, const std::string& date,
                                const std::string& startTime, double basePrice) {
    const Movie* movie = findMovieById(movieId);
    if (!movie)
        throw std::runtime_error("Movie not found: " + movieId);
    if (movie->getStatus() != MovieStatus::SHOWING)
        throw std::runtime_error("Movie is not currently showing: " + movieId);
    if (!roomIdExists(roomId))
        throw std::runtime_error("Room not found: " + roomId);
    if (basePrice < 0)
        throw std::runtime_error("Base price cannot be negative.");

    std::regex dateRegex("^\\d{4}-\\d{2}-\\d{2}$");
    if (!std::regex_match(date, dateRegex)) {
        throw std::runtime_error("Invalid date format. Expected YYYY-MM-DD.");
    }
    
    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));

    if (month < 1 || month > 12) {
        throw std::runtime_error("Invalid month. Expected 01-12.");
    }

    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && isLeapYear) {
        daysInMonth[2] = 29;
    }

    if (day < 1 || day > daysInMonth[month]) {
        throw std::runtime_error("Invalid day for the given month and year.");
    }

    std::regex timeRegex("^([01]\\d|2[0-3]):([0-5]\\d)$");
    if (!std::regex_match(startTime, timeRegex)) {
        throw std::runtime_error("Invalid time format. Expected HH:MM (00:00 - 23:59).");
    }

    std::string newId = generateShowtimeId();
    Showtime newSt(newId, movieId, roomId, date, startTime, basePrice);

    // Conflict check — same room, same date, overlapping time
    for (const auto& existing : showtimes) {
        const Movie* em = findMovieById(existing.getMovieId());
        int existDur = em ? em->getDuration() : 0;
        if (newSt.conflictsWith(existing, movie->getDuration(), existDur)) {
            throw std::runtime_error(
                "Showtime conflicts with " + existing.getId() +
                " in room " + roomId + " (including 15-min cleanup).");
        }
    }

    showtimes.push_back(std::move(newSt));
    return newId;
}

Showtime* CinemaSystem::findShowtimeById(const std::string& id) {
    std::string queryId = toUpperCopy(id);
    auto it = std::find_if(showtimes.begin(), showtimes.end(),
                           [&queryId](const Showtime& s) { return s.getId() == queryId; });
    return (it == showtimes.end()) ? nullptr : &(*it);
}

const Showtime* CinemaSystem::findShowtimeById(const std::string& id) const {
    std::string queryId = toUpperCopy(id);
    auto it = std::find_if(showtimes.begin(), showtimes.end(),
                           [&queryId](const Showtime& s) { return s.getId() == queryId; });
    return (it == showtimes.end()) ? nullptr : &(*it);
}

void CinemaSystem::displayShowtimes() const {
    if (showtimes.empty()) { std::cout << "No showtimes.\n"; return; }
    std::cout << "\n===== ALL SHOWTIMES (" << showtimes.size() << ") =====\n";
    for (const auto& st : showtimes) {
        const Movie* mv   = findMovieById(st.getMovieId());
        auto         room = getRoomById(st.getRoomId());
        st.display(mv ? mv->getName() : st.getMovieId(),
                   room ? room->getName() : st.getRoomId(),
                   mv ? mv->getDuration() : 0);
    }
}

void CinemaSystem::displayShowtimesByDate(const std::string& date) const {
    std::cout << "\n===== SHOWTIMES ON " << date << " =====\n";
    int count = 0;
    for (const auto& st : showtimes) {
        if (st.getDate() != date) continue;
        const Movie* mv   = findMovieById(st.getMovieId());
        auto         room = getRoomById(st.getRoomId());
        st.display(mv ? mv->getName() : st.getMovieId(),
                   room ? room->getName() : st.getRoomId(),
                   mv ? mv->getDuration() : 0);
        ++count;
    }
    if (count == 0) std::cout << "No showtimes on this date.\n";
}

void CinemaSystem::displayShowtimesByMovie(const std::string& movieId) const {
    const Movie* mv = findMovieById(movieId);
    std::cout << "\n===== SHOWTIMES FOR "
              << (mv ? mv->getName() : movieId) << " =====\n";
    int count = 0;
    for (const auto& st : showtimes) {
        if (st.getMovieId() != movieId) continue;
        auto room = getRoomById(st.getRoomId());
        st.display(mv ? mv->getName() : movieId,
                   room ? room->getName() : st.getRoomId(),
                   mv ? mv->getDuration() : 0);
        ++count;
    }
    if (count == 0) std::cout << "No showtimes found.\n";
}

bool CinemaSystem::loadShowtimes(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    showtimes.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream ss(line);
        std::string id, movieId, roomId, date, time, priceStr;
        if (!std::getline(ss, id,       '|')) continue;
        if (!std::getline(ss, movieId,  '|')) continue;
        if (!std::getline(ss, roomId,   '|')) continue;
        if (!std::getline(ss, date,     '|')) continue;
        if (!std::getline(ss, time,     '|')) continue;
        if (!std::getline(ss, priceStr))      continue;
        try {
            showtimes.emplace_back(id, movieId, roomId, date, time, std::stod(priceStr));
        } catch (...) { continue; }
    }
    return true;
}

bool CinemaSystem::saveShowtimes(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& st : showtimes)
        file << st.getId()      << '|' << st.getMovieId()  << '|'
             << st.getRoomId()  << '|' << st.getDate()     << '|'
             << st.getStartTime()<< '|'
             << static_cast<long long>(st.getBasePrice()) << '\n';
    return true;
}

// ============================================================
// Ticket Selling
// ============================================================

void CinemaSystem::sellTicket(const std::string& showtimeId, const std::string& customerId,
                               const std::string& seatRaw, const std::string& ticketType,
                               const std::string& discountCode) {
    // Normalize seat: uppercase row letter (e.g. "b2" → "B2")
    std::string seat = seatRaw;
    if (!seat.empty()) seat[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(seat[0])));
    // 1. Showtime
    Showtime* st = findShowtimeById(showtimeId);
    if (!st) throw std::runtime_error("Showtime not found: " + showtimeId);

    // 2. Movie
    const Movie* movie = findMovieById(st->getMovieId());
    if (!movie) throw std::runtime_error("Data error — movie missing for showtime: " + showtimeId);

    // 3. Customer
    const Customer* customer = findCustomerById(customerId);
    if (!customer) throw std::runtime_error("Customer not found: " + customerId);

    // 4. Age restriction
    if (customer->getAge() < movie->getAgeRestriction())
        throw std::runtime_error(
            "Customer age (" + std::to_string(customer->getAge()) +
            ") is below movie age restriction (" +
            std::to_string(movie->getAgeRestriction()) + "+).");

    // 5. Room + seat validation
    auto room = getRoomById(st->getRoomId());
    if (!room) throw std::runtime_error("Data error — room missing: " + st->getRoomId());
    if (!room->isValidSeat(seat))
        throw std::runtime_error("Invalid seat: " + seat);

    // 6. Showtime-level seat availability
    if (st->isSeatBooked(seat))
        throw std::runtime_error("Seat " + seat + " is already booked for showtime " + showtimeId + ".");

    // 7. Ticket type pricing (polymorphism via TicketPricingStrategy)
    auto pricing    = createPricingStrategy(ticketType);
    double pricedBase  = pricing->calculatePrice(st->getBasePrice());
    double surcharge   = room->getSurcharge();

    // 8. Discount (polymorphism via DiscountPolicy)
    double discountAmount = 0.0;
    std::string usedCode  = "NONE";
    if (!discountCode.empty() && discountCode != "NONE") {
        auto policy = findDiscount(discountCode);
        if (!policy)
            throw std::runtime_error("Invalid discount code: " + discountCode);
        double afterDiscount = policy->applyDiscount(pricedBase + surcharge, *customer);
        discountAmount = (pricedBase + surcharge) - afterDiscount;
        usedCode = discountCode;
    }

    // 9. Final price validation
    double finalPrice = pricedBase + surcharge - discountAmount;
    if (finalPrice < 0)
        throw std::runtime_error("Discount exceeds ticket price.");

    // 10. Create and store ticket
    std::string ticketId = generateTicketId();
    Ticket ticket(ticketId, customerId, showtimeId, Seat(seat),
                  pricing->getName(), pricedBase, surcharge, discountAmount, usedCode);

    st->bookSeat(seat);
    tickets.push_back(ticket);

    // 11. Print ticket receipt
    ticket.printTicket(customer->getName(), movie->getName(),
                       room->getName() + " - " + room->getTypeString(),
                       st->getDate() + " " + st->getStartTime());
}

void CinemaSystem::displayTickets() const {
    if (tickets.empty()) { std::cout << "No tickets sold.\n"; return; }
    std::cout << "\n===== ALL TICKETS (" << tickets.size() << ") =====\n";
    for (const auto& t : tickets) {
        const Customer* c  = findCustomerById(t.getCustomerId());
        const Showtime* st = findShowtimeById(t.getShowtimeId());
        const Movie*    mv = st ? findMovieById(st->getMovieId()) : nullptr;
        std::cout << "[" << t.getId() << "] "
                  << (c  ? c->getName()    : t.getCustomerId())
                  << " | " << (mv ? mv->getName() : t.getShowtimeId())
                  << " | Seat: " << t.getSeat().getId()
                  << " | " << t.getTicketType()
                  << " | " << static_cast<long long>(t.getFinalPrice()) << " VND\n";
    }
}

void CinemaSystem::displayTicket(const std::string& ticketId) const {
    auto it = std::find_if(tickets.begin(), tickets.end(),
                           [&ticketId](const Ticket& t) { return t.getId() == ticketId; });
    if (it == tickets.end()) { std::cout << "Ticket not found: " << ticketId << "\n"; return; }

    const Customer* c    = findCustomerById(it->getCustomerId());
    const Showtime* st   = findShowtimeById(it->getShowtimeId());
    const Movie*    mv   = st ? findMovieById(st->getMovieId()) : nullptr;
    auto            room = st ? getRoomById(st->getRoomId()) : nullptr;

    it->printTicket(
        c    ? c->getName()  : it->getCustomerId(),
        mv   ? mv->getName() : it->getShowtimeId(),
        room ? (room->getName() + " - " + room->getTypeString()) : it->getShowtimeId(),
        st   ? (st->getDate() + " " + st->getStartTime()) : "");
}

bool CinemaSystem::loadTickets(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    tickets.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream ss(line);
        std::string id, custId, stId, seat, type, priceStr;
        if (!std::getline(ss, id,       '|')) continue;
        if (!std::getline(ss, custId,   '|')) continue;
        if (!std::getline(ss, stId,     '|')) continue;
        if (!std::getline(ss, seat,     '|')) continue;
        if (!std::getline(ss, type,     '|')) continue;
        if (!std::getline(ss, priceStr))      continue;
        try {
            tickets.emplace_back(id, custId, stId, Seat(seat), type, std::stod(priceStr));
        } catch (...) { continue; }
    }
    return true;
}

bool CinemaSystem::saveTickets(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& t : tickets)
        file << t.getId()          << '|' << t.getCustomerId()  << '|'
             << t.getShowtimeId()  << '|' << t.getSeat().getId() << '|'
             << t.getTicketType()  << '|'
             << static_cast<long long>(t.getFinalPrice()) << '\n';
    return true;
}

// ============================================================
// Revenue Statistics
// ============================================================

void CinemaSystem::displayRevenueReport() const {
    if (tickets.empty()) { std::cout << "No ticket data available.\n"; return; }

    double totalRevenue = 0.0;
    for (const auto& t : tickets) totalRevenue += t.getFinalPrice();
    int    totalTickets = static_cast<int>(tickets.size());

    // Revenue + count per movie using std::map (sorted by key)
    std::map<std::string, double> revenueByMovieId;
    std::map<std::string, int>    countByMovieId;
    for (const auto& t : tickets) {
        const Showtime* st = findShowtimeById(t.getShowtimeId());
        if (st) {
            revenueByMovieId[st->getMovieId()] += t.getFinalPrice();
            countByMovieId  [st->getMovieId()]++;
        }
    }

    // Revenue per date using std::map
    std::map<std::string, double> revenueByDate;
    for (const auto& t : tickets) {
        const Showtime* st = findShowtimeById(t.getShowtimeId());
        if (st) revenueByDate[st->getDate()] += t.getFinalPrice();
    }

    // Sort movies by revenue descending (for top 3)
    std::vector<std::pair<std::string, double>> movieRevVec(
        revenueByMovieId.begin(), revenueByMovieId.end());
    std::sort(movieRevVec.begin(), movieRevVec.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Ticket type counts using std::count_if
    int adultCount   = static_cast<int>(std::count_if(tickets.begin(), tickets.end(),
        [](const Ticket& t) { return t.getTicketType() == "Adult"; }));
    int childCount   = static_cast<int>(std::count_if(tickets.begin(), tickets.end(),
        [](const Ticket& t) { return t.getTicketType() == "Child"; }));
    int studentCount = static_cast<int>(std::count_if(tickets.begin(), tickets.end(),
        [](const Ticket& t) { return t.getTicketType() == "Student"; }));

    // Overall occupancy rate
    int totalCapacity = 0;
    for (const auto& st : showtimes) {
        auto room = getRoomById(st.getRoomId());
        if (room) totalCapacity += room->getTotalSeats();
    }

    std::cout << "\n========= REVENUE REPORT =========\n";
    std::cout << "Total tickets sold: " << totalTickets << "\n";
    std::cout << "  - Adult:   " << adultCount   << "\n";
    std::cout << "  - Child:   " << childCount   << "\n";
    std::cout << "  - Student: " << studentCount << "\n";
    std::cout << "Total revenue: " << static_cast<long long>(totalRevenue) << " VND\n";
    if (totalCapacity > 0) {
        double rate = (static_cast<double>(totalTickets) / totalCapacity) * 100.0;
        std::cout << "Seat occupancy rate: " << std::fixed << std::setprecision(1)
                  << rate << "%\n";
    }

    std::cout << "\nRevenue by movie:\n";
    int rank = 1;
    for (const auto& [mid, rev] : movieRevVec) {
        const Movie* mv = findMovieById(mid);
        std::cout << "  " << rank++ << ". "
                  << (mv ? mv->getName() : mid)
                  << ": " << static_cast<long long>(rev) << " VND"
                  << " (" << countByMovieId.at(mid) << " tickets)\n";
    }

    std::cout << "\nRevenue by date:\n";
    for (const auto& [date, rev] : revenueByDate)
        std::cout << "  " << date << ": " << static_cast<long long>(rev) << " VND\n";

    int top = std::min(3, static_cast<int>(movieRevVec.size()));
    std::cout << "\nTop " << top << " highest revenue movies:\n";
    for (int i = 0; i < top; ++i) {
        const Movie* mv = findMovieById(movieRevVec[i].first);
        std::cout << "  " << (i + 1) << ". "
                  << (mv ? mv->getName() : movieRevVec[i].first)
                  << ": " << static_cast<long long>(movieRevVec[i].second) << " VND\n";
    }
    std::cout << "==================================\n";
}

// ============================================================
// Discount Management
// ============================================================

void CinemaSystem::addDiscount(std::shared_ptr<DiscountPolicy> policy) {
    discountPolicies[policy->getCode()] = std::move(policy);
}

void CinemaSystem::displayDiscounts() const {
    std::cout << "\n===== DISCOUNT CODES =====\n";
    if (discountPolicies.empty()) { std::cout << "No discount codes.\n"; return; }
    for (const auto& [code, policy] : discountPolicies)
        std::cout << "  " << code << "  ->  " << policy->getDescription() << "\n";
}

std::shared_ptr<DiscountPolicy> CinemaSystem::findDiscount(const std::string& code) const {
    std::string queryCode = toUpperCopy(code);
    auto it = discountPolicies.find(queryCode);
    return (it == discountPolicies.end()) ? nullptr : it->second;
}

// ============================================================
// Batch Save / Load
// ============================================================

bool CinemaSystem::saveAll(const std::string& dataDir) const {
    bool ok = true;
    ok &= saveMovies   (dataDir + "/movies.txt");
    ok &= saveRooms    (dataDir + "/rooms.txt");
    ok &= saveCustomers(dataDir + "/customers.txt");
    ok &= saveShowtimes(dataDir + "/showtimes.txt");
    ok &= saveTickets  (dataDir + "/tickets.txt");
    return ok;
}

bool CinemaSystem::loadAll(const std::string& dataDir) {
    bool ok = true;
    ok &= loadMovies   (dataDir + "/movies.txt");
    ok &= loadRooms    (dataDir + "/rooms.txt");
    ok &= loadCustomers(dataDir + "/customers.txt");
    ok &= loadShowtimes(dataDir + "/showtimes.txt");
    ok &= loadTickets  (dataDir + "/tickets.txt");
    reconcileShowtimeSeats();  // rebuild per-showtime booked seats from tickets
    return ok;
}