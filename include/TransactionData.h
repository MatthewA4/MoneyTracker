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
//TransactionData.h

#pragma once

#include "CSVParser.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

class TransactionData {
public:
    TransactionData();
    ~TransactionData() = default;
    
    void addTransaction(const Transaction& transaction);
    void addTransactions(const std::vector<Transaction>& transactions);
    
    const std::vector<Transaction>& getAllTransactions() const;
    std::vector<Transaction> getTransactionsByCategory(const std::string& category) const;
    std::vector<Transaction> getTransactionsByDateRange(const std::string& startDate, const std::string& endDate) const;
    std::vector<Transaction> getTransactionsByAccount(const std::string& accountName) const;
    
    double getTotalSpending() const;
    double getTotalSpendingByCategory(const std::string& category) const;
    double getAverageTransaction() const;
    
    std::map<std::string, double> getCategoryTotals() const;
    std::map<std::string, double> getMonthlyTotals() const;
    std::vector<std::string> getUniqueCategories() const;
    std::vector<std::string> getUniqueAccounts() const;
    
private:
    std::vector<Transaction> transactions;
    
    std::string extractMonth(const std::string& date) const;
};
