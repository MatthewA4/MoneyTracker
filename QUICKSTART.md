# MoneyTracker - Quick Start Guide

## Two Ways to Use MoneyTracker

### 🖥️ Option 1: Graphical Interface (GUI) - Recommended

**Best for:** Non-technical users, interactive workflow, visual preferences

```bash
./build/moneytracker-gui
```

A user-friendly window opens with tabs for:
1. **Input** - Select CSV files and name accounts
2. **Configuration** - Choose category settings
3. **Summary** - View income/expense overview
4. **Categories** - See spending breakdown
5. **Monthly** - Track trends over time
6. **Transactions** - Review detailed ledger

**Complete guide**: See [GUI_USER_GUIDE.md](GUI_USER_GUIDE.md)

### 💻 Option 2: Command Line (CLI) - For Automation

**Best for:** Scripting, automation, server deployments, batch processing

```bash
# Single account
./build/moneytracker -i checking.csv -a "Checking"

# Multiple accounts
./build/moneytracker \
  -i checking.csv -a "Checking" \
  -i savings.csv -a "Savings" \
  -o my_budget.xlsx

# With verbose output
./build/moneytracker -i data.csv --verbose

# Custom categories
./build/moneytracker -i data.csv --category-config my_categories.json

# Console-only (no Excel)
./build/moneytracker -i data.csv --no-spreadsheet
```

**Complete CLI documentation**: See [README.md](README.md)

## Installation

### Step 1: Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libboost-all-dev libxlsxwriter-dev cmake ninja-build libgtk-3-dev
```

**macOS (Homebrew):**
```bash
brew install boost libxlsxwriter cmake ninja gtk+3
```

### Step 2: Build

```bash
cd MoneyTracker
mkdir -p build
cd build
cmake -G Ninja ..
ninja
```

Two executables created:
- `./moneytracker` - CLI tool
- `./moneytracker-gui` - GUI tool

## Prepare Your Data

### 1. Export CSV from Your Bank

Most banks support CSV export in their web portal:
1. Log into your bank account
2. Look for "Download" or "Export" option
3. Select date range (typically last 1-3 months)
4. Download as CSV file

### 2. Check CSV Format

Your bank CSV should have columns like:
- **Bank Format**: Date, Description, Debit, Credit, Balance
- **Generic Format**: Date, Description, Amount

Example (Bank Format):
```
Date,Description,Debit,Credit,Balance
01/15/2024,Direct Deposit,,2500.00,5000.00
01/18/2024,Grocery Store,45.25,,4954.75
```

Example (Generic Format):
```
Date,Description,Amount
01/15/2024,Direct Deposit,2500.00
01/18/2024,Grocery Store,-45.25
```

MoneyTracker automatically detects the format!

## First Run Example

### Using GUI:
```bash
./build/moneytracker-gui
```
1. Click "Add File..." → select `checking.csv`
2. Enter account name: "My Checking"
3. Click "Choose Output..." → select where to save Excel
4. Click "Analyze" button
5. Review results in tabs
6. Click "Open Generated Excel" to view file

### Using CLI:
```bash
./build/moneytracker -i checking.csv -a "My Checking" -v
```

## What You Get

### Console Output:
```
========== MoneyTracker Configuration ==========
Input files: checking.csv
Account names: My Checking
Output file: budget_report.xlsx
=============================================

Total Income:    $5000.00
Total Expenses:  $150.00
Net Change:      $4850.00

=== SPENDING BY CATEGORY ===
Groceries        : $45.25
Dining           : $32.50
Entertainment    : $25.00
...

Generating Excel spreadsheet...
OK Spreadsheet generated successfully: budget_report.xlsx
```

### Excel Spreadsheet:
Professional multi-sheet workbook with:
- **Summary** - Key metrics and totals
- **Categories** - Breakdown by category
- **Monthly** - Trends over time  
- **Transactions** - Complete ledger

## Common Tasks

### View only console report
```bash
./build/moneytracker -i data.csv --no-spreadsheet
```

### Analyze multiple accounts together
```bash
./build/moneytracker \
  -i checking.csv -a "Checking" \
  -i savings.csv -a "Savings" \
  -i credit_card.csv -a "Amex" \
  -o combined_budget.xlsx
```

### Use custom category rules
Edit `data/categories.json` with your own merchant keywords, then:
```bash
./build/moneytracker -i data.csv --category-config data/categories.json
```

### Run analysis every month
```bash
# Create a script: monthly_analysis.sh
#!/bin/bash
DATE=$(date +%Y-%m)
./build/moneytracker \
  -i checking_$DATE.csv -a "Checking" \
  -o budget_$DATE.xlsx

# Run it: bash monthly_analysis.sh
```

## Troubleshooting

### Q: "GTK3 not found" on Linux
**A:** Install GTK3 dev files:
```bash
sudo apt-get install libgtk-3-dev
```

### Q: "libboost not found"
**A:** Install Boost libraries:
```bash
sudo apt-get install libboost-all-dev
```

### Q: CSV file won't load
**A:** Check:
- File exists at the path you specified
- File encoding is UTF-8
- Date format is MM/DD/YYYY, DD/MM/YYYY, or YYYY-MM-DD
- CSV is valid (proper commas and quotes)

### Q: Categories not matching
**A:** 
- Check `data/categories.json` keywords match your bank's transaction descriptions
- Keywords are case-insensitive
- Try adding more keywords to the JSON file
- Example: If "WHOLE FOODS" appears in descriptions, add "WHOLE FOODS" to Groceries keywords

### Q: Excel file can't be opened
**A:**
- Ensure output directory exists and is writable
- Try saving to a different location
- Check you have enough disk space
- Verify the file path doesn't contain special characters

## Examples by Use Case

### Personal Finance Tracking
```bash
./build/moneytracker -i my_checking.csv -a "Personal" -v
# Review console output for monthly spending
```

### Multiple Account Consolidation
```bash
./build/moneytracker \
  -i checking.csv -a "Checking" \
  -i savings.csv -a "Savings" \
  -i discover.csv -a "Discover Card" \
  -o full_financial_picture.xlsx
```

### Business Expense Analysis
```bash
# Use business account CSV
./build/moneytracker -i business_account.csv \
  --category-config business_categories.json \
  -a "Business" \
  -o business_expenses.xlsx
```

### Tax Preparation
```bash
# Export full year data
./build/moneytracker -i 2024_full_year.csv -a "All" \
  -o tax_summary.xlsx
# Share Excel with accountant
```

### Monthly Reporting
```bash
# Create monthly reports automatically
for month in {01..12}; do
  ./build/moneytracker \
    -i 2024-$month.csv \
    -a "2024-$month" \
    -o budget_2024-$month.xlsx
done
```

## Next Steps

1. **GUI Users**: 
   - Start with `./build/moneytracker-gui`
   - Read [GUI_USER_GUIDE.md](GUI_USER_GUIDE.md) for detailed help

2. **CLI Users**:
   - Try `./build/moneytracker --help` for all options
   - Read [README.md](README.md) for complete documentation

3. **Customization**:
   - Edit `data/categories.json` to customize categories
   - Create scripts for automated monthly analysis

## Resources

- **User Guide**: [README.md](README.md)
- **GUI Guide**: [GUI_USER_GUIDE.md](GUI_USER_GUIDE.md)
- **Getting Started**: [GETTING_STARTED.md](GETTING_STARTED.md)
- **Recent Improvements**: [IMPROVEMENTS.md](IMPROVEMENTS.md)
- **Build Status**: [BUILD_STATUS.md](BUILD_STATUS.md)

## Support & Feedback

For help:
1. Check the relevant user guide above
2. Review sample CSV files in `data/` directory
3. Run with `--verbose` flag for detailed output
4. Check `categories.json` for category configuration options

---

**MoneyTracker is ready to use!** 
Choose GUI for visual experience or CLI for automation.
