//Copyright (C) 2026 Matthew Anderson
//MIT License

#pragma once

#include <stdexcept>
#include <string>

// Input validation macros for security
#define VALIDATE_NOT_EMPTY(str, msg)                                                               \
    if ((str).empty()) {                                                                            \
        throw std::invalid_argument("Validation failed: " msg);                                    \
    }

#define VALIDATE_FILE_PATH(path)                                                                   \
    if ((path).empty() || (path).find("..") != std::string::npos) {                                \
        throw std::invalid_argument("Invalid file path: " path);                                   \
    }

#define VALIDATE_RANGE(value, min, max, name)                                                     \
    if ((value) < (min) || (value) > (max)) {                                                      \
        throw std::out_of_range(name " out of valid range");                                       \
    }

// Noexcept specification helpers
#define NOEXCEPT_IF(condition) noexcept(condition)

namespace mt {
// Simple result type for error propagation without exceptions
template <typename T, typename E = std::string>
class Result {
public:
    static Result success(T value) { return Result(std::move(value), true, E()); }
    static Result error(E err) { return Result(T(), false, std::move(err)); }

    bool is_ok() const { return ok_; }
    bool is_err() const { return !ok_; }

    T& unwrap() {
        if (!ok_) throw std::runtime_error("Called unwrap on error result: " + error_);
        return value_;
    }

    const T& unwrap() const {
        if (!ok_) throw std::runtime_error("Called unwrap on error result: " + error_);
        return value_;
    }

    const E& error() const { return error_; }

private:
    Result(T val, bool ok, E err) : value_(std::move(val)), ok_(ok), error_(std::move(err)) {}

    T value_;
    bool ok_;
    E error_;
};
} // namespace mt
