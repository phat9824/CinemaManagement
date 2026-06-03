cmake_minimum_required(VERSION 3.14)
project(CinemaManagementMini)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# 1. Thu thập logic (loại bỏ main.cpp để tránh xung đột với Test)
file(GLOB SOURCES "src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# 2. Build chương trình chính
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

# 3. Cấu hình GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# 4. Build Test
enable_testing()
include(GoogleTest)
add_executable(test_core tests/test_core.cpp ${SOURCES})
target_link_libraries(test_core PRIVATE GTest::gtest_main)
gtest_discover_tests(test_core)
