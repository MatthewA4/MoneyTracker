//Copyright (C) 2026 Matthew Anderson
//MIT License

#include "ConfigManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

ConfigManager::ConfigManager() {
    loadDefaultCategories();
}

bool ConfigManager::loadCategoriesFromFile(const std::string& filePath) {
    // Simple JSON parsing - for production would use a proper JSON library
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not load categories from " << filePath << ", using defaults" << std::endl;
        return false;
    }
    
    categories.clear();
    std::string line;
    std::string currentCategory;
    
    while (std::getline(file, line)) {
        // Very basic JSON parsing - look for category names
        if (line.find("\"category\"") != std::string::npos) {
            size_t start = line.find("\"category\": \"");
            if (start != std::string::npos) {
                start += 13;  // len("\"category\": \"")
                size_t end = line.find("\"", start);
                currentCategory = line.substr(start, end - start);
            }
        }
        // Look for keywords array
        if (line.find("\"keywords\"") != std::string::npos && !currentCategory.empty()) {
            std::vector<std::string> keywords;
            // This is simplified - a real implementation would use a JSON parser
            while (std::getline(file, line)) {
                if (line.find("]") != std::string::npos) break;
                size_t start = line.find("\"");
                if (start != std::string::npos) {
                    size_t end = line.find("\"", start + 1);
                    if (end != std::string::npos) {
                        keywords.push_back(line.substr(start + 1, end - start - 1));
                    }
                }
            }
            if (!keywords.empty()) {
                addCategory(currentCategory, keywords);
            }
        }
    }
    
    file.close();
    return !categories.empty();
}

std::string ConfigManager::categorizeTransaction(const std::string& description) const {
    std::string desc = toLower(description);
    
    for (const auto& rule : categories) {
        for (const auto& keyword : rule.keywords) {
            if (desc.find(toLower(keyword)) != std::string::npos) {
                return rule.category;
            }
        }
    }
    
    return "Other";
}

void ConfigManager::loadDefaultCategories() {
    categories.clear();
    
    addCategory("Groceries", {
        "grocery", "safeway", "trader", "whole foods", "kroger", "publix",
        "walmart grocery", "costco", "market", "supermarket"
    });
    
    addCategory("Gas", {
        "gas", "fuel", "shell", "chevron", "bp", "exxon", "mobil",
        "texaco", "sunoco", "speedway"
    });
    
    addCategory("Dining", {
        "restaurant", "cafe", "pizza", "burger", "diner", "bar",
        "coffee", "starbucks", "chipotle", "taco bell", "mcdonalds",
        "wendy's", "chick-fil-a", "olive garden", "applebee's",
        "dinner", "lunch", "breakfast", "food delivery"
    });
    
    addCategory("Shopping", {
        "amazon", "ebay", "walmart", "target", "mall", "store",
        "retail", "boutique", "department store", "costco", "sam's club"
    });
    
    addCategory("Entertainment", {
        "netflix", "spotify", "hulu", "disney", "movie", "theater",
        "cinema", "concert", "ticket", "amusement"
    });
    
    addCategory("Utilities", {
        "electricity", "water", "gas bill", "internet", "cable",
        "phone bill", "electric", "power", "utility"
    });
    
    addCategory("Healthcare", {
        "doctor", "hospital", "pharmacy", "dental", "health",
        "medical", "clinic", "cvs", "walgreens"
    });
    
    addCategory("Transportation", {
        "uber", "lyft", "taxi", "parking", "toll", "transit",
        "bus", "train", "metro", "parking garage"
    });
    
    addCategory("Subscriptions", {
        "subscription", "membership", "prime", "membership fee"
    });
    
    addCategory("Transfers", {
        "transfer", "deposit", "xfer", "move funds", "wire"
    });
}

void ConfigManager::addCategory(const std::string& name, const std::vector<std::string>& keywords) {
    CategoryRule rule;
    rule.category = name;
    rule.keywords = keywords;
    categories.push_back(rule);
}

std::string ConfigManager::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
