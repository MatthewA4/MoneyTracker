//Copyright (C) 2026 Matthew Anderson
//MIT License

#include "AlertSystem.h"

AlertSystem::AlertSystem() : overallLimit(0.0) {}

void AlertSystem::setCategoryLimit(const std::string& category, double limit) {
    categoryLimits[category] = limit;
}

void AlertSystem::setOverallLimit(double limit) {
    overallLimit = limit;
}

std::vector<Alert> AlertSystem::checkTransactions(const TransactionData& data) {
    alerts.clear();
    
    auto categoryTotals = data.getCategoryTotals();
    double totalExpenses = data.getTotalSpending();
    
    // Check category limits
    for (const auto& limit : categoryLimits) {
        auto it = categoryTotals.find(limit.first);
        if (it != categoryTotals.end() && it->second > limit.second) {
            Alert alert;
            alert.type = Alert::WARNING;
            alert.category = limit.first;
            alert.amount = it->second;
            alert.limit = limit.second;
            alert.message = "Category '" + limit.first + "' exceeded limit: $" + 
                          std::to_string(static_cast<int>(it->second)) + " / $" + 
                          std::to_string(static_cast<int>(limit.second));
            alerts.push_back(alert);
        }
    }
    
    // Check overall limit
    if (overallLimit > 0 && totalExpenses > overallLimit) {
        Alert alert;
        alert.type = Alert::WARNING;
        alert.category = "Overall";
        alert.amount = totalExpenses;
        alert.limit = overallLimit;
        alert.message = "Overall spending exceeded limit: $" + 
                       std::to_string(static_cast<int>(totalExpenses)) + " / $" + 
                       std::to_string(static_cast<int>(overallLimit));
        alerts.push_back(alert);
    }
    
    return alerts;
}

bool AlertSystem::isCategoryExceeded(const std::string& category) const {
    auto it = categoryLimits.find(category);
    return it != categoryLimits.end();
}
