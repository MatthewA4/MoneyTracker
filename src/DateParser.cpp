//Copyright (C) 2026 Matthew Anderson
//MIT License

#include "DateParser.h"
#include <sstream>
#include <regex>
#include <algorithm>

std::string DateParser::parse(const std::string& dateStr) {
    if (!isValid(dateStr)) {
        throw std::invalid_argument("Invalid date format: " + dateStr);
    }
    return normalize(dateStr);
}

std::string DateParser::normalize(const std::string& dateStr) {
    // Detect format and parse accordingly
    int format = detectFormat(dateStr);
    
    int year = 0, month = 0, day = 0;
    std::string cleanDate = dateStr;
    
    // Remove common separators and normalize
    std::replace(cleanDate.begin(), cleanDate.end(), '-', '/');
    
    std::istringstream iss(cleanDate);
    std::string part1, part2, part3;
    
    // Split by /
    if (std::getline(iss, part1, '/') && std::getline(iss, part2, '/') && std::getline(iss, part3, '/')) {
        try {
            int p1 = std::stoi(part1);
            int p2 = std::stoi(part2);
            int p3 = std::stoi(part3);
            
            if (format == 0) {  // MM/DD/YYYY
                month = p1;
                day = p2;
                year = p3;
            } else if (format == 1) {  // DD/MM/YYYY
                day = p1;
                month = p2;
                year = p3;
            } else if (format == 2) {  // YYYY/MM/DD
                year = p1;
                month = p2;
                day = p3;
            }
        } catch (...) {
            throw std::invalid_argument("Could not parse date components: " + dateStr);
        }
    }
    
    if (!isValidDate(year, month, day)) {
        throw std::invalid_argument("Invalid date values: " + dateStr);
    }
    
    // Return in YYYY-MM-DD format
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
    return std::string(buffer);
}

bool DateParser::isValid(const std::string& dateStr) {
    if (dateStr.empty()) return false;
    
    // Check if matches any known pattern
    std::regex pattern1(R"(^(\d{1,2})[/-](\d{1,2})[/-](\d{4})$)");  // MM/DD/YYYY or DD/MM/YYYY
    std::regex pattern2(R"(^(\d{4})[/-](\d{1,2})[/-](\d{1,2})$)");  // YYYY-MM-DD
    
    return std::regex_match(dateStr, pattern1) || std::regex_match(dateStr, pattern2);
}

std::string DateParser::extractMonth(const std::string& dateStr) {
    if (!isValid(dateStr)) {
        throw std::invalid_argument("Invalid date format: " + dateStr);
    }
    
    std::string normalized = normalize(dateStr);
    // Return first 7 characters: YYYY-MM
    return normalized.substr(0, 7);
}

int DateParser::detectFormat(const std::string& dateStr) {
    // Try to determine if it's MM/DD/YYYY or DD/MM/YYYY or YYYY/MM/DD
    std::string cleanDate = dateStr;
    std::replace(cleanDate.begin(), cleanDate.end(), '-', '/');
    
    std::istringstream iss(cleanDate);
    std::string part1, part2, part3;
    
    if (std::getline(iss, part1, '/') && std::getline(iss, part2, '/') && std::getline(iss, part3, '/')) {
        try {
            int p1 = std::stoi(part1);
            int p2 = std::stoi(part2);
            
            // If first part is 4 digits, it's YYYY-MM-DD format
            if (part1.length() == 4) {
                return 2;
            }
            
            // If first part > 12, it must be DD/MM/YYYY
            if (p1 > 12) {
                return 1;
            }
            
            // If second part > 12, it must be MM/DD/YYYY
            if (p2 > 12) {
                return 0;
            }
            
            // Default to MM/DD/YYYY (US format)
            return 0;
        } catch (...) {
            return 0;  // Default to MM/DD/YYYY
        }
    }
    
    return 0;  // Default format
}

bool DateParser::isValidDate(int year, int month, int day) {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    // Days per month (non-leap year)
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeapYear) {
        daysInMonth[2] = 29;
    }
    
    return day <= daysInMonth[month];
}
