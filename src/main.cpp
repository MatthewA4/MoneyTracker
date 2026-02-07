//Copyright (C) 2026 Matthew Anderson
//MIT License

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "CSVParser.h"
#include "TransactionData.h"
#include "BudgetAnalyzer.h"
#include "SpreadsheetGenerator.h"
#include "ConfigManager.h"
#include "AlertSystem.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void printHelp(const po::options_description& desc) {
    std::cout << "MoneyTracker - Budget Analysis Tool" << std::endl;
    std::cout << "===================================" << std::endl << std::endl;
    std::cout << "A powerful C++ application that imports CSV files from banks" << std::endl;
    std::cout << "and generates professional budget analysis spreadsheets." << std::endl << std::endl;
    std::cout << desc << std::endl;
}

bool fileExists(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

bool isDirectoryWritable(const std::string& dirPath) {
    fs::path dir(dirPath);
    if (!dir.has_parent_path()) {
        dir = fs::current_path();
    } else {
        dir = dir.parent_path();
    }
    
    if (!fs::exists(dir)) {
        try {
            fs::create_directories(dir);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    return fs::is_directory(dir);
}

int main(int argc, char* argv[]) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("input,i", po::value<std::vector<std::string>>(), 
             "input CSV file(s)")
            ("account,a", po::value<std::vector<std::string>>(), 
             "account name(s) corresponding to input files")
            ("output,o", po::value<std::string>()->default_value("budget_report.xlsx"), 
             "output Excel file")
            ("format,f", po::value<std::string>()->default_value("auto"), 
             "CSV format: auto, bank, or generic (default: auto)")
            ("category-config", po::value<std::string>(),
             "path to custom categories.json file")
            ("verbose,v", "verbose output")
            ("no-spreadsheet", "skip Excel spreadsheet generation (console output only)");
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        
        if (vm.count("help")) {
            printHelp(desc);
            return 0;
        }
        
        // ==================== INPUT VALIDATION ====================
        if (!vm.count("input")) {
            std::cerr << "Error: No input files specified. Use -i <file>" << std::endl;
            printHelp(desc);
            return 1;
        }
        
        auto inputFiles = vm["input"].as<std::vector<std::string>>();
        
        // Validate input files exist
        for (const auto& inputFile : inputFiles) {
            if (!fileExists(inputFile)) {
                std::cerr << "Error: Input file not found: " << inputFile << std::endl;
                return 1;
            }
        }
        
        auto accountNames = vm.count("account") ? vm["account"].as<std::vector<std::string>>() 
                                               : std::vector<std::string>();
        
        // Default account names if not provided
        while (accountNames.size() < inputFiles.size()) {
            accountNames.push_back("Account " + std::to_string(accountNames.size() + 1));
        }
        
        std::string format = vm["format"].as<std::string>();
        std::string outputFile = vm["output"].as<std::string>();
        bool verbose = vm.count("verbose") > 0;
        bool generateSpreadsheet = !vm.count("no-spreadsheet");
        
        // Validate output directory is writable
        if (generateSpreadsheet && !isDirectoryWritable(outputFile)) {
            std::cerr << "Error: Output directory is not writable: " 
                     << fs::path(outputFile).parent_path() << std::endl;
            return 1;
        }
        
        if (verbose) {
            std::cout << "========== MoneyTracker Configuration ==========" << std::endl;
            std::cout << "Input files: ";
            for (const auto& f : inputFiles) std::cout << f << " ";
            std::cout << std::endl;
            std::cout << "Account names: ";
            for (const auto& a : accountNames) std::cout << a << " ";
            std::cout << std::endl;
            std::cout << "Output file: " << outputFile << std::endl;
            std::cout << "Format: " << format << std::endl;
            std::cout << "Generate Spreadsheet: " << (generateSpreadsheet ? "Yes" : "No") << std::endl;
            std::cout << "=============================================" << std::endl << std::endl;
        }
        
        // ==================== LOAD CONFIGURATION ====================
        auto configManager = std::make_shared<ConfigManager>();
        
        if (vm.count("category-config")) {
            std::string configPath = vm["category-config"].as<std::string>();
            if (fileExists(configPath)) {
                if (verbose) {
                    std::cout << "Loading custom category configuration from: " << configPath << std::endl;
                }
                configManager->loadCategoriesFromFile(configPath);
            } else {
                std::cerr << "Warning: Category config file not found: " << configPath << std::endl;
            }
        } else {
            // Try to load from default location
            std::string defaultConfig = "data/categories.json";
            if (fileExists(defaultConfig)) {
                if (verbose) {
                    std::cout << "Loading category configuration from: " << defaultConfig << std::endl;
                }
                configManager->loadCategoriesFromFile(defaultConfig);
            }
        }
        
        // ==================== PARSE CSV FILES ====================
        TransactionData allData;
        CSVParser parser(configManager);
        
        int totalTransactions = 0;
        for (size_t i = 0; i < inputFiles.size(); ++i) {
            if (verbose) {
                std::cout << "Processing: " << inputFiles[i] 
                         << " (Account: " << accountNames[i] << ")..." << std::endl;
            }
            
            try {
                std::vector<Transaction> transactions;
                
                if (format == "bank") {
                    transactions = parser.parseBank(inputFiles[i], accountNames[i]);
                } else if (format == "generic") {
                    transactions = parser.parseGeneric(inputFiles[i], accountNames[i]);
                } else {
                    transactions = parser.parse(inputFiles[i], accountNames[i]);
                }
                
                allData.addTransactions(transactions);
                totalTransactions += transactions.size();
                
                if (verbose) {
                    std::cout << "  OK Loaded " << transactions.size() << " transactions" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing " << inputFiles[i] << ": " << e.what() << std::endl;
                return 1;
            }
        }
        
        if (verbose) {
            std::cout << "\nTotal transactions loaded: " << totalTransactions << std::endl << std::endl;
        }
        
        if (totalTransactions == 0) {
            std::cerr << "Error: No transactions were loaded from input files" << std::endl;
            return 1;
        }
        
        // ==================== ANALYZE BUDGET ====================
        BudgetAnalyzer analyzer(allData);
        BudgetSummary summary = analyzer.analyzeBudget();
        
        // ==================== DISPLAY CONSOLE OUTPUT ====================
        std::cout << "=== BUDGET SUMMARY ===" << std::endl;
        std::cout << "Total Income:    $" << std::fixed << std::setprecision(2) 
                 << summary.totalIncome << std::endl;
        std::cout << "Total Expenses:  $" << summary.totalExpenses << std::endl;
        std::cout << "Net Change:      $" << summary.netChange << std::endl;
        
        std::cout << "\n=== SPENDING BY CATEGORY ===" << std::endl;
        for (const auto& cat : summary.categoryBreakdown) {
            std::cout << "  " << std::setw(20) << std::left << cat.first 
                     << ": $" << std::setw(10) << std::right << std::fixed << std::setprecision(2)
                     << cat.second << std::endl;
        }
        
        std::cout << "\n=== MONTHLY TRENDS ===" << std::endl;
        for (const auto& month : summary.monthlyTrends) {
            std::cout << "  " << month.first << ": $" << std::fixed << std::setprecision(2) 
                     << month.second << std::endl;
        }
        
        if (verbose) {
            std::cout << "\n=== ACCOUNT BREAKDOWN ===" << std::endl;
            for (const auto& account : summary.accountBreakdown) {
                std::cout << "  " << account.first << ": $" << std::fixed << std::setprecision(2)
                         << account.second << std::endl;
            }
            
            std::cout << "\n=== STATISTICS ===" << std::endl;
            std::cout << "Total Transactions: " << totalTransactions << std::endl;
            std::cout << "Average Transaction: $" << analyzer.getAverageTransaction() << std::endl;
            std::cout << "Average Monthly Spending: $" << analyzer.getAverageMonthlySpending() << std::endl;
        }
        
        // ==================== GENERATE SPREADSHEET ====================
        if (generateSpreadsheet) {
            if (verbose) {
                std::cout << "\nGenerating Excel spreadsheet..." << std::endl;
            }
            
            try {
                SpreadsheetGenerator generator;
                if (!generator.generateSpreadsheet(allData, analyzer, outputFile)) {
                    std::cerr << "Error: Failed to generate spreadsheet" << std::endl;
                    return 1;
                }
                
                std::cout << "\nOK Spreadsheet generated successfully: " << outputFile << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error generating spreadsheet: " << e.what() << std::endl;
                return 1;
            }
        }
        
        std::cout << "\nOK Analysis complete!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
