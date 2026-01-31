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
//CSVParser.h

#pragma once

#include <string>
#include <vector>
#include <memory>

struct Transaction {
    std::string date;
    std::string description;
    std::string category;
    double amount;
    double balance;
    std::string accountName;
    
    Transaction() : amount(0.0), balance(0.0) {}
};

class CSVParser {
public:
    CSVParser();
    ~CSVParser() = default;
    
    // Parse bank format CSV (debit/credit columns)
    std::vector<Transaction> parseBank(const std::string& filePath, const std::string& accountName);
    
    // Parse generic CSV format
    std::vector<Transaction> parseGeneric(const std::string& filePath, const std::string& accountName);
    
    // Auto-detect and parse CSV
    std::vector<Transaction> parse(const std::string& filePath, const std::string& accountName);
    
private:
    std::vector<std::string> splitLine(const std::string& line, char delimiter = ',');
    std::string trim(const std::string& str);
    double parseAmount(const std::string& amount);
    std::string categorizeTransaction(const std::string& description);
};
