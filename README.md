 # Bank Management System (C++)

A console-based Bank Management System built in C++ using Object-Oriented Programming (OOP) concepts. Supports multiple accounts with persistent data storage using file handling.

## Features
- Create a new account
- Deposit money
- Withdraw money
- Check balance (PIN protected)
- Change PIN
- View account information
- List all active accounts
- Close an account
- **Data persistence** — account data is saved to `accounts.txt` and reloaded automatically, so it is not lost when the program closes
- **Multiple accounts** — the system can manage many customer accounts, not just one
- **Input validation** — invalid inputs (letters instead of numbers, etc.) no longer crash the program
- PINs are stored as hashed values rather than plain text

## Technologies Used
- C++ (OOP: classes, encapsulation)
- File Handling (`fstream`)
- Standard Template Library (STL) — `vector`, `sstream`

## How to Compile & Run

```bash
g++ -std=c++17 BankManagementSystem.cpp -o bank
./bank
```

On Windows (with g++/MinGW):
```bash
g++ -std=c++17 BankManagementSystem.cpp -o bank.exe
bank.exe
```

## How It Works
1. Run the program — it automatically loads existing accounts from `accounts.txt` (if the file exists).
2. Use the menu to create accounts, deposit/withdraw funds, or manage existing accounts.
3. Every change (deposit, withdrawal, new account, etc.) is immediately saved back to `accounts.txt`.

## Possible Future Improvements
- Move to a proper database (e.g. SQLite/MySQL) instead of a flat text file
- Add transaction history logging
- Add an admin panel for bank staff
- Use a stronger cryptographic hash (e.g. SHA-256) for PIN storage

## Author
Samreen Yaqub — [github.com/s-samreen](https://github.com/s-samreen)
