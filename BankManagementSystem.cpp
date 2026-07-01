/*
    Bank Management System
    -----------------------
    A console-based banking application in C++ using Object-Oriented
    Programming. Supports multiple accounts with persistent storage
    (data is saved to a file so it is not lost when the program closes).

    Features:
      - Create account
      - Deposit / Withdraw money
      - Check balance (PIN protected)
      - Change PIN
      - View account information
      - Close account
      - List all accounts
      - Data persistence using file handling (accounts.txt)
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <functional>

using namespace std;

const string DATA_FILE = "accounts.txt";

// ---------- Simple hash helper for PIN storage ----------
// Note: this uses std::hash for demonstration purposes.
// A real production system would use a proper cryptographic hash (e.g. bcrypt/SHA-256).
string hashPin(const string &pin)
{
    hash<string> hasher;
    size_t hashed = hasher(pin);
    stringstream ss;
    ss << hashed;
    return ss.str();
}

// ---------- Account class ----------
class Account
{
public:
    string name;
    long long accountNo;
    string pinHash;
    double balance;
    double limit;
    bool active;

    Account() : accountNo(0), balance(0), limit(50000), active(true) {}

    Account(string n, long long accNo, string pHash, double bal, double lim)
        : name(n), accountNo(accNo), pinHash(pHash), balance(bal), limit(lim), active(true) {}

    // Serialize account to a single line for file storage
    string toFileLine() const
    {
        stringstream ss;
        ss << accountNo << "|" << name << "|" << pinHash << "|"
           << balance << "|" << limit << "|" << (active ? 1 : 0);
        return ss.str();
    }

    // Parse an account back from a stored line
    static Account fromFileLine(const string &line)
    {
        Account a;
        stringstream ss(line);
        string field;

        getline(ss, field, '|'); a.accountNo = stoll(field);
        getline(ss, field, '|'); a.name = field;
        getline(ss, field, '|'); a.pinHash = field;
        getline(ss, field, '|'); a.balance = stod(field);
        getline(ss, field, '|'); a.limit = stod(field);
        getline(ss, field, '|'); a.active = (field == "1");

        return a;
    }
};

// ---------- Bank class: manages all accounts ----------
class Bank
{
private:
    vector<Account> accounts;

    void loadAccounts()
    {
        accounts.clear();
        ifstream file(DATA_FILE);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line))
        {
            if (!line.empty())
                accounts.push_back(Account::fromFileLine(line));
        }
        file.close();
    }

    void saveAccounts()
    {
        ofstream file(DATA_FILE, ios::trunc);
        for (const auto &acc : accounts)
            file << acc.toFileLine() << "\n";
        file.close();
    }

    int findAccountIndex(long long accNo)
    {
        for (size_t i = 0; i < accounts.size(); i++)
            if (accounts[i].accountNo == accNo && accounts[i].active)
                return (int)i;
        return -1;
    }

    // Reads an integer safely, re-prompting on invalid input
    long long readLongLong(const string &prompt)
    {
        long long value;
        while (true)
        {
            cout << prompt;
            cin >> value;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }

    double readDouble(const string &prompt)
    {
        double value;
        while (true)
        {
            cout << prompt;
            cin >> value;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }

public:
    Bank() { loadAccounts(); }

    void createAccount()
    {
        Account a;
        cout << "\nEnter Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, a.name);

        a.accountNo = readLongLong("Enter Account Number: ");

        if (findAccountIndex(a.accountNo) != -1)
        {
            cout << "An account with this number already exists!\n";
            return;
        }

        string pin;
        cout << "Set PIN (4 digits): ";
        cin >> pin;
        a.pinHash = hashPin(pin);

        a.balance = 0;
        a.limit = 50000;
        a.active = true;

        accounts.push_back(a);
        saveAccounts();

        cout << "\nAccount Created Successfully!\n";
    }

    void depositMoney()
    {
        long long accNo = readLongLong("Enter Account Number: ");
        int idx = findAccountIndex(accNo);
        if (idx == -1) { cout << "Account not found!\n"; return; }

        double amount = readDouble("Enter Amount to Deposit: ");
        if (amount > 0)
        {
            accounts[idx].balance += amount;
            saveAccounts();
            cout << "Deposit Successful! Current Balance: " << accounts[idx].balance << endl;
        }
        else
        {
            cout << "Invalid Amount!\n";
        }
    }

    void withdrawMoney()
    {
        long long accNo = readLongLong("Enter Account Number: ");
        int idx = findAccountIndex(accNo);
        if (idx == -1) { cout << "Account not found!\n"; return; }

        double amount = readDouble("Enter Amount to Withdraw: ");
        if (amount > 0 && amount <= accounts[idx].balance)
        {
            accounts[idx].balance -= amount;
            saveAccounts();
            cout << "Withdrawal Successful! Remaining Balance: " << accounts[idx].balance << endl;
        }
        else
        {
            cout << "Insufficient Balance or Invalid Amount!\n";
        }
    }

    void checkBalance()
    {
        long long accNo = readLongLong("Enter Account Number: ");
        int idx = findAccountIndex(accNo);
        if (idx == -1) { cout << "Account not found!\n"; return; }

        string pin;
        cout << "Enter PIN: ";
        cin >> pin;

        if (hashPin(pin) == accounts[idx].pinHash)
            cout << "Current Balance: " << accounts[idx].balance << endl;
        else
            cout << "Wrong PIN!\n";
    }

    void changePin()
    {
        long long accNo = readLongLong("Enter Account Number: ");
        int idx = findAccountIndex(accNo);
        if (idx == -1) { cout << "Account not found!\n"; return; }

        string oldPin, newPin;
        cout << "Enter Old PIN: ";
        cin >> oldPin;

        if (hashPin(oldPin) == accounts[idx].pinHash)
        {
            cout << "Enter New PIN: ";
            cin >> newPin;
            accounts[idx].pinHash = hashPin(newPin);
            saveAccounts();
            cout << "PIN Changed Successfully!\n";
        }
        else
        {
            cout << "Incorrect PIN!\n";
        }
    }

    void accountInformation()
    {
        long long accNo = readLongLong("Enter Account Number: ");
        int idx = findAccountIndex(accNo);
        if (idx == -1) { cout << "Account not found!\n"; return; }

        cout << "\n----- Account Information -----\n";
        cout << "Name: " << accounts[idx].name << endl;
        cout << "Account Number: " << accounts[idx].accountNo << endl;
        cout << "Balance: " << accounts[idx].balance << endl;
        cout << "Account Limit: " << accounts[idx].limit << endl;
    }

    void listAccounts()
    {
        cout << "\n----- All Accounts -----\n";
        bool any = false;
        for (const auto &acc : accounts)
        {
            if (acc.active)
            {
                cout << "Account No: " << acc.accountNo << " | Name: " << acc.name
                     << " | Balance: " << acc.balance << endl;
                any = true;
            }
        }
        if (!any) cout << "No accounts found.\n";
    }

    void closeAccount()
    {
        long long accNo = readLongLong("Enter Account Number: ");
        int idx = findAccountIndex(accNo);
        if (idx == -1) { cout << "Account not found!\n"; return; }

        string pin;
        cout << "Enter PIN to confirm closure: ";
        cin >> pin;

        if (hashPin(pin) != accounts[idx].pinHash)
        {
            cout << "Incorrect PIN! Account closure cancelled.\n";
            return;
        }

        cout << "Account Closed Successfully!\n";
        cout << "Refund Amount: " << accounts[idx].balance << endl;

        accounts[idx].active = false;
        accounts[idx].balance = 0;
        saveAccounts();
    }
};

int main()
{
    Bank bank;
    int choice;

    do
    {
        cout << "\n===== BANK MANAGEMENT SYSTEM =====\n";
        cout << "1. Create New Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Check Balance\n";
        cout << "5. Change PIN\n";
        cout << "6. Account Information\n";
        cout << "7. List All Accounts\n";
        cout << "8. Close Account\n";
        cout << "9. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice, please try again.\n";
            continue;
        }

        switch (choice)
        {
            case 1: bank.createAccount(); break;
            case 2: bank.depositMoney(); break;
            case 3: bank.withdrawMoney(); break;
            case 4: bank.checkBalance(); break;
            case 5: bank.changePin(); break;
            case 6: bank.accountInformation(); break;
            case 7: bank.listAccounts(); break;
            case 8: bank.closeAccount(); break;
            case 9: cout << "Thank You For Using Bank Management System!" << endl; break;
            default: cout << "Invalid Choice!" << endl;
        }

    } while (choice != 9);

    return 0;
}
