//Copyright (C) 2026 Matthew Anderson
//MIT License

#pragma once

#include <string>
#include <vector>
#include "TransactionData.h"

struct Alert {
    enum AlertType { WARNING, ERROR, INFO };
    
    AlertType type;
    std::string message;
    std::string category;
    double amount;
    double limit;
};

class AlertSystem {
public:
    AlertSystem();
    
    // Set budget limit for a category
    void setCategoryLimit(const std::string& category, double limit);
    
    // Set overall spending limit
    void setOverallLimit(double limit);
    
    // Check transactions against limits and generate alerts
    std::vector<Alert> checkTransactions(const TransactionData& data);
    
    // Check if category exceeded limit
    bool isCategoryExceeded(const std::string& category) const;
    
private:
    std::map<std::string, double> categoryLimits;
    double overallLimit;
    std::vector<Alert> alerts;
};
