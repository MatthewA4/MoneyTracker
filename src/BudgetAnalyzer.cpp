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
//BudgetAnalyzer.cpp

#include "BudgetAnalyzer.h"
#include <algorithm>
#include <numeric>

BudgetAnalyzer::BudgetAnalyzer(const TransactionData& data) : transactionData(data) {}

BudgetSummary BudgetAnalyzer::analyzeBudget() const {
    BudgetSummary summary;
    
    const auto& transactions = transactionData.getAllTransactions();
    
    summary.totalIncome = 0.0;
    summary.totalExpenses = 0.0;
    
    for (const auto& transaction : transactions) {
        if (transaction.amount > 0) {
            summary.totalIncome += transaction.amount;
        } else {
            summary.totalExpenses += std::abs(transaction.amount);
        }
    }
    
    summary.netChange = summary.totalIncome - summary.totalExpenses;
    
    // Category breakdown
    summary.categoryBreakdown = transactionData.getCategoryTotals();
    
    // Account breakdown
    for (const auto& account : transactionData.getUniqueAccounts()) {
        auto accountTrans = transactionData.getTransactionsByAccount(account);
        double accountTotal = 0.0;
        for (const auto& t : accountTrans) {
            accountTotal += (t.amount < 0 ? t.amount : 0);
        }
        summary.accountBreakdown[account] = accountTotal;
    }
    
    // Monthly trends
    summary.monthlyTrends = transactionData.getMonthlyTotals();
    
    return summary;
}

std::map<std::string, double> BudgetAnalyzer::getTopSpendingCategories(int limit) const {
    auto categoryTotals = transactionData.getCategoryTotals();
    
    std::vector<std::pair<std::string, double>> sorted(categoryTotals.begin(), categoryTotals.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return std::abs(a.second) > std::abs(b.second); });
    
    std::map<std::string, double> result;
    for (int i = 0; i < std::min(limit, (int)sorted.size()); ++i) {
        result[sorted[i].first] = sorted[i].second;
    }
    
    return result;
}

std::map<std::string, double> BudgetAnalyzer::getMonthlyTrends() const {
    return transactionData.getMonthlyTotals();
}

std::map<std::string, double> BudgetAnalyzer::getCategoryAnalysis() const {
    return transactionData.getCategoryTotals();
}

double BudgetAnalyzer::getSpendingTrend() const {
    auto monthlyTrends = transactionData.getMonthlyTotals();
    
    if (monthlyTrends.size() < 2) return 0.0;
    
    auto it = monthlyTrends.rbegin();
    double lastMonth = it->second;
    ++it;
    double prevMonth = it->second;
    
    if (prevMonth == 0) return 0.0;
    return ((lastMonth - prevMonth) / std::abs(prevMonth)) * 100;
}

double BudgetAnalyzer::getAverageMonthlySpending() const {
    auto monthlyTrends = transactionData.getMonthlyTotals();
    
    if (monthlyTrends.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& pair : monthlyTrends) {
        total += std::abs(pair.second);
    }
    
    return total / monthlyTrends.size();
}
