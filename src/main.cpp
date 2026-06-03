#include "CinemaSystem.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

// ============================================================
// Input helpers
// ============================================================

void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) { clearInputBuffer(); return value; }
        std::cin.clear();
        clearInputBuffer();
        std::cout << "Invalid number, please try again.\n";
    }
}

int readIntOptional(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (value.empty()) return -1;
        try {
            size_t pos;
            int num = std::stoi(value, &pos);
            if (pos == value.length()) return num;
        } catch (...) {}
        std::cout << "Invalid number, please try again.\n";
    }
}

double readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) { clearInputBuffer(); return value; }
        std::cin.clear();
        clearInputBuffer();
        std::cout << "Invalid number, please try again.\n";
    }
}

// ============================================================
// Display helpers
// ============================================================

void printMovieList(const std::vector<Movie>& movies) {
    if (movies.empty()) { std::cout << "No movies found.\n"; return; }
    for (const auto& m : movies) m.display();
}

RoomType getRoomTypeInput() {
    std::cout << "Room type:\n  1. Normal\n  2. VIP\n  3. IMAX\n";
    int choice = readInt("Choose: ");
    switch (choice) {
        case 2: return RoomType::VIP;
        case 3: return RoomType::IMAX;
        default: std::cout << "Defaulting to Normal.\n"; return RoomType::NORMAL;
    }
}

std::string getTicketTypeInput() {
    std::cout << "Ticket type:\n  1. Adult (full price)\n  2. Child (30% off)\n  3. Student (20% off)\n";
    int choice = readInt("Choose: ");
    switch (choice) {
        case 2: return "Child";
        case 3: return "Student";
        default: return "Adult";
    }
}

// ============================================================
// Menu displays
// ============================================================

void showMainMenu() {
    std::cout << "\n========== CINEMA MANAGEMENT ==========\n";
    std::cout << "1. Movie management\n";
    std::cout << "2. Room management\n";
    std::cout << "3. Showtime management\n";
    std::cout << "4. Customer management\n";
    std::cout << "5. Sell ticket\n";
    std::cout << "6. View tickets\n";
    std::cout << "7. Revenue report\n";
    std::cout << "8. Save data\n";
    std::cout << "9. Load data\n";
    std::cout << "0. Exit\n";
    std::cout << "=======================================\n";
    std::cout << "Choose: ";
}

void showMovieMenu() {
    std::cout << "\n----- Movie Management -----\n";
    std::cout << "1. Add movie\n";
    std::cout << "2. Update movie\n";
    std::cout << "3. Stop showing movie\n";
    std::cout << "4. Search movie by name\n";
    std::cout << "5. Filter movies by genre\n";
    std::cout << "6. List all showing movies\n";
    std::cout << "7. Display all movies\n";
    std::cout << "0. Back\n";
    std::cout << "Choose: ";
}

void showRoomMenu() {
    std::cout << "\n----- Room Management -----\n";
    std::cout << "1. Add room\n";
    std::cout << "2. Update room\n";
    std::cout << "3. Display all rooms\n";
    std::cout << "4. View showtime seat layout\n";
    std::cout << "0. Back\n";
    std::cout << "Choose: ";
}

void showShowtimeMenu() {
    std::cout << "\n----- Showtime Management -----\n";
    std::cout << "1. Add showtime\n";
    std::cout << "2. Display all showtimes\n";
    std::cout << "3. Display showtimes by date\n";
    std::cout << "4. Display showtimes by movie\n";
    std::cout << "0. Back\n";
    std::cout << "Choose: ";
}

void showCustomerMenu() {
    std::cout << "\n----- Customer Management -----\n";
    std::cout << "1. Add customer\n";
    std::cout << "2. Search customer by phone\n";
    std::cout << "3. View customer ticket history\n";
    std::cout << "4. View all customers\n";
    std::cout << "0. Back\n";
    std::cout << "Choose: ";
}

// ============================================================
// Handler: Movie Management
// ============================================================

void handleMovieManagement(CinemaSystem& cinema) {
    while (true) {
        showMovieMenu();
        int choice = readInt("");
        try {
            switch (choice) {
                case 1: {
                    std::string name  = readLine("Movie name: ");
                    std::string genre = readLine("Genre: ");
                    int dur   = readInt("Duration (minutes): ");
                    int age   = readInt("Age restriction: ");
                    std::string newId = cinema.addMovie(name, genre, dur, age);
                    std::cout << "[OK] Movie added. Generated ID: " << newId << "\n";
                    break;
                }
                case 2: {
                    cinema.displayMovies();
                    std::string id = readLine("Movie ID to update: ");
                    auto results = cinema.searchMoviesByName("");
                    const Movie* found = nullptr;
                    for (const auto& m : results) {
                        if (m.getId() == id) { found = &m; break; }
                    }
                    if (!found) { std::cout << "[ERROR] Movie not found.\n"; break; }
                    std::cout << "Current: " << found->getName()
                              << " | " << found->getGenre()
                              << " | " << found->getDuration() << " min"
                              << " | Age: " << found->getAgeRestriction() << "+\n";
                    std::string name  = readLine("New name (Enter to keep): ");
                    std::string genre = readLine("New genre (Enter to keep): ");
                    int dur = readIntOptional("New duration (Enter to keep): ");
                    int age = readIntOptional("New age restriction (Enter to keep): ");
                    if (cinema.updateMovie(id, name, genre, dur, age))
                        std::cout << "[OK] Movie updated.\n";
                    else
                        std::cout << "[ERROR] Movie not found.\n";
                    break;
                }
                case 3: {
                    std::string id = readLine("Movie ID to stop: ");
                    if (cinema.stopMovie(id))
                        std::cout << "[OK] Movie stopped.\n";
                    else
                        std::cout << "[ERROR] Movie not found.\n";
                    break;
                }
                case 4: {
                    std::string kw = readLine("Search keyword: ");
                    auto result = cinema.searchMoviesByName(kw);
                    std::cout << "\nResults (" << result.size() << "):\n";
                    printMovieList(result);
                    break;
                }
                case 5: {
                    std::string genre = readLine("Genre to filter: ");
                    auto result = cinema.filterMoviesByGenre(genre);
                    std::cout << "\nResults (" << result.size() << "):\n";
                    printMovieList(result);
                    break;
                }
                case 6:
                    cinema.displayShowingMovies();
                    break;
                case 7:
                    cinema.displayMovies();
                    break;
                case 0:
                    return;
                default:
                    std::cout << "[ERROR] Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
    }
}

// ============================================================
// Handler: Room Management
// ============================================================

void handleRoomManagement(CinemaSystem& cinema) {
    while (true) {
        showRoomMenu();
        int choice = readInt("");
        try {
            switch (choice) {
                case 1: {
                    std::string name = readLine("Room name: ");
                    RoomType type    = getRoomTypeInput();
                    int rows = readInt("Number of rows: ");
                    int cols = readInt("Seats per row: ");
                    std::string newId = cinema.addRoom(name, type, rows, cols);
                    std::cout << "[OK] Room added. Generated ID: " << newId << "\n";
                    break;
                }
                case 2: {
                    cinema.displayRooms();
                    std::string id = readLine("Room ID to update: ");
                    auto room = cinema.getRoomById(id);
                    if (!room) { std::cout << "[ERROR] Room not found.\n"; break; }
                    std::cout << "Current: " << room->getName()
                              << " | " << room->getTypeString()
                              << " | " << room->getRows() << "x" << room->getSeatsPerRow() << "\n";
                    std::string name = readLine("New name (Enter to keep): ");
                    std::cout << "New room type (1=Normal, 2=VIP, 3=IMAX, Enter to keep): ";
                    int typeInt = readIntOptional("");
                    int rows = readIntOptional("New rows (Enter to keep): ");
                    int cols = readIntOptional("New seats per row (Enter to keep): ");
                    if (cinema.updateRoom(id, name, typeInt, rows, cols))
                        std::cout << "[OK] Room updated.\n";
                    else
                        std::cout << "[ERROR] Room not found.\n";
                    break;
                }
                case 3:
                    cinema.displayRooms();
                    break;
                case 4: {
                    cinema.displayShowtimes();
                    std::string stId = readLine("Showtime ID to view seat layout: ");
                    cinema.displayShowtimeSeatLayout(stId);
                    break;
                }
                case 0:
                    return;
                default:
                    std::cout << "[ERROR] Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
    }
}

// ============================================================
// Handler: Showtime Management
// ============================================================

void handleShowtimeManagement(CinemaSystem& cinema) {
    while (true) {
        showShowtimeMenu();
        int choice = readInt("");
        try {
            switch (choice) {
                case 1: {
                    cinema.displayMovies();
                    std::string movieId = readLine("Movie ID: ");
                    cinema.displayRooms();
                    std::string roomId  = readLine("Room ID: ");
                    std::string date    = readLine("Date (YYYY-MM-DD): ");
                    std::string time    = readLine("Start time (HH:MM): ");
                    double price = readDouble("Base ticket price (VND): ");
                    std::string newId = cinema.addShowtime(movieId, roomId, date, time, price);
                    std::cout << "[OK] Showtime added. Generated ID: " << newId << "\n";
                    break;
                }
                case 2:
                    cinema.displayShowtimes();
                    break;
                case 3: {
                    std::string date = readLine("Date (YYYY-MM-DD): ");
                    cinema.displayShowtimesByDate(date);
                    break;
                }
                case 4: {
                    cinema.displayMovies();
                    std::string movieId = readLine("Movie ID: ");
                    cinema.displayShowtimesByMovie(movieId);
                    break;
                }
                case 0:
                    return;
                default:
                    std::cout << "[ERROR] Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
    }
}

// ============================================================
// Handler: Customer Management
// ============================================================

void handleCustomerManagement(CinemaSystem& cinema) {
    while (true) {
        showCustomerMenu();
        int choice = readInt("");
        try {
            switch (choice) {
                case 1: {
                    std::string name  = readLine("Full name: ");
                    int age = readInt("Age: ");
                    std::string phone = readLine("Phone number: ");
                    std::cout << "Membership level:\n  1. Normal\n  2. Silver\n  3. Gold\n";
                    int lvl = readInt("Choose: ");
                    MembershipLevel level = MembershipLevel::NORMAL;
                    if (lvl == 2) level = MembershipLevel::SILVER;
                    else if (lvl == 3) level = MembershipLevel::GOLD;
                    std::string newId = cinema.addCustomer(name, age, phone, level);
                    std::cout << "[OK] Customer added. Generated ID: " << newId << "\n";
                    break;
                }
                case 2: {
                    std::string phone = readLine("Phone number to search: ");
                    auto result = cinema.findCustomerByPhone(phone);
                    if (result.has_value()) {
                        std::cout << "\nCustomer found:\n";
                        result->display();
                        std::cout << "Total spending: "
                                  << static_cast<long long>(
                                         cinema.getCustomerTotalSpending(result->getId()))
                                  << " VND\n";
                    } else {
                        std::cout << "No customer found with phone: " << phone << "\n";
                    }
                    break;
                }
                case 3: {
                    cinema.displayCustomers();
                    std::string id = readLine("Customer ID: ");
                    cinema.displayCustomerTickets(id);
                    break;
                }
                case 4:
                    cinema.displayCustomers();
                    break;
                case 0:
                    return;
                default:
                    std::cout << "[ERROR] Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
    }
}

// ============================================================
// Handler: Sell Ticket
// ============================================================

void handleSellTicket(CinemaSystem& cinema) {
    try {
        // Step 1: Pick showtime
        cinema.displayShowtimes();
        std::string showtimeId = readLine("\nShowtime ID: ");

        // Step 2: Find showtime to show seat info
        const Showtime* st = const_cast<const CinemaSystem&>(cinema).findShowtimeById(showtimeId);
        if (!st) { std::cout << "[ERROR] Showtime not found.\n"; return; }

        // Show seat layout for this showtime (based on showtime bookings)
        cinema.displayShowtimeSeatLayout(showtimeId);

        // Step 3: Pick customer
        cinema.displayCustomers();
        std::string input = readLine("\nCustomer ID (or phone number to search): ");
        std::string customerId = input;

        // If input looks like a phone number (starts with digit or +), search by phone
        if (!input.empty() && (std::isdigit(static_cast<unsigned char>(input[0])) || input[0] == '+')) {
            auto found = cinema.findCustomerByPhone(input);
            if (found.has_value()) {
                found->display();
                customerId = found->getId();
            } else {
                std::cout << "[ERROR] No customer with phone: " << input << "\n";
                return;
            }
        }

        // Step 4: Seat selection
        std::string seat = readLine("Seat (e.g. A1): ");

        // Step 5: Ticket type
        std::string ticketType = getTicketTypeInput();

        // Step 6: Discount
        cinema.displayDiscounts();
        std::string discountCode = readLine("Discount code (press Enter to skip): ");
        if (discountCode.empty()) discountCode = "NONE";

        // Step 7: Sell
        cinema.sellTicket(showtimeId, customerId, seat, ticketType, discountCode);

    } catch (const std::exception& e) {
        std::cout << "[ERROR] " << e.what() << "\n";
    }
}

// ============================================================
// Handler: View Tickets
// ============================================================

void handleViewTickets(CinemaSystem& cinema) {
    cinema.displayTickets();
    std::string ticketId = readLine("\nEnter ticket ID for detailed view (or press Enter to skip): ");
    if (!ticketId.empty()) cinema.displayTicket(ticketId);
}

// ============================================================
// Handler: Revenue Report
// ============================================================

void handleRevenueReport(CinemaSystem& cinema) {
    cinema.displayRevenueReport();
}

} // anonymous namespace

// ============================================================
// main()
// ============================================================

int main() {
    CinemaSystem cinema;
    const std::string dataDir = "data";

    // Auto-load on startup
    if (cinema.loadAll(dataDir))
        std::cout << "[OK] Data loaded from '" << dataDir << "/'.\n";
    else
        std::cout << "[WARN] Some data files could not be loaded (they may be created later).\n";

    while (true) {
        showMainMenu();

        int mainChoice;
        if (!(std::cin >> mainChoice)) {
            std::cin.clear();
            clearInputBuffer();
            std::cout << "[ERROR] Invalid choice.\n";
            continue;
        }
        clearInputBuffer();

        try {
            switch (mainChoice) {
                case 1: handleMovieManagement(cinema);    break;
                case 2: handleRoomManagement(cinema);     break;
                case 3: handleShowtimeManagement(cinema); break;
                case 4: handleCustomerManagement(cinema); break;
                case 5: handleSellTicket(cinema);         break;
                case 6: handleViewTickets(cinema);        break;
                case 7: handleRevenueReport(cinema);      break;
                case 8:
                    if (cinema.saveAll(dataDir))
                        std::cout << "[OK] All data saved to '" << dataDir << "/'.\n";
                    else
                        std::cout << "[ERROR] Some data failed to save.\n";
                    break;
                case 9:
                    cinema.loadAll(dataDir);
                    std::cout << "[OK] All data reloaded.\n";
                    break;
                case 0:
                    cinema.saveAll(dataDir);
                    std::cout << "[OK] Data saved. Goodbye.\n";
                    return 0;
                default:
                    std::cout << "[ERROR] Invalid choice. Please enter 0-9.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
    }

    return 0;
}