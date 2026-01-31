cmake_minimum_required(VERSION 3.12)
project(MoneyTracker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Find required packages
find_package(Boost 1.70.0 REQUIRED COMPONENTS system filesystem program_options)

# Find xlsxwriter using pkg-config or manual search
find_library(XLSXWRITER_LIBRARY xlsxwriter)
find_path(XLSXWRITER_INCLUDE_DIR xlsxwriter.h)

if(NOT XLSXWRITER_LIBRARY OR NOT XLSXWRITER_INCLUDE_DIR)
    message(FATAL_ERROR "xlsxwriter library not found. Please install libxlsxwriter-dev")
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${XLSXWRITER_INCLUDE_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/CSVParser.cpp
    src/TransactionData.cpp
    src/BudgetAnalyzer.cpp
    src/SpreadsheetGenerator.cpp
)

# Create executable
add_executable(moneytracker ${SOURCES})

# Link libraries
target_link_libraries(moneytracker
    PRIVATE
    Boost::system
    Boost::filesystem
    Boost::program_options
    ${XLSXWRITER_LIBRARY}
)

# Installation targets
install(TARGETS moneytracker DESTINATION bin)

# Install documentation
install(FILES README.md DESTINATION share/doc/moneytracker)
install(FILES GETTING_STARTED.md DESTINATION share/doc/moneytracker)
install(FILES PROJECT_SUMMARY.md DESTINATION share/doc/moneytracker)

# Install sample data
install(FILES data/sample_bank.csv DESTINATION share/moneytracker/examples)
install(FILES data/sample_savings.csv DESTINATION share/moneytracker/examples)

# Configure uninstall script
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
        IMMEDIATE @ONLY)
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
endif()

# Enable testing
enable_testing()

# Optional: Add test executable if tests exist
file(GLOB TEST_SOURCES "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(moneytracker_tests ${TEST_SOURCES} ${SOURCES})
    target_link_libraries(moneytracker_tests
        PRIVATE
        Boost::system
        Boost::filesystem
        Boost::program_options
        ${XLSXWRITER_LIBRARY}
    )
    add_test(NAME MoneyTrackerTests COMMAND moneytracker_tests)
endif()
