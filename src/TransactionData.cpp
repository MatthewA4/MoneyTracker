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
//TransactionData.cpp

#include "TransactionData.h"
#include <algorithm>
#include <numeric>

TransactionData::TransactionData() {}

void TransactionData::addTransaction(const Transaction& transaction) {
    transactions.push_back(transaction);
}

void TransactionData::addTransactions(const std::vector<Transaction>& trans) {
    transactions.insert(transactions.end(), trans.begin(), trans.end());
}

const std::vector<Transaction>& TransactionData::getAllTransactions() const {
    return transactions;
}

std::vector<Transaction> TransactionData::getTransactionsByCategory(const std::string& category) const {
    std::vector<Transaction> result;
    std::copy_if(transactions.begin(), transactions.end(), 
                 std::back_inserter(result),
                 [&category](const Transaction& t) { return t.category == category; });
    return result;
}

std::vector<Transaction> TransactionData::getTransactionsByAccount(const std::string& accountName) const {
    std::vector<Transaction> result;
    std::copy_if(transactions.begin(), transactions.end(), 
                 std::back_inserter(result),
                 [&accountName](const Transaction& t) { return t.accountName == accountName; });
    return result;
}

std::vector<Transaction> TransactionData::getTransactionsByDateRange(const std::string& startDate, const std::string& endDate) const {
    std::vector<Transaction> result;
    std::copy_if(transactions.begin(), transactions.end(), 
                 std::back_inserter(result),
                 [&startDate, &endDate](const Transaction& t) { 
                     return t.date >= startDate && t.date <= endDate; 
                 });
    return result;
}

double TransactionData::getTotalSpending() const {
    return std::accumulate(transactions.begin(), transactions.end(), 0.0,
                          [](double sum, const Transaction& t) {
                              return sum + (t.amount < 0 ? t.amount : 0);
                          });
}

double TransactionData::getTotalSpendingByCategory(const std::string& category) const {
    auto categoryTrans = getTransactionsByCategory(category);
    return std::accumulate(categoryTrans.begin(), categoryTrans.end(), 0.0,
                          [](double sum, const Transaction& t) {
                              return sum + (t.amount < 0 ? t.amount : 0);
                          });
}

double TransactionData::getAverageTransaction() const {
    if (transactions.empty()) return 0.0;
    double total = std::accumulate(transactions.begin(), transactions.end(), 0.0,
                                   [](double sum, const Transaction& t) { return sum + std::abs(t.amount); });
    return total / transactions.size();
}

std::map<std::string, double> TransactionData::getCategoryTotals() const {
    std::map<std::string, double> result;
    for (const auto& transaction : transactions) {
        result[transaction.category] += (transaction.amount < 0 ? transaction.amount : 0);
    }
    return result;
}

std::string TransactionData::extractMonth(const std::string& date) const {
    // Assumes date format: YYYY-MM-DD
    if (date.length() >= 7) {
        return date.substr(0, 7);
    }
    return date;
}

std::map<std::string, double> TransactionData::getMonthlyTotals() const {
    std::map<std::string, double> result;
    for (const auto& transaction : transactions) {
        std::string month = extractMonth(transaction.date);
        result[month] += (transaction.amount < 0 ? transaction.amount : 0);
    }
    return result;
}

std::vector<std::string> TransactionData::getUniqueCategories() const {
    std::vector<std::string> categories;
    std::vector<std::string> result;
    
    for (const auto& transaction : transactions) {
        categories.push_back(transaction.category);
    }
    
    std::sort(categories.begin(), categories.end());
    categories.erase(std::unique(categories.begin(), categories.end()), categories.end());
    
    return categories;
}

std::vector<std::string> TransactionData::getUniqueAccounts() const {
    std::vector<std::string> accounts;
    
    for (const auto& transaction : transactions) {
        accounts.push_back(transaction.accountName);
    }
    
    std::sort(accounts.begin(), accounts.end());
    accounts.erase(std::unique(accounts.begin(), accounts.end()), accounts.end());
    
    return accounts;
}
