//Copyright (C) 2026 Matthew Anderson
//MIT License

#pragma once

#include <string>
#include <stdexcept>

class DateParser {
public:
    // Parse date from various formats
    // Supports: MM/DD/YYYY, DD/MM/YYYY, YYYY-MM-DD, MM-DD-YYYY
    static std::string parse(const std::string& dateStr);
    
    // Normalize to YYYY-MM-DD format
    static std::string normalize(const std::string& dateStr);
    
    // Check if date string is valid
    static bool isValid(const std::string& dateStr);
    
    // Extract month from normalized date (YYYY-MM)
    static std::string extractMonth(const std::string& dateStr);
    
private:
    static int detectFormat(const std::string& dateStr);
    static bool isValidDate(int year, int month, int day);
};
