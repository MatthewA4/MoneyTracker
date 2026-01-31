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
//main.cpp

#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "CSVParser.h"
#include "TransactionData.h"
#include "BudgetAnalyzer.h"
#include "SpreadsheetGenerator.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void printHelp(const po::options_description& desc) {
    std::cout << "MoneyTracker - Budget Analysis Tool" << std::endl;
    std::cout << "===================================" << std::endl << std::endl;
    std::cout << desc << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("input,i", po::value<std::vector<std::string>>(), "input CSV file(s)")
            ("account,a", po::value<std::vector<std::string>>(), "account name(s) corresponding to input files")
            ("output,o", po::value<std::string>()->default_value("budget_report.xlsx"), "output Excel file")
            ("format,f", po::value<std::string>()->default_value("auto"), "CSV format: auto, bank, or generic")
            ("generate-spreadsheet", "generate Excel spreadsheet (optional, not enabled by default)")
            ("verbose,v", "verbose output");
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        
        if (vm.count("help")) {
            printHelp(desc);
            return 0;
        }
        
        // Validate inputs
        if (!vm.count("input")) {
            std::cerr << "Error: No input files specified. Use -i <file>" << std::endl;
            printHelp(desc);
            return 1;
        }
        
        auto inputFiles = vm["input"].as<std::vector<std::string>>();
        auto accountNames = vm.count("account") ? vm["account"].as<std::vector<std::string>>() 
                                               : std::vector<std::string>();
        
        // Default account names if not provided
        while (accountNames.size() < inputFiles.size()) {
            accountNames.push_back("Account " + std::to_string(accountNames.size() + 1));
        }
        
        std::string format = vm["format"].as<std::string>();
        std::string outputFile = vm["output"].as<std::string>();
        bool verbose = vm.count("verbose") > 0;
        bool generateSpreadsheet = vm.count("generate-spreadsheet") > 0;
        
        if (verbose) {
            std::cout << "Input files: ";
            for (const auto& f : inputFiles) std::cout << f << " ";
            std::cout << std::endl;
            if (generateSpreadsheet) {
                std::cout << "Output file: " << outputFile << std::endl;
            }
            std::cout << "Format: " << format << std::endl;
        }
        
        // Parse CSV files
        TransactionData allData;
        CSVParser parser;
        
        for (size_t i = 0; i < inputFiles.size(); ++i) {
            if (verbose) {
                std::cout << "Processing: " << inputFiles[i] << " (Account: " << accountNames[i] << ")" << std::endl;
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
                
                if (verbose) {
                    std::cout << "  Loaded " << transactions.size() << " transactions" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing " << inputFiles[i] << ": " << e.what() << std::endl;
                return 1;
            }
        }
        
        if (verbose) {
            std::cout << "Total transactions loaded: " << allData.getAllTransactions().size() << std::endl;
        }
        
        // Analyze budget
        BudgetAnalyzer analyzer(allData);
        BudgetSummary summary = analyzer.analyzeBudget();
        
        // Display command-line output
        std::cout << "\n=== BUDGET SUMMARY ===" << std::endl;
        std::cout << "Total Income: $" << summary.totalIncome << std::endl;
        std::cout << "Total Expenses: $" << summary.totalExpenses << std::endl;
        std::cout << "Net Change: $" << summary.netChange << std::endl;
        
        std::cout << "\n=== SPENDING BY CATEGORY ===" << std::endl;
        for (const auto& cat : summary.categoryBreakdown) {
            std::cout << "  " << cat.first << ": $" << cat.second << std::endl;
        }
        
        std::cout << "\n=== MONTHLY TRENDS ===" << std::endl;
        for (const auto& month : summary.monthlyTrends) {
            std::cout << "  " << month.first << ": $" << month.second << std::endl;
        }
        
        if (verbose) {
            std::cout << "\nTotal transactions processed: " << allData.getAllTransactions().size() << std::endl;
        }
        
        // Generate spreadsheet if requested
        if (generateSpreadsheet) {
            if (verbose) {
                std::cout << "\nGenerating Excel spreadsheet..." << std::endl;
            }
            
            SpreadsheetGenerator generator;
            if (!generator.generateSpreadsheet(allData, analyzer, outputFile)) {
                std::cerr << "Error: Failed to generate spreadsheet" << std::endl;
                return 1;
            }
            
            std::cout << "\nSpreadsheet generated: " << outputFile << std::endl;
        }
        
        if (verbose) {
            std::cout << "\nBudget Summary:" << std::endl;
            std::cout << "  Total Income: $" << summary.totalIncome << std::endl;
            std::cout << "  Total Expenses: $" << summary.totalExpenses << std::endl;
            std::cout << "  Net Change: $" << summary.netChange << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
