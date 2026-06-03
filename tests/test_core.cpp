#include <gtest/gtest.h>
#include "CinemaSystem.hpp"
#include "Movie.hpp"
#include "Room.hpp"
#include "Showtime.hpp"
#include "Ticket.hpp"
#include "Customer.hpp"
#include "DiscountPolicy.hpp"
#include <stdexcept>
#include <memory>

class CinemaCoreTest : public ::testing::Test {
protected:
    CinemaSystem cinema;
    std::string c1, c2, c3;
    std::string r1;

    void SetUp() override {
        // Setup initial state for tests
        c1 = cinema.addCustomer("Adult", 25, "0901", MembershipLevel::NORMAL);
        c2 = cinema.addCustomer("Child", 10, "0902", MembershipLevel::NORMAL);
        c3 = cinema.addCustomer("Student", 19, "0903", MembershipLevel::NORMAL);
        
        r1 = cinema.addRoom("Hall 1", RoomType::NORMAL, 5, 8); // 40 seats
    }
};

// 1. Thêm phim thành công
TEST_F(CinemaCoreTest, AddMovieSucceeds) {
    EXPECT_NO_THROW(cinema.addMovie("Dune 2", "Sci-fi", 166, 13));
}

// 2. Thêm phim thời lượng không hợp lệ thì báo lỗi
TEST_F(CinemaCoreTest, AddMovieInvalidDurationThrows) {
    EXPECT_THROW(cinema.addMovie("Copy", "Action", 0, 0), std::runtime_error);
}

// 3. Tạo suất chiếu hợp lệ
TEST_F(CinemaCoreTest, CreateValidShowtimeSucceeds) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    EXPECT_NO_THROW(cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 90000.0));
}

// 4. Tạo suất chiếu bị trùng lịch thì báo lỗi
TEST_F(CinemaCoreTest, RejectConflictingShowtime) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string m2 = cinema.addMovie("Comedy", "Comedy", 100, 0);
    cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 90000.0);
    // Overlapping time in the same room (19:30 + 166m = 22:16) -> 20:00 will conflict
    EXPECT_THROW(cinema.addShowtime(m2, r1, "2026-06-01", "20:00", 70000.0), std::runtime_error);
}

// 5. Bán vé ghế còn trống thì thành công
TEST_F(CinemaCoreTest, SellAvailableSeatSucceeds) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 90000.0);
    EXPECT_NO_THROW(cinema.sellTicket(st1, c1, "A1", "Adult", "NONE"));
}

// 6. Bán vé ghế đã đặt thì báo lỗi
TEST_F(CinemaCoreTest, RejectAlreadyBookedSeat) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 90000.0);
    cinema.sellTicket(st1, c1, "A1", "Adult", "NONE");
    // Selling A1 again should fail
    EXPECT_THROW(cinema.sellTicket(st1, c1, "A1", "Adult", "NONE"), std::runtime_error);
}

// 7. Khách chưa đủ tuổi thì không bán vé
TEST_F(CinemaCoreTest, RejectUnderageCustomer) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 90000.0);
    // C02 (c2) is 10 years old, m1 requires 13+
    EXPECT_THROW(cinema.sellTicket(st1, c2, "A2", "Child", "NONE"), std::runtime_error);
}

// 8. Tính giá vé trẻ em đúng (Giảm 30%)
TEST_F(CinemaCoreTest, ChildTicketPricing) {
    std::string m2 = cinema.addMovie("Minions", "Comedy", 90, 0);
    std::string st2 = cinema.addShowtime(m2, r1, "2026-06-02", "10:00", 100000.0);
    cinema.sellTicket(st2, c2, "A1", "Child", "NONE");
    // 100k * 70% = 70k
    EXPECT_DOUBLE_EQ(cinema.getCustomerTotalSpending(c2), 70000.0);
}

// 9. Tính giá vé sinh viên đúng (Giảm 20%)
TEST_F(CinemaCoreTest, StudentTicketPricing) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 100000.0);
    cinema.sellTicket(st1, c3, "A1", "Student", "NONE");
    // 100k * 80% = 80k
    EXPECT_DOUBLE_EQ(cinema.getCustomerTotalSpending(c3), 80000.0);
}

// 10. Áp dụng mã giảm giá đúng (Giảm 10%)
TEST_F(CinemaCoreTest, ApplyDiscountCorrectly) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 100000.0);
    // Using SALE10 default discount (10% off final price)
    // Adult base = 100k, 100k * 90% = 90k
    cinema.sellTicket(st1, c1, "A1", "Adult", "SALE10");
    EXPECT_DOUBLE_EQ(cinema.getCustomerTotalSpending(c1), 90000.0);
}

// 11. Cập nhật thông tin phim thành công
TEST_F(CinemaCoreTest, UpdateMovieSucceeds) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    EXPECT_TRUE(cinema.updateMovie(m1, "Dune 3", "Action", 180, 16));
    auto res = cinema.searchMoviesByName("Dune 3");
    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0].getDuration(), 180);
}

// 12. Dừng chiếu phim thì không còn trong danh sách đang chiếu
TEST_F(CinemaCoreTest, StopMovieRemovesFromShowing) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    cinema.stopMovie(m1);
    auto showing = cinema.getShowingMovies();
    EXPECT_TRUE(showing.empty());
}

// 13. Tìm kiếm phim theo tên (Không phân biệt hoa thường)
TEST_F(CinemaCoreTest, SearchMovieByName) {
    cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    auto res = cinema.searchMoviesByName("dune");
    EXPECT_EQ(res.size(), 1u);
}

// 14. Lọc phim theo thể loại
TEST_F(CinemaCoreTest, FilterMovieByGenre) {
    cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    auto res = cinema.filterMoviesByGenre("Sci-fi");
    EXPECT_EQ(res.size(), 1u);
}

// 15. Thêm phòng mới thành công
TEST_F(CinemaCoreTest, AddRoomSucceeds) {
    std::string r2 = cinema.addRoom("Hall VIP", RoomType::VIP, 4, 6);
    EXPECT_TRUE(cinema.roomIdExists(r2));
}

// 16. Update phòng (giữ nguyên một số trường bằng cách truyền -1)
TEST_F(CinemaCoreTest, UpdateRoomSucceeds) {
    cinema.updateRoom(r1, "New Hall", -1, 6, 10);
    auto room = cinema.getRoomById(r1);
    EXPECT_EQ(room->getName(), "New Hall");
    EXPECT_EQ(room->getRows(), 6);
}

// 17. Mua vé ghế không tồn tại báo lỗi
TEST_F(CinemaCoreTest, RejectInvalidSeatFormat) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 90000.0);
    EXPECT_THROW(cinema.sellTicket(st1, c1, "Z99", "Adult", "NONE"), std::runtime_error);
}

// 18. Áp dụng giảm giá cố định (-20k)
TEST_F(CinemaCoreTest, ApplyFixedDiscount) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 100000.0);
    cinema.sellTicket(st1, c1, "A1", "Adult", "FIX20K");
    // 100k - 20k = 80k
    EXPECT_DOUBLE_EQ(cinema.getCustomerTotalSpending(c1), 80000.0);
}

// 19. Báo lỗi khi dùng mã giảm giá không tồn tại
TEST_F(CinemaCoreTest, RejectInvalidDiscountCode) {
    std::string m1 = cinema.addMovie("Dune 2", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 100000.0);
    EXPECT_THROW(cinema.sellTicket(st1, c1, "A1", "Adult", "FAKE_CODE"), std::runtime_error);
}

// 20. Doanh thu của khách hàng tích lũy đúng sau nhiều lần mua
TEST_F(CinemaCoreTest, CustomerSpendingAccumulates) {
    std::string m1 = cinema.addMovie("Dune", "Sci-fi", 166, 13);
    std::string st1 = cinema.addShowtime(m1, r1, "2026-06-01", "19:30", 100000.0);
    
    // Ticket 1: No discount (100k)
    cinema.sellTicket(st1, c1, "A1", "Adult", "NONE"); 
    // Ticket 2: 10% discount (90k)
    cinema.sellTicket(st1, c1, "A2", "Adult", "SALE10"); 
    
    EXPECT_DOUBLE_EQ(cinema.getCustomerTotalSpending(c1), 190000.0);
}
