# MoneyTracker - Bank CSV to Budget Spreadsheet Generator

A professional C++ application that imports CSV files from banks and generates comprehensive budget analysis spreadsheets with categorization, trends, and financial insights.

## Features

- ✅ **Multi-Account Support**: Process multiple bank account files simultaneously
- ✅ **Auto-Format Detection**: Automatically detects bank or generic CSV formats
- ✅ **Smart Categorization**: Automatically categorizes transactions with configurable rules
- ✅ **Multi-Date Format Support**: Handles MM/DD/YYYY, DD/MM/YYYY, and YYYY-MM-DD formats
- ✅ **Comprehensive Error Handling**: Detailed error messages and file validation
- ✅ **Robust CSV Parsing**: Handles quoted fields, various delimiters, and edge cases
- ✅ **Comprehensive Analysis**: 
  - Income vs. Expenses breakdown
  - Spending by category
  - Monthly trends and analysis
  - Account-level analysis
  - Average transaction calculations
- ✅ **Excel Export**: Generates professional XLSX spreadsheets with:
  - Summary sheet with key metrics
  - Detailed transaction listing
  - Category breakdown
  - Monthly trends
  - Charts and graphs
- ✅ **Configuration System**: Customizable category rules via JSON config file
- ✅ **Verbose Logging**: Detailed output for debugging and verification

## Requirements

- **C++17** or later
- **CMake 3.12+**
- **Ninja** build system
- **Boost Libraries**: system, filesystem, program_options
- **libxlsxwriter**: For Excel file generation

## Installation

### Prerequisites (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install libboost-all-dev libxlsxwriter-dev cmake ninja-build g++ libgtk-3-dev
```

### Prerequisites (macOS with Homebrew)

```bash
brew install boost libxlsxwriter cmake ninja gtk+3
```

### Build Instructions

```bash
cd MoneyTracker
mkdir -p build
cd build
cmake -G Ninja ..
ninja
```

The compiled binaries will be in the `build/` directory:
- `moneytracker` - Command-line interface
- `moneytracker-gui` - Graphical user interface (if GTK3 is available)

## Quick Start

### GUI Usage (Recommended for Most Users)

```bash
./build/moneytracker-gui
```

The GUI provides an intuitive interface for:
- Selecting CSV files and assigning account names
- Configuring transaction categories
- Viewing analysis results in tabbed interface
- Generating professional Excel spreadsheets

**No command-line knowledge required!** See [GUI_USER_GUIDE.md](GUI_USER_GUIDE.md) for complete walkthrough.

### Basic CLI Usage

```bash
./build/moneytracker -i checking.csv -a "My Checking Account" -v
```

This will:
1. Parse the CSV file
2. Display budget summary to console
3. Generate `budget_report.xlsx` with detailed analysis

### Multiple Accounts

```bash
./build/moneytracker \
  -i checking.csv -a "Checking" \
  -i savings.csv -a "Savings" \
  -i credit_card.csv -a "Amex" \
  -o financial_summary.xlsx -v
```

### Console-Only Analysis

```bash
./build/moneytracker -i transactions.csv --no-spreadsheet -v
```

### Custom Category Configuration

```bash
./build/moneytracker -i data.csv --category-config my_categories.json -v
```

## Command Line Options

| Option | Short | Description | Default |
|--------|-------|-------------|---------|
| `--help` | `-h` | Display help message | - |
| `--input` | `-i` | Input CSV file (can use multiple times) | Required |
| `--account` | `-a` | Account name (corresponds to input files) | "Account 1", "Account 2", etc. |
| `--output` | `-o` | Output Excel file path | `budget_report.xlsx` |
| `--format` | `-f` | CSV format: `auto`, `bank`, or `generic` | `auto` |
| `--category-config` | - | Path to custom categories.json file | Uses default config |
| `--verbose` | `-v` | Enable verbose/detailed output | Disabled |
| `--no-spreadsheet` | - | Skip Excel generation (console only) | Spreadsheet is generated |

## CSV Format Support

### Bank Format (Default)

```csv
Date,Description,Debit,Credit,Balance
01/15/2024,SAFEWAY GROCERY,-50.00,,1000.00
01/16/2024,SALARY DEPOSIT,,2000.00,3000.00
```

### Generic Format

```csv
Date,Description,Amount,Balance
01/15/2024,SAFEWAY GROCERY,-50.00,1000.00
01/16/2024,SALARY DEPOSIT,2000.00,3000.00
```

## Category Configuration

Edit `data/categories.json` to customize categorization. Default categories:

- **Groceries**: Safeway, Trader Joe's, Whole Foods, Kroger, etc.
- **Gas**: Shell, Chevron, BP, Exxon, etc.
- **Dining**: Restaurants, cafes, food delivery, etc.
- **Shopping**: Amazon, Walmart, Target, malls, etc.
- **Entertainment**: Netflix, Spotify, movies, concerts, etc.
- **Utilities**: Electricity, water, internet, phone
- **Healthcare**: Doctors, hospitals, pharmacies
- **Transportation**: Uber, Lyft, parking, tolls
- **Subscriptions**: Monthly/annual memberships
- **Transfers**: Bank transfers, deposits, wires

## Excel Output

The generated `.xlsx` includes:

1. **Summary Sheet**: Income, expenses, net change, category breakdown
2. **Transactions Sheet**: Complete transaction list with account info
3. **By Category**: Spending totals by category with percentages
4. **Monthly Trends**: Month-by-month spending analysis
5. **Charts**: Pie charts and column charts for visual analysis

## Example Output

```
========== MoneyTracker Configuration ==========
Input files: checking.csv
Account names: Checking
Output file: budget_report.xlsx
Format: auto
Generate Spreadsheet: Yes
=============================================

Loading category configuration from: data/categories.json
Processing: checking.csv (Account: Checking)...
  OK Loaded 150 transactions

=== BUDGET SUMMARY ===
Total Income:    $4500.00
Total Expenses:  $2850.45
Net Change:      $1649.55

=== SPENDING BY CATEGORY ===
  Groceries           : $    450.00
  Dining              : $    380.50
  Gas                 : $    200.00
  Shopping            : $    620.00

=== MONTHLY TRENDS ===
  2024-01: $2850.45
  2024-02: $2750.30

OK Spreadsheet generated successfully: budget_report.xlsx

OK Analysis complete!
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "File not found" | Check file path, verify permissions |
| "Date parsing failed" | Use MM/DD/YYYY, DD/MM/YYYY, or YYYY-MM-DD |
| "Wrong format detected" | Use `-f bank` or `-f generic` to specify |
| "Categories not loading" | Verify JSON syntax, check file path |
| "Excel file not created" | Verify output directory exists, check disk space |

## Architecture

### Core Components

- **CSVParser**: Multi-format CSV parsing with date validation
- **DateParser**: Multi-format date parsing (MM/DD/YYYY, DD/MM/YYYY, YYYY-MM-DD)
- **ConfigManager**: Category rules from JSON configuration
- **TransactionData**: Transaction storage and queries
- **BudgetAnalyzer**: Spending analysis and trend calculation
- **AlertSystem**: Budget limit tracking and alerts
- **SpreadsheetGenerator**: Professional Excel file generation

## Development

To extend the application:

1. **Add Categories**: Edit `src/ConfigManager.cpp` or custom `categories.json`
2. **Add Analysis**: Add methods to `BudgetAnalyzer` class
3. **Add Sheets**: Create new methods in `SpreadsheetGenerator`
4. **Add Formats**: Extend `CSVParser` for new bank formats

## Performance

- Efficient O(n) CSV parsing
- Fast transaction queries and aggregation
- Memory-efficient design (100K+ transactions)
- Quick Excel generation

## Known Limitations

- Excel chart generation is simplified
- Single-threaded processing
- JSON parsing is basic (no external library)

## Future Enhancements

- [ ] Advanced Excel charts
- [ ] Multi-threaded parsing
- [ ] Regex category rules
- [ ] Database backend
- [ ] GUI interface
- [ ] OFX/QFX file support
- [ ] Recurring transaction detection
- [ ] Web dashboard

## License

MIT License - See LICENSE file

## Contributing

Contributions welcome! Please follow C++17 standards and include proper error handling.

---

Made for better personal finance management

