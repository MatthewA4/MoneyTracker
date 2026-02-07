# MoneyTracker GUI Application User Guide

## Overview

`moneytracker-gui` is a graphical user interface for MoneyTracker that makes it easy to import bank CSV files and generate professional budget analysis spreadsheets without using the command line.

## System Requirements

- **Operating System**: Linux with GTK3 (3.0+)
- **Display Server**: X11 or Wayland
- **Dependencies**: libgtk-3-0 (installed automatically with libgtk-3-dev)

## Installation

The GUI binary is compiled automatically if GTK3 development files are available:

```bash
sudo apt-get install libgtk-3-dev
cd MoneyTracker/build
cmake -G Ninja ..
ninja
```

Two executables will be created:
- `moneytracker` - Command-line interface (no display needed)
- `moneytracker-gui` - Graphical user interface (GUI)

## Running the Application

### From Terminal
```bash
./build/moneytracker-gui
```

### From File Manager
Double-click the `moneytracker-gui` executable in your file manager.

## Interface Overview

The GUI presents a tabbed interface with 6 main sections:

### 1. **Input Tab** - Add Your Bank Files

This is where you start. Here you can:

- **Add CSV Files**: Click "Add File..." to select one or more bank CSV files
- **Account Names**: For each file, enter a meaningful account name (e.g., "Checking", "Savings", "Credit Card")
- **Output Location**: Choose where to save the generated Excel spreadsheet
- **File List**: See all added files with their account names and remove any as needed

**Workflow**:
1. Click "Add File..." button
2. Navigate to and select your bank CSV file
3. Enter an account name in the dialog
4. Repeat for additional files (optional)
5. Click "Choose Output..." to specify where to save the Excel file
6. Proceed to the Configuration tab

### 2. **Configuration Tab** - Customize Categories

Configure how transactions are categorized:

- **Category Configuration File**: Path to `categories.json` file
  - Default: `data/categories.json`
  - Leave blank to use default categories
  - Click "Browse..." to select a custom file

- **Default Categories Include**:
  - Groceries
  - Dining
  - Transportation
  - Utilities
  - Entertainment
  - Healthcare
  - Shopping
  - Salary/Income
  - Fees
  - Other

**How It Works**:
The system reads your `categories.json` file and automatically matches transaction descriptions to categories using merchant keywords. For example, if you paid "WHOLE FOODS MARKET", it will be matched to "Groceries".

**Customizing Categories**:
Edit `data/categories.json` to add your own merchant keywords:
```json
[
  {
    "name": "Groceries",
    "keywords": ["WHOLE FOODS", "KROGER", "SAFEWAY", "TRADER JOES"]
  },
  ...
]
```

### 3. **Summary Tab** - High-Level Overview

After running analysis, this tab shows:

- **Total Income**: Sum of all deposits and credits
- **Total Expenses**: Sum of all withdrawals and debits
- **Net Change**: Difference between income and expenses
- **Account List**: All accounts processed with their transaction counts

Use this to quickly assess your overall financial picture.

### 4. **Categories Tab** - Spending Breakdown

See how much you spent in each category:

```
Groceries: $450.25
Dining: $230.50
Transportation: $180.75
...
```

This helps identify where your money goes and find areas to cut back if needed.

### 5. **Monthly Tab** - Trends Over Time

Analyze your spending patterns by month:

```
2024-01: $1,250.50
2024-02: $1,180.25
2024-03: $1,520.75
```

Perfect for identifying seasonal spending patterns and detecting unusual months.

### 6. **Transactions Tab** - Detailed Ledger

View all transactions in a formatted table:

```
Date       | Description                   | Amount  | Category       | Account
-----------|-------------------------------|---------|----------------|----------
2024-01-05 | Direct Deposit ACME Corp      | 2500.00 | Salary/Income | Checking
2024-01-07 | WHOLE FOODS #1234             | 45.25   | Groceries     | Checking
...
```

Useful for verification and spot-checking specific transactions.

## Workflow Example

### Example: Analyzing Personal Finances

1. **Input Tab**:
   - Add `checking.csv` as "Checking"
   - Add `savings.csv` as "Savings"
   - Choose output: `monthly_budget.xlsx`

2. **Configuration Tab**:
   - Use default categories (or browse to custom file)

3. **Click "Analyze"**:
   - The application processes all files
   - Results populate in tabs 3-6
   - Shows completion message

4. **Review Results**:
   - **Summary**: See total income ($4,500) vs expenses ($1,250)
   - **Categories**: Identify top spending categories
   - **Monthly**: Check for spending spikes
   - **Transactions**: Verify specific entries

5. **Export**:
   - Click "Open Generated Excel" to view the spreadsheet
   - Share with accountant or financial advisor

## File Formats

### Supported CSV Formats

The application auto-detects and supports two CSV format styles:

**Bank Format** (Debit/Credit columns):
```
Date,Description,Debit,Credit,Balance
01/05/2024,Direct Deposit,0,2500.00,5000.00
01/07/2024,Grocery Store,45.25,0,4954.75
```

**Generic Format** (Amount column):
```
Date,Description,Amount
01/05/2024,Direct Deposit,2500.00
01/07/2024,Grocery Store,-45.25
```

### categories.json Format

```json
[
  {
    "name": "Groceries",
    "keywords": ["WHOLE FOODS", "KROGER", "SAFEWAY", "TRADER JOES", "SPROUTS"]
  },
  {
    "name": "Dining",
    "keywords": ["RESTAURANT", "CAFE", "PIZZA", "BURGER", "STARBUCKS"]
  },
  ...
]
```

## Output: Generated Excel Spreadsheet

The application creates a professional Excel spreadsheet with multiple sheets:

1. **Summary** - Overview metrics and totals
2. **Categories** - Spending breakdown by category
3. **Monthly** - Monthly trends
4. **Transactions** - Complete ledger with all details

All sheets are formatted with:
- Professional styling
- Currency formatting
- Clear headers
- Proper column widths

## Troubleshooting

### "GTK3 not found" Error
Install GTK3 development files:
```bash
sudo apt-get install libgtk-3-dev
```

### CSV File Not Recognized
- Check file encoding (should be UTF-8)
- Verify date format is one of:
  - MM/DD/YYYY
  - DD/MM/YYYY
  - YYYY-MM-DD
- Ensure decimal values use either `.` or `,` as separator

### Categories Not Matching
- Check `categories.json` file path is correct
- Verify category keywords match your bank's transaction descriptions
- Use wildcards by editing JSON keywords

### Excel File Won't Open
- Ensure output directory exists and is writable
- Try saving to a different location
- Verify sufficient disk space available

## Command-Line Alternative

For automated/scripted use, the CLI version (`moneytracker`) offers more flexibility:

```bash
./build/moneytracker \
  -i checking.csv -a "Checking" \
  -i savings.csv -a "Savings" \
  -o budget.xlsx \
  --category-config custom_categories.json \
  --verbose
```

See [README.md](README.md) for complete CLI documentation.

## Advanced Features

### Custom Category Matching

Edit `data/categories.json` to add more specific keywords:

```json
{
  "name": "Coffee",
  "keywords": ["STARBUCKS", "COFFEE BEAN", "LOCAL CAFE", "ESPRESSO"]
}
```

The matching is case-insensitive and matches any transaction containing these keywords.

### Multiple Files Analysis

Combine data from multiple accounts:
- Different bank accounts
- Multiple credit cards
- Investment accounts (if CSV available)

All will be aggregated in the analysis.

### Recurring Transactions

The GUI displays all transactions. To identify recurring items:
1. Go to **Transactions Tab**
2. Look for repeated descriptions on the same day each month
3. Calculate monthly/annual costs

## Tips & Best Practices

1. **Organization**: Create separate categories.json files for different analysis needs
2. **Backup**: Keep original CSV files for audit trails
3. **Regular Updates**: Run monthly analysis to track spending trends
4. **Accounts**: Use clear account names (avoid abbreviations)
5. **Archives**: Save generated Excel files with date in filename (e.g., `budget_2024-02.xlsx`)

## Performance

- **Small files** (< 1,000 transactions): < 1 second
- **Medium files** (1,000-10,000): < 5 seconds
- **Large files** (> 10,000): < 30 seconds

Processing is done in-memory for speed.

## Privacy & Security

- All processing happens locally on your computer
- No data is sent to external servers
- CSV files remain on your system
- Excel output is created only where you specify

## Support & Feedback

For issues or feature requests:
1. Check this guide for solutions
2. Review `IMPROVEMENTS.md` for recent changes
3. Check `README.md` for additional documentation

## See Also

- [README.md](README.md) - Complete project documentation
- [GETTING_STARTED.md](GETTING_STARTED.md) - Setup instructions
- [IMPROVEMENTS.md](IMPROVEMENTS.md) - Recent enhancements
- [CLI User Guide](README.md#cli-usage) - Command-line interface
