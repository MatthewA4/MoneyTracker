//Copyright (C) 2026 Matthew Anderson
//MIT License

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

struct CategoryRule {
    std::string category;
    std::vector<std::string> keywords;  // Any of these keywords triggers the category
};

class ConfigManager {
public:
    ConfigManager();
    
    // Load configuration from JSON file
    bool loadCategoriesFromFile(const std::string& filePath);
    
    // Get category based on transaction description
    std::string categorizeTransaction(const std::string& description) const;
    
    // Get all categories
    const std::vector<CategoryRule>& getCategories() const { return categories; }
    
    // Reload default categories if file not found
    void loadDefaultCategories();
    
private:
    std::vector<CategoryRule> categories;
    void addCategory(const std::string& name, const std::vector<std::string>& keywords);
    std::string toLower(const std::string& str) const;
};
