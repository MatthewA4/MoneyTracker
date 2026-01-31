# MoneyTracker - Getting Started

## Quick Start

### 1. Build the Project
```bash
cd MoneyTracker
mkdir -p build && cd build
cmake -G Ninja ..
ninja
cd ..
```

### 2. Run with Sample Data
```bash
./build/moneytracker \
  -i data/sample_bank.csv \
  -a "Bank" \
  -i data/sample_savings.csv \
  -a "Savings" \
  --verbose
```

### 3. Open the Generated Spreadsheet
```bash
# The file "budget_report.xlsx" will be created
# Open with Excel, Google Sheets, or LibreOffice Calc
```

## Usage Examples

### Single Account
```bash
./build/moneytracker -i mybank.csv -a "My Checking" -o mybudget.xlsx
```

### Multiple Accounts
```bash
./build/moneytracker \
  -i checking.csv -a "Checking" \
  -i savings.csv -a "Savings" \
  -i credit.csv -a "Credit Card" \
  -o full_budget.xlsx
```

### Specify Format
```bash
# Auto-detect (default)
./build/moneytracker -i file.csv

# Bank format
./build/moneytracker -i file.csv --format bank

# Generic format
./build/moneytracker -i file.csv --format generic
```

### Verbose Output
```bash
./build/moneytracker -i file.csv --verbose
```

## CSV Format Support

### Bank Format
```
Date,Description,Debit,Credit,Balance
01/15/2024,GROCERY,-50.00,,1000.00
01/16/2024,DEPOSIT,,2000.00,3000.00
```

### Generic Format
```
Date,Description,Amount,Balance
01/15/2024,GROCERY,-50.00,1000.00
01/16/2024,DEPOSIT,2000.00,3000.00
```

## Generated Excel Sheets

1. **Summary** - Overview of income, expenses, and categories
2. **Transactions** - Detailed transaction list
3. **By Category** - Spending totals per category
4. **Monthly Trends** - Month-by-month spending analysis

## Automatic Categorization

Transactions are automatically categorized as:
- Groceries (Safeway, Trader Joe's, Whole Foods)
- Gas (Shell, Chevron, fuel stations)
- Dining (Restaurants, cafes, pizza places)
- Shopping (Amazon, Walmart, Target, eBay)
- Subscriptions (Netflix, Spotify, Hulu)
- Transfers (XFer, deposit, transfer)
- Other (uncategorized)

## Tips

1. **Export CSV from your bank** - Most banks have a CSV/Excel export option
2. **Use account names** - Name accounts clearly for organization (e.g., "Chase Checking", "AmEx")
3. **Check categorization** - Review the spreadsheet to ensure transactions are categorized correctly
4. **Monthly analysis** - Look at monthly trends to identify spending patterns
5. **Budget planning** - Use category totals to plan your budget

## Troubleshooting

### File not found
```
Error: Could not open file: myfile.csv
```
Check that the file path is correct and the file exists.

### Unknown format error
If your bank's CSV format isn't recognized, use `--format generic` and verify the column order.

### No transactions loaded
Verify the CSV file has data and follows the expected format (check headers).

## Example Workflow

1. Export transactions from your bank as CSV
2. Run: `./moneytracker -i transactions.csv -a "Checking"`
3. Open generated Excel file
4. Review categorization
5. Analyze spending patterns
6. Plan next month's budget
