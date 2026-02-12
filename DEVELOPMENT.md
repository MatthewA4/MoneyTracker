# Development Guide

Welcome! This guide covers building, testing, and releasing MoneyTracker.

## Quick Start

```bash
cd MoneyTracker
mkdir -p build
cd build
cmake -G Ninja ..
ninja
./moneytracker-gui  # Or ./moneytracker for CLI
```

## Running Tests

```bash
cd build
ctest --output-on-failure -j1
# Or directly: ./run_tests
```

## Building with Sanitizers

Catch memory/undefined-behavior issues at runtime:

```bash
mkdir -p build-sanitizers
cd build-sanitizers
cmake -G Ninja -DENABLE_SANITIZERS=ON -DBUILD_TESTS=ON ..
ninja
ctest --output-on-failure
```

## Code Quality Checks

### Formatting

```bash
# Check code formatting (CI enforces this)
clang-format -style=file -i src/*.cpp include/*.h

# Verify no changes would be made
git diff --exit-code || echo "Code needs formatting"
```

### Static Analysis

```bash
# Run clang-tidy (non-fatal, informational)
clang-tidy $(git ls-files 'src/*.cpp' 'include/*.h') -- -Iinclude -std=gnu++17
```

## Architecture

- **moneytracker_core**: Static library with parsing, analysis, and export logic
- **moneytracker**: CLI application linked against core
- **moneytracker-gui**: GTK3 GUI application linked against core
- **run_tests**: Unit tests linked against core

## Key Components

| Module           | Purpose                                    |
| ---------------- | ------------------------------------------ |
| CSVParser        | Multi-format CSV parsing with auto-detect |
| TransactionData  | In-memory transaction storage              |
| BudgetAnalyzer   | Financial analysis and categorization      |
| ConfigManager    | Category rules from JSON config            |
| SpreadsheetGen   | Excel XLSX export with formatting          |
| Logger           | Structured spdlog-based logging            |
| Security         | File I/O safety and data redaction         |

## Debugging

### Enable verbose logging

Modify Logger initialization in main.cpp:
```cpp
mt::Logger::setLevel(LogLevel::DEBUG);  // or use environment variable
```

### Run with Address Sanitizer

```bash
mkdir build-asan
cd build-asan
cmake -G Ninja -DENABLE_SANITIZERS=ON ..
ninja
ASAN_OPTIONS=halt_on_error=1 ./run_tests
```

## Making a Release

```bash
# Ensure you're on main/master with no uncommitted changes
./scripts/release.sh 1.2.0

# This will:
# 1. Update CMakeLists.txt version
# 2. Commit and create git tag
# 3. Print push instructions

# Then push:
git push origin v1.2.0

# GitHub Actions will automatically:
# 1. Build release artifacts
# 2. Create SHA256 checksums
# 3. Upload to GitHub Releases
```

## CI/CD Pipeline

The GitHub Actions workflow runs on every push and PR:

1. **Build** - CMake + Ninja on Ubuntu
2. **Format Check** - Enforces clang-format compliance
3. **Lint** - Runs clang-tidy (non-fatal)
4. **Tests** - Runs unit suite with CTest
5. **Sanitizers** - Builds with ASAN/UBSAN (optional)

## Dependencies

See CONTRIBUTING.md for installation instructions on your platform.

- Boost 1.70+
- libxlsxwriter
- GTK3 (optional, for GUI)
- CMake 3.12+
- Ninja (or Make)
- spdlog (auto-downloaded by CMake)
- GoogleTest (auto-downloaded by CMake if BUILD_TESTS=ON)

## Security

Read SECURITY.md for:
- How to report vulnerabilities
- Security practices and checks
- Supported versions
- Input validation and file safety

## Contributing

Before submitting a PR:

1. Fork and create a feature branch
2. Make changes and add tests
3. Run `clang-format -style=file -i` on your files
4. Ensure all tests pass: `ctest --output-on-failure`
5. Push and open a PR

See CONTRIBUTING.md for detailed guidelines.

## License

This project is licensed under the MIT License. See LICENSE for details.
