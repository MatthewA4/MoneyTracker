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
//SpreadsheetGenerator.h

#pragma once

#include "TransactionData.h"
#include "BudgetAnalyzer.h"
#include <string>
#include <memory>

class SpreadsheetGenerator {
public:
    SpreadsheetGenerator();
    ~SpreadsheetGenerator() = default;
    
    bool generateSpreadsheet(const TransactionData& data, 
                            const BudgetAnalyzer& analyzer,
                            const std::string& outputPath);
    
private:
    bool createSummarySheet(void* workbook, const BudgetAnalyzer& analyzer);
    bool createTransactionSheet(void* workbook, const TransactionData& data);
    bool createCategorySheet(void* workbook, const TransactionData& data, const BudgetAnalyzer& analyzer);
    bool createMonthlySheet(void* workbook, const TransactionData& data, const BudgetAnalyzer& analyzer);
    bool createCharts(void* workbook, const TransactionData& data, const BudgetAnalyzer& analyzer);
};
