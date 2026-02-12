cmake_minimum_required(VERSION 3.12)
project(MoneyTracker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Sanitizer options
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(ENABLE_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

# Find required packages
find_package(Boost 1.70.0 REQUIRED COMPONENTS system filesystem program_options)

# Find pkg-config
find_package(PkgConfig REQUIRED)

# Optional: FetchContent for external header-only or small deps
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.11.0
)
FetchContent_MakeAvailable(spdlog)

option(ENABLE_SANITIZERS "Enable Address/Undefined sanitizers" OFF)

# Find xlsxwriter using pkg-config or manual search
find_library(XLSXWRITER_LIBRARY xlsxwriter)
find_path(XLSXWRITER_INCLUDE_DIR xlsxwriter.h)

if(NOT XLSXWRITER_LIBRARY OR NOT XLSXWRITER_INCLUDE_DIR)
    message(FATAL_ERROR "xlsxwriter library not found. Please install libxlsxwriter-dev")
endif()

# Find GTK3 for GUI using pkg-config
pkg_check_modules(GTK3 QUIET gtk+-3.0)

# Note: include directories are set per-target using `target_include_directories` below

# Source files for core library
set(CORE_SOURCES
    src/CSVParser.cpp
    src/TransactionData.cpp
    src/BudgetAnalyzer.cpp
    src/SpreadsheetGenerator.cpp
    src/DateParser.cpp
    src/ConfigManager.cpp
    src/AlertSystem.cpp
    src/Logger.cpp
)

# Create a reusable core library for the project
add_library(moneytracker_core STATIC ${CORE_SOURCES})
target_include_directories(moneytracker_core
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE ${XLSXWRITER_INCLUDE_DIR}
)
target_compile_features(moneytracker_core PUBLIC cxx_std_17)
target_compile_options(moneytracker_core PRIVATE -Wall -Wextra)
target_link_libraries(moneytracker_core
    PUBLIC
    Boost::system
    Boost::filesystem
    Boost::program_options
    ${XLSXWRITER_LIBRARY}
    PUBLIC spdlog::spdlog
)

if(ENABLE_SANITIZERS)
    target_compile_options(moneytracker_core PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(moneytracker_core PRIVATE -fsanitize=address,undefined)
endif()

# When sanitizers are enabled, ensure executables also link the sanitizer runtimes
if(ENABLE_SANITIZERS)
    if(TARGET moneytracker)
        target_compile_options(moneytracker PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(moneytracker PRIVATE -fsanitize=address,undefined)
    endif()
    if(TARGET moneytracker-gui)
        target_compile_options(moneytracker-gui PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(moneytracker-gui PRIVATE -fsanitize=address,undefined)
    endif()
    if(TARGET run_tests)
        target_compile_options(run_tests PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(run_tests PRIVATE -fsanitize=address,undefined)
    endif()
endif()

# CLI Application
add_executable(moneytracker src/main.cpp)
target_link_libraries(moneytracker PRIVATE moneytracker_core)

# GUI Application (if GTK3 is available)
if(GTK3_FOUND)
    message(STATUS "GTK3 found - building GUI application")
    
    add_executable(moneytracker-gui src/main_gui.cpp src/MoneyTrackerGUI.cpp)
    target_link_libraries(moneytracker-gui PRIVATE moneytracker_core ${GTK3_LIBRARIES})
    target_include_directories(moneytracker-gui PRIVATE ${GTK3_INCLUDE_DIRS})
    install(TARGETS moneytracker-gui RUNTIME DESTINATION bin)
else()
    message(WARNING "GTK3 not found - skipping GUI application (install libgtk-3-dev to enable)")
endif()

# Installation targets
install(TARGETS moneytracker
    RUNTIME DESTINATION bin
)

# Install core library (for packaging)
install(TARGETS moneytracker_core
    ARCHIVE DESTINATION lib
)

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
    
    # Create test executable (link core sources so tests can use project code)
    add_executable(run_tests ${TEST_SOURCES} ${CORE_SOURCES})
    target_link_libraries(run_tests
        PRIVATE
        gtest
        gtest_main
        Boost::system
        Boost::filesystem
        Boost::program_options
        ${XLSXWRITER_LIBRARY}
        spdlog::spdlog
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

# Optional: Add test executable if tests exist (only when not using BUILD_TESTS)
if(NOT BUILD_TESTS)
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
endif()
