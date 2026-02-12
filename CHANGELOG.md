# Changelog

All notable changes to the MoneyTracker project are documented in this file.

## [1.1.0] - 2026-02-12

### Added

**Professional-Grade Infrastructure**
- Modular CMake build with `moneytracker_core` static library
- GitHub Actions CI/CD pipeline with build, test, and validation steps
- Unit testing framework (GoogleTest) with initial test coverage
- Structured logging using spdlog with centralized Logger abstraction
- Code formatting and linting configuration (clang-format, clang-tidy)
- CONTRIBUTING.md with development guidelines and code standards
- Sanitizer support (AddressSanitizer, UndefinedBehaviorSanitizer)

### Changed

- Replaced ad-hoc logging with centralized spdlog-based Logger
- Updated file-opening mechanism to use g_spawn_command_line_async (safer than system())
- Modernized CMake configuration with target-based includes and compile options
- Enhanced install() targets with CMake export support

### Fixed

- Removed unused-parameter warnings in MoneyTrackerGUI callbacks
- Fixed spreadsheet generator warnings for unused parameters

### Workflow

- CI enforces code formatting with clang-format
- CI runs optional clang-tidy checks for code quality
- CI includes unit test execution
- CI builds and tests with sanitizers (non-fatal)

## [1.0.0] - 2025-12-01

### Initial Release

- Base CSV parsing and transaction data management
- Budget analysis and categorization
- Excel spreadsheet generation
- CLI application
- GTK3 GUI application
- Configuration system via categories.json

