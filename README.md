# MoneyTracker - Bank CSV to Budget Spreadsheet Generator

A powerful C++ application that imports CSV files from banks and generates professional budget analysis spreadsheets with graphs and insights.

## Features

- **Multi-Account Support**: Process multiple bank account files simultaneously
- **Auto-Format Detection**: Automatically detects bank or generic CSV formats
- **Smart Categorization**: Automatically categorizes transactions (Groceries, Gas, Dining, Shopping, etc.)
- **Comprehensive Analysis**: 
  - Income vs. Expenses breakdown
  - Spending by category
  - Monthly trends and analysis
  - Account-level analysis
- **Excel Export**: Generates professional XLSX spreadsheets with:
  - Summary sheet with key metrics
  - Detailed transaction listing
  - Category breakdown
  - Monthly trends
  - Charts and graphs (pie charts for categories, column charts for trends)

## Requirements

- C++ 17 or later
- CMake 3.12+
- Ninja build system
- libboost (system, filesystem, program_options)
- libxlsxwriter

## Installation

### Dependencies (Ubuntu/Debian)

```bash
sudo apt-get install libboost-all-dev libxlsxwriter-dev cmake ninja-build
```

### macOS (using Homebrew)

```bash
brew install boost libxlsxwriter cmake ninja
```

### Build

```bash
cd MoneyTracker
mkdir -p build
cd build
cmake -G Ninja ..
ninja
```

## Usage

### Basic Usage

```bash
./moneytracker -i account1.csv -a "Checking" -o budget_report.xlsx
```

### Multiple Accounts

```bash
./moneytracker \
  -i checking.csv -a "Checking" \
  -i savings.csv -a "Savings" \
  -o full_budget_report.xlsx
```

### With Options

```bash
./moneytracker \
  -i transactions.csv \
  --account "My Account" \
  --output budget.xlsx \
  --format bank \
  --verbose
```

### Command Line Options

- `-h, --help`: Show help message
- `-i, --input FILE`: Input CSV file (can specify multiple times)
- `-a, --account NAME`: Account name (corresponds to input files)
- `-o, --output FILE`: Output Excel file (default: budget_report.xlsx)
- `-f, --format FORMAT`: CSV format: `auto`, `bank`, or `generic` (default: auto)
- `-v, --verbose`: Enable verbose output

## CSV Format Support

### Bank Format

```
Date,Description,Debit,Credit,Balance
01/15/2024,GROCERY STORE,-50.00,,1000.00
01/16/2024,SALARY DEPOSIT,,2000.00,3000.00
```

### Generic Format

```
Date,Description,Amount,Balance
01/15/2024,GROCERY STORE,-50.00,1000.00
01/16/2024,SALARY DEPOSIT,2000.00,3000.00
```

## Output

The generated Excel file includes:

1. **Summary Sheet**: Overview of income, expenses, and category breakdown
2. **Transactions Sheet**: Detailed list of all transactions with account info
3. **By Category Sheet**: Spending totals grouped by category
4. **Monthly Trends Sheet**: Month-by-month spending analysis
5. **Charts Sheet**: Visual representations (pie charts, column charts)

## Architecture

- `CSVParser`: Handles CSV file parsing for multiple formats
- `TransactionData`: Manages and queries transaction data
- `BudgetAnalyzer`: Analyzes spending patterns and generates insights
- `SpreadsheetGenerator`: Creates Excel files with formatting and charts

## Project Structure

```
MoneyTracker/
├── src/
│   ├── main.cpp
│   ├── CSVParser.cpp
│   ├── TransactionData.cpp
│   ├── BudgetAnalyzer.cpp
│   └── SpreadsheetGenerator.cpp
├── include/
│   ├── CSVParser.h
│   ├── TransactionData.h
│   ├── BudgetAnalyzer.h
│   └── SpreadsheetGenerator.h
├── tests/
├── data/
│   └── (sample CSV files)
├── CMakeLists.txt
└── README.md
```

## Examples

### Example: Analyze Bank Account

1. Export transactions from your bank as CSV
2. Run:
   ```bash
   ./moneytracker -i transactions.csv -a "Checking" --verbose
   ```
3. Open the generated `budget_report.xlsx` in Excel or Google Sheets

### Example: Combine Multiple Accounts

```bash
./moneytracker \
  -i checking.csv -a "Chase Checking" \
  -i savings.csv -a "Chase Savings" \
  -i cc.csv -a "Amex" \
  -o combined_budget.xlsx \
  --verbose
```

## Development

To extend the application:

1. Add new categories in `CSVParser::categorizeTransaction()`
2. Add new analysis methods in `BudgetAnalyzer`
3. Create new sheets in `SpreadsheetGenerator::createXxxSheet()`
4. Add chart types in `SpreadsheetGenerator::createCharts()`

## License

MIT

## Contributing

Contributions welcome! Please ensure code follows C++17 standards and includes proper error handling.
