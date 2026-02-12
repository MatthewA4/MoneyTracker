# Contributing Guidelines

This document describes coding style and development workflow for the MoneyTracker project.

Code style
- Follow the `.clang-format` file in the repository. Run `clang-format -style=file -i <files>` before committing.
- Use modern C++ (C++17). Prefer `std::string_view`, `constexpr`, `auto` with meaningful names, and RAII.
- Avoid raw `new`/`delete`; prefer smart pointers.

Static analysis & tests
- Run unit tests with CMake: `cmake -S . -B build -DBUILD_TESTS=ON && cmake --build build && ctest --test-dir build`.
- Run `clang-tidy` locally to catch style and correctness issues.

Commit messages
- Use short (50 char) summary, blank line, then detailed description.

Pull requests
- Include a description of changes, testing performed, and any migration notes.
- Ensure CI passes before merging.
