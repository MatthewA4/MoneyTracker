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
//SpreadsheetGenerator.cpp

#include "SpreadsheetGenerator.h"
#include "xlsxwriter.h"
#include <cmath>
#include <iomanip>
#include <sstream>

SpreadsheetGenerator::SpreadsheetGenerator() {}

bool SpreadsheetGenerator::generateSpreadsheet(const TransactionData& data,
                                               const BudgetAnalyzer& analyzer,
                                               const std::string& outputPath) {
    lxw_workbook* workbook = workbook_new(outputPath.c_str());
    if (!workbook) return false;
    
    // Create sheets
    if (!createSummarySheet(workbook, analyzer)) {
        workbook_close(workbook);
        return false;
    }
    
    if (!createTransactionSheet(workbook, data)) {
        workbook_close(workbook);
        return false;
    }
    
    if (!createCategorySheet(workbook, data, analyzer)) {
        workbook_close(workbook);
        return false;
    }
    
    if (!createMonthlySheet(workbook, data, analyzer)) {
        workbook_close(workbook);
        return false;
    }
    
    workbook_close(workbook);
    return true;
}

bool SpreadsheetGenerator::createSummarySheet(void* wb, const BudgetAnalyzer& analyzer) {
    lxw_workbook* workbook = static_cast<lxw_workbook*>(wb);
    lxw_worksheet* worksheet = workbook_add_worksheet(workbook, "Summary");
    if (!worksheet) return false;
    
    lxw_format* header_format = workbook_add_format(workbook);
    format_set_bold(header_format);
    format_set_font_size(header_format, 14);
    format_set_bg_color(header_format, 0xD3D3D3);
    
    lxw_format* label_format = workbook_add_format(workbook);
    format_set_bold(label_format);
    
    lxw_format* currency_format = workbook_add_format(workbook);
    format_set_num_format(currency_format, "$#,##0.00");
    
    worksheet_set_column(worksheet, 0, 1, 30, NULL);
    
    // Title
    worksheet_write_string(worksheet, 0, 0, "Budget Summary", header_format);
    
    // Get summary data (cast away const for this call)
    BudgetSummary summary = const_cast<BudgetAnalyzer&>(analyzer).analyzeBudget();
    
    int row = 2;
    worksheet_write_string(worksheet, row, 0, "Total Income:", label_format);
    worksheet_write_number(worksheet, row, 1, summary.totalIncome, currency_format);
    row++;
    
    worksheet_write_string(worksheet, row, 0, "Total Expenses:", label_format);
    worksheet_write_number(worksheet, row, 1, summary.totalExpenses, currency_format);
    row++;
    
    worksheet_write_string(worksheet, row, 0, "Net Change:", label_format);
    worksheet_write_number(worksheet, row, 1, summary.netChange, currency_format);
    row += 2;
    
    worksheet_write_string(worksheet, row, 0, "Spending by Category:", header_format);
    row++;
    
    for (const auto& pair : summary.categoryBreakdown) {
        worksheet_write_string(worksheet, row, 0, pair.first.c_str(), NULL);
        worksheet_write_number(worksheet, row, 1, std::abs(pair.second), currency_format);
        row++;
    }
    
    return true;
}

bool SpreadsheetGenerator::createTransactionSheet(void* wb, const TransactionData& data) {
    lxw_workbook* workbook = static_cast<lxw_workbook*>(wb);
    lxw_worksheet* worksheet = workbook_add_worksheet(workbook, "Transactions");
    if (!worksheet) return false;
    
    lxw_format* header_format = workbook_add_format(workbook);
    format_set_bold(header_format);
    format_set_bg_color(header_format, 0xCCCCCC);
    
    lxw_format* currency_format = workbook_add_format(workbook);
    format_set_num_format(currency_format, "$#,##0.00");
    
    // Set column widths
    worksheet_set_column(worksheet, 0, 0, 12, NULL);
    worksheet_set_column(worksheet, 1, 1, 30, NULL);
    worksheet_set_column(worksheet, 2, 2, 15, NULL);
    worksheet_set_column(worksheet, 3, 3, 12, NULL);
    worksheet_set_column(worksheet, 4, 4, 12, NULL);
    worksheet_set_column(worksheet, 5, 5, 15, NULL);
    
    // Headers
    worksheet_write_string(worksheet, 0, 0, "Date", header_format);
    worksheet_write_string(worksheet, 0, 1, "Description", header_format);
    worksheet_write_string(worksheet, 0, 2, "Category", header_format);
    worksheet_write_string(worksheet, 0, 3, "Amount", header_format);
    worksheet_write_string(worksheet, 0, 4, "Balance", header_format);
    worksheet_write_string(worksheet, 0, 5, "Account", header_format);
    
    const auto& transactions = data.getAllTransactions();
    for (size_t i = 0; i < transactions.size(); ++i) {
        const auto& t = transactions[i];
        int row = i + 1;
        
        worksheet_write_string(worksheet, row, 0, t.date.c_str(), NULL);
        worksheet_write_string(worksheet, row, 1, t.description.c_str(), NULL);
        worksheet_write_string(worksheet, row, 2, t.category.c_str(), NULL);
        worksheet_write_number(worksheet, row, 3, t.amount, currency_format);
        worksheet_write_number(worksheet, row, 4, t.balance, currency_format);
        worksheet_write_string(worksheet, row, 5, t.accountName.c_str(), NULL);
    }
    
    return true;
}

bool SpreadsheetGenerator::createCategorySheet(void* wb, const TransactionData& data, const BudgetAnalyzer& analyzer) {
    lxw_workbook* workbook = static_cast<lxw_workbook*>(wb);
    lxw_worksheet* worksheet = workbook_add_worksheet(workbook, "By Category");
    if (!worksheet) return false;
    
    lxw_format* header_format = workbook_add_format(workbook);
    format_set_bold(header_format);
    format_set_bg_color(header_format, 0xCCCCCC);
    
    lxw_format* currency_format = workbook_add_format(workbook);
    format_set_num_format(currency_format, "$#,##0.00");
    
    worksheet_set_column(worksheet, 0, 0, 20, NULL);
    worksheet_set_column(worksheet, 1, 1, 15, NULL);
    
    worksheet_write_string(worksheet, 0, 0, "Category", header_format);
    worksheet_write_string(worksheet, 0, 1, "Total Spending", header_format);
    
    (void)data;
    auto categoryAnalysis = const_cast<BudgetAnalyzer&>(analyzer).getCategoryAnalysis();
    int row = 1;
    for (const auto& pair : categoryAnalysis) {
        worksheet_write_string(worksheet, row, 0, pair.first.c_str(), NULL);
        worksheet_write_number(worksheet, row, 1, std::abs(pair.second), currency_format);
        row++;
    }
    
    return true;
}

bool SpreadsheetGenerator::createMonthlySheet(void* wb, const TransactionData& data, const BudgetAnalyzer& analyzer) {
    lxw_workbook* workbook = static_cast<lxw_workbook*>(wb);
    lxw_worksheet* worksheet = workbook_add_worksheet(workbook, "Monthly Trends");
    if (!worksheet) return false;
    
    lxw_format* header_format = workbook_add_format(workbook);
    format_set_bold(header_format);
    format_set_bg_color(header_format, 0xCCCCCC);
    
    lxw_format* currency_format = workbook_add_format(workbook);
    format_set_num_format(currency_format, "$#,##0.00");
    
    worksheet_set_column(worksheet, 0, 0, 15, NULL);
    worksheet_set_column(worksheet, 1, 1, 15, NULL);
    
    worksheet_write_string(worksheet, 0, 0, "Month", header_format);
    worksheet_write_string(worksheet, 0, 1, "Total Spending", header_format);
    
    (void)data;
    auto monthlyTrends = const_cast<BudgetAnalyzer&>(analyzer).getMonthlyTrends();
    int row = 1;
    for (const auto& pair : monthlyTrends) {
        worksheet_write_string(worksheet, row, 0, pair.first.c_str(), NULL);
        worksheet_write_number(worksheet, row, 1, std::abs(pair.second), currency_format);
        row++;
    }
    
    return true;
}

bool SpreadsheetGenerator::createCharts(void* wb, const TransactionData& data, const BudgetAnalyzer& analyzer) {
    (void)wb;
    (void)data;
    (void)analyzer;
    // Note: Charts require more complex setup with proper series configuration.
    // This is a placeholder for future enhancement.
    return true;
}
