cmake_minimum_required(VERSION 3.12)
project(MoneyTracker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Find required packages
find_package(Boost 1.70.0 REQUIRED COMPONENTS system filesystem program_options)

# Find pkg-config
find_package(PkgConfig REQUIRED)

# Find xlsxwriter using pkg-config or manual search
find_library(XLSXWRITER_LIBRARY xlsxwriter)
find_path(XLSXWRITER_INCLUDE_DIR xlsxwriter.h)

if(NOT XLSXWRITER_LIBRARY OR NOT XLSXWRITER_INCLUDE_DIR)
    message(FATAL_ERROR "xlsxwriter library not found. Please install libxlsxwriter-dev")
endif()

# Find GTK3 for GUI using pkg-config
pkg_check_modules(GTK3 QUIET gtk+-3.0)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${XLSXWRITER_INCLUDE_DIR})

# Source files for core library
set(CORE_SOURCES
    src/CSVParser.cpp
    src/TransactionData.cpp
    src/BudgetAnalyzer.cpp
    src/SpreadsheetGenerator.cpp
    src/DateParser.cpp
    src/ConfigManager.cpp
    src/AlertSystem.cpp
)

# CLI Application
set(CLI_SOURCES
    src/main.cpp
    ${CORE_SOURCES}
)

add_executable(moneytracker ${CLI_SOURCES})

target_link_libraries(moneytracker
    PRIVATE
    Boost::system
    Boost::filesystem
    Boost::program_options
    ${XLSXWRITER_LIBRARY}
)

# GUI Application (if GTK3 is available)
if(GTK3_FOUND)
    message(STATUS "GTK3 found - building GUI application")
    
    set(GUI_SOURCES
        src/main_gui.cpp
        src/MoneyTrackerGUI.cpp
        ${CORE_SOURCES}
    )
    
    add_executable(moneytracker-gui ${GUI_SOURCES})
    
    target_link_libraries(moneytracker-gui
        PRIVATE
        Boost::system
        Boost::filesystem
        Boost::program_options
        ${XLSXWRITER_LIBRARY}
        ${GTK3_LIBRARIES}
    )
    
    target_include_directories(moneytracker-gui PRIVATE ${GTK3_INCLUDE_DIRS})
    
    install(TARGETS moneytracker-gui DESTINATION bin)
else()
    message(WARNING "GTK3 not found - skipping GUI application (install libgtk-3-dev to enable)")
endif()

# Installation targets
install(TARGETS moneytracker DESTINATION bin)

# Install documentation
install(FILES README.md DESTINATION share/doc/moneytracker)
install(FILES GETTING_STARTED.md DESTINATION share/doc/moneytracker)
install(FILES PROJECT_SUMMARY.md DESTINATION share/doc/moneytracker)
install(FILES IMPROVEMENTS.md DESTINATION share/doc/moneytracker)

# Install data files
install(FILES data/categories.json DESTINATION share/moneytracker)

# Enable testing
enable_testing()

# Google Test integration (optional - disabled by default due to FetchContent download)
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_dateparser.cpp")
    # Download and configure GoogleTest
    include(FetchContent)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP ON
    )
    FetchContent_MakeAvailable(googletest)
    
    # Test source files
    set(TEST_SOURCES
        tests/test_dateparser.cpp
        tests/test_csvparser.cpp
        tests/test_configmanager.cpp
    )
    
    # Create test executable
    add_executable(run_tests ${TEST_SOURCES})
    target_link_libraries(run_tests
        PRIVATE
        gtest
        gtest_main
        Boost::system
        Boost::filesystem
        Boost::program_options
        ${XLSXWRITER_LIBRARY}
    )
    target_include_directories(run_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
    add_test(NAME UnitTests COMMAND run_tests)
endif()



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
