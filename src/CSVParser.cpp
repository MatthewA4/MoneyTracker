//Copyright (C) 2026 Matthew Anderson
//MIT License
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
//CSVParser.cpp

#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

CSVParser::CSVParser() {}

std::vector<std::string> CSVParser::splitLine(const std::string& line, char delimiter) {
    std::vector<std::string> result;
    std::string item;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == delimiter && !inQuotes) {
            result.push_back(trim(item));
            item.clear();
        } else {
            item += c;
        }
    }
    result.push_back(trim(item));
    
    return result;
}

std::string CSVParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\"\0");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r\"\0");
    return str.substr(first, (last - first + 1));
}

double CSVParser::parseAmount(const std::string& amount) {
    std::string cleaned = amount;
    // Remove commas and dollar signs
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), ','), cleaned.end());
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '$'), cleaned.end());
    cleaned = trim(cleaned);
    
    try {
        return std::stod(cleaned);
    } catch (...) {
        return 0.0;
    }
}

std::string CSVParser::categorizeTransaction(const std::string& description) {
    std::string desc = description;
    std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
    
    // Simple categorization logic
    if (desc.find("grocery") != std::string::npos || desc.find("safeway") != std::string::npos ||
        desc.find("trader") != std::string::npos || desc.find("whole foods") != std::string::npos) {
        return "Groceries";
    }
    if (desc.find("gas") != std::string::npos || desc.find("fuel") != std::string::npos ||
        desc.find("shell") != std::string::npos || desc.find("chevron") != std::string::npos) {
        return "Gas";
    }
    if (desc.find("restaurant") != std::string::npos || desc.find("cafe") != std::string::npos ||
        desc.find("pizza") != std::string::npos || desc.find("burger") != std::string::npos) {
        return "Dining";
    }
    if (desc.find("amazon") != std::string::npos || desc.find("ebay") != std::string::npos ||
        desc.find("walmart") != std::string::npos || desc.find("target") != std::string::npos) {
        return "Shopping";
    }
    if (desc.find("netflix") != std::string::npos || desc.find("spotify") != std::string::npos ||
        desc.find("hulu") != std::string::npos || desc.find("subscription") != std::string::npos) {
        return "Subscriptions";
    }
    if (desc.find("deposit") != std::string::npos || desc.find("transfer") != std::string::npos ||
        desc.find("xfer") != std::string::npos) {
        return "Transfers";
    }
    
    return "Other";
}

std::vector<Transaction> CSVParser::parseBank(const std::string& filePath, const std::string& accountName) {
    std::vector<Transaction> transactions;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    
    std::string line;
    int lineNumber = 0;
    
    // Skip header lines
    while (std::getline(file, line) && lineNumber < 2) {
        lineNumber++;
    }
    
    // Bank format: Date,Description,Debit,Credit,Balance
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        auto parts = splitLine(line);
        if (parts.size() < 5) continue;
        
        Transaction transaction;
        transaction.date = parts[0];
        transaction.description = parts[1];
        
        // Parse debit/credit
        double debit = parseAmount(parts[2]);
        double credit = parseAmount(parts[3]);
        transaction.amount = (debit > 0) ? -debit : credit;
        
        transaction.balance = parseAmount(parts[4]);
        transaction.category = categorizeTransaction(transaction.description);
        transaction.accountName = accountName;
        
        transactions.push_back(transaction);
    }
    
    file.close();
    return transactions;
}

std::vector<Transaction> CSVParser::parseGeneric(const std::string& filePath, const std::string& accountName) {
    std::vector<Transaction> transactions;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        lineNumber++;
        
        // Skip header lines (lines starting with Date or Column names)
        if (lineNumber == 1 && (line.find("Date") != std::string::npos || 
                                line.find("Posting Date") != std::string::npos ||
                                line.find("Transaction") != std::string::npos)) {
            continue;
        }
        
        auto parts = splitLine(line);
        if (parts.size() < 2) continue;  // Need at least date and amount
        
        Transaction transaction;
        
        // Try to parse date (first column)
        transaction.date = parts[0];
        
        // Look for amount - usually in 2nd or 3rd column
        // For various formats: Date, Amount, Description OR Date, Description, Amount
        double amount = 0.0;
        std::string description;
        
        if (parts.size() >= 3) {
            // Try column 2 as amount first
            amount = parseAmount(parts[1]);
            if (amount != 0.0 || parts[1].find_first_of("-0123456789.$") != std::string::npos) {
                // Column 2 is amount
                description = (parts.size() > 2) ? parts[2] : "";
            } else {
                // Column 2 is not amount, try column 3
                amount = parseAmount(parts[2]);
                description = parts[1];
            }
        } else if (parts.size() == 2) {
            // Only date and amount
            amount = parseAmount(parts[1]);
            description = "";
        }
        
        transaction.amount = amount;
        transaction.description = description;
        
        // Get balance if available (usually last column)
        if (parts.size() >= 4) {
            transaction.balance = parseAmount(parts[parts.size() - 1]);
        }
        
        transaction.category = categorizeTransaction(transaction.description);
        transaction.accountName = accountName;
        
        transactions.push_back(transaction);
    }
    
    file.close();
    return transactions;
}

std::vector<Transaction> CSVParser::parse(const std::string& filePath, const std::string& accountName) {
    // Try to auto-detect format by reading first few lines
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    
    std::string firstLine, secondLine;
    std::getline(file, firstLine);
    std::getline(file, secondLine);
    file.close();
    
    // Check if it looks like bank format (with Debit/Credit columns)
    if (firstLine.find("Date") != std::string::npos && firstLine.find("Debit") != std::string::npos) {
        return parseBank(filePath, accountName);
    }
    
    // Default to generic format
    return parseGeneric(filePath, accountName);
}
