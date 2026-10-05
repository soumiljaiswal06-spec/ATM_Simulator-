/*
 * ATM Simulator - single-file version (C++11, standard library only)
 * Compile:  g++ -std=c++11 -o atm ATM_Simulator.cpp
 * Run:      ./atm        (Windows: atm.exe)
 * DEMO accounts: 1001/1234 (Savings), 1002/5678 (Savings), 1003/4321 (Current)
 */
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ---------- Utils (declaration) ----------
// Reusable console helpers (input validation + formatting).
// Kept in a namespace because these functions need no object state.
namespace Utils {
    std::string readLine(const std::string& prompt);            // reads one full line
    int         readInt(const std::string& prompt);             // loops until a valid whole number
    double      readDouble(const std::string& prompt);          // loops until a valid number
    int         readMenuChoice(const std::string& prompt, int minChoice, int maxChoice);
    std::string formatMoney(double value);                      // e.g. "Rs. 25000.00"
    void        printHeader(const std::string& title);          // boxed heading
    void        printLine(char symbol = '-', int width = 44);
}

// ---------- Transaction (declaration) ----------
// One entry in an account's history (deposit, withdrawal, transfer ...).
class Transaction {
private:
    std::string type;
    double amount;                  // positive = money in, negative = money out
    double balanceAfter;
    std::string timestamp;

    static int totalTransactions;   // shared by ALL Transaction objects

public:
    Transaction(const std::string& type, double amount, double balanceAfter);

    std::string getType() const;
    double getAmount() const;
    double getBalanceAfter() const;
    std::string getTimestamp() const;

    void display() const;           // prints one row of the mini statement

    static int getTotalTransactions();
};

// ---------- Account (declaration) ----------
// Abstract base class. Every real account is a SavingsAccount or CurrentAccount.
// The rules that DIFFER between account types are pure virtual functions.
class Account {
private:                                    // encapsulation: nobody outside can touch these
    int accountNumber;
    std::string holderName;
    std::string pin;                        // string so that PINs like "0123" are possible
    double balance;
    std::vector<Transaction> history;
    int failedAttempts;
    bool blocked;

    static int totalAccounts;               // shared counter of live accounts

    void record(const std::string& type, double signedAmount);   // adds to history

public:
    Account(int accountNumber, const std::string& holderName,
            const std::string& pin, double openingBalance);
    virtual ~Account();                     // virtual: deleting via Account* must clean up properly

    // ---- read-only access ----
    int getAccountNumber() const;
    std::string getHolderName() const;
    double getBalance() const;
    bool isBlocked() const;
    int getRemainingAttempts() const;

    // ---- PIN handling ----
    bool verifyPin(const std::string& enteredPin);      // counts wrong attempts, blocks after 3
    bool checkPin(const std::string& enteredPin) const; // plain check, no counting
    bool changePin(const std::string& newPin, std::string& message);
    static bool isValidPinFormat(const std::string& pin);

    // ---- banking operations (each returns success, message explains failure) ----
    bool deposit(double amount, std::string& message);
    bool withdraw(double amount, std::string& message);
    bool transferOut(double amount, std::string& message);
    void transferIn(double amount);

    // ---- display ----
    void showMiniStatement() const;                 // last 5 transactions
    void showMiniStatement(int count) const;        // overloaded: last 'count' transactions
    void displayDetails() const;                    // full details
    void displayDetails(bool showBalance) const;    // overloaded: balance optional

    // ---- rules that depend on the account type ----
    virtual std::string getAccountType() const = 0;
    virtual double getWithdrawalLimit() const = 0;  // max per ATM withdrawal
    virtual double getAvailableFunds() const = 0;   // how much can be taken out right now

    static int getTotalAccounts();
};

// ---------- SavingsAccount (declaration) ----------
// Savings account: lower withdrawal limit and a minimum balance must be kept.
class SavingsAccount : public Account {
public:
    SavingsAccount(int accountNumber, const std::string& holderName,
                   const std::string& pin, double openingBalance);

    std::string getAccountType() const override;
    double getWithdrawalLimit() const override;
    double getAvailableFunds() const override;
};

// ---------- CurrentAccount (declaration) ----------
// Current account: higher withdrawal limit and an overdraft facility.
class CurrentAccount : public Account {
public:
    CurrentAccount(int accountNumber, const std::string& holderName,
                   const std::string& pin, double openingBalance);

    std::string getAccountType() const override;
    double getWithdrawalLimit() const override;
    double getAvailableFunds() const override;
};

// ---------- Bank (declaration) ----------
// The Bank owns all accounts and handles things that involve more than one account.
class Bank {
private:
    std::string bankName;
    std::vector<Account*> accounts;     // base-class pointers -> polymorphism

    Bank(const Bank&);                  // copying a Bank would copy raw pointers, so forbid it
    Bank& operator=(const Bank&);

    void createDemoAccounts();

public:
    explicit Bank(const std::string& bankName);
    ~Bank();                            // deletes every account created with 'new'

    std::string getBankName() const;
    Account* findAccount(int accountNumber) const;     // nullptr if not found
    bool transfer(Account& sender, int receiverNumber, double amount, std::string& message);
};

// ---------- ATM (declaration) ----------
// The ATM machine: talks to the user, and asks the Bank/Account objects to do the work.
class ATM {
private:
    Bank bank;
    Account* currentAccount;            // account of the logged-in user (nullptr if none)

    bool login();
    void sessionMenu();
    void showSessionMenu() const;

    void checkBalance() const;
    void withdrawMoney();
    void depositMoney();
    void transferMoney();
    void changePin();

public:
    ATM();
    void run();
};

// ---------- Utils (implementation) ----------
namespace Utils {

string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {          // input stream closed (Ctrl+D / Ctrl+Z / end of file)
        cout << "\n\nInput ended. Exiting ATM Simulator.\n";
        exit(0);
    }
    return line;
}

int readInt(const string& prompt) {
    while (true) {
        istringstream iss(readLine(prompt));
        int value;
        char extra;
        // must read a number AND have nothing else after it ("12abc" is rejected)
        if ((iss >> value) && !(iss >> extra))
            return value;
        cout << "Invalid input. Please enter a whole number.\n";
    }
}

double readDouble(const string& prompt) {
    while (true) {
        istringstream iss(readLine(prompt));
        double value;
        char extra;
        if ((iss >> value) && !(iss >> extra) && isfinite(value) && fabs(value) <= 1e9)
            return value;
        cout << "Invalid input. Please enter a valid numeric amount.\n";
    }
}

int readMenuChoice(const string& prompt, int minChoice, int maxChoice) {
    while (true) {
        int choice = readInt(prompt);
        if (choice >= minChoice && choice <= maxChoice)
            return choice;
        cout << "Invalid choice. Please select " << minChoice << " to " << maxChoice << ".\n";
    }
}

string formatMoney(double value) {
    ostringstream out;
    out << "Rs. " << fixed << setprecision(2) << value;
    return out.str();
}

void printLine(char symbol, int width) {
    cout << string(width, symbol) << "\n";
}

void printHeader(const string& title) {
    const int width = 44;
    int pad = (width - static_cast<int>(title.size())) / 2;
    printLine('=', width);
    cout << string(pad > 0 ? pad : 0, ' ') << title << "\n";
    printLine('=', width);
}

} // namespace Utils

// ---------- Transaction (implementation) ----------
int Transaction::totalTransactions = 0;     // static member must be defined once

Transaction::Transaction(const string& type, double amount, double balanceAfter)
    : type(type), amount(amount), balanceAfter(balanceAfter) {
    time_t now = time(nullptr);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d-%b %I:%M %p", localtime(&now));
    timestamp = buffer;
    ++totalTransactions;
}

string Transaction::getType() const { return type; }
double Transaction::getAmount() const { return amount; }
double Transaction::getBalanceAfter() const { return balanceAfter; }
string Transaction::getTimestamp() const { return timestamp; }

void Transaction::display() const {
    string sign = (amount >= 0) ? "+" : "-";
    cout << left  << setw(18) << timestamp
         << setw(14) << type
         << right << setw(12) << (sign + Utils::formatMoney(fabs(amount))) << "\n";
}

int Transaction::getTotalTransactions() { return totalTransactions; }

// ---------- Account (implementation) ----------
namespace {
    const int MAX_PIN_ATTEMPTS = 3;
}

int Account::totalAccounts = 0;

Account::Account(int accountNumber, const string& holderName,
                 const string& pin, double openingBalance)
    : accountNumber(accountNumber), holderName(holderName), pin(pin),
      balance(openingBalance), failedAttempts(0), blocked(false) {
    ++totalAccounts;
}

Account::~Account() {
    --totalAccounts;
}

void Account::record(const string& type, double signedAmount) {
    history.push_back(Transaction(type, signedAmount, balance));
}

// ---------- getters ----------
int Account::getAccountNumber() const { return accountNumber; }
string Account::getHolderName() const { return holderName; }
double Account::getBalance() const { return balance; }
bool Account::isBlocked() const { return blocked; }
int Account::getRemainingAttempts() const { return MAX_PIN_ATTEMPTS - failedAttempts; }
int Account::getTotalAccounts() { return totalAccounts; }

// ---------- PIN ----------
bool Account::checkPin(const string& enteredPin) const {
    return enteredPin == pin;
}

bool Account::verifyPin(const string& enteredPin) {
    if (blocked)
        return false;
    if (checkPin(enteredPin)) {
        failedAttempts = 0;
        return true;
    }
    ++failedAttempts;
    if (failedAttempts >= MAX_PIN_ATTEMPTS)
        blocked = true;
    return false;
}

bool Account::isValidPinFormat(const string& pin) {
    if (pin.size() != 4)
        return false;
    for (char c : pin)
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

bool Account::changePin(const string& newPin, string& message) {
    if (!isValidPinFormat(newPin)) {
        message = "PIN must be exactly 4 digits.";
        return false;
    }
    if (newPin == pin) {
        message = "New PIN cannot be the same as the old PIN.";
        return false;
    }
    pin = newPin;
    return true;
}

// ---------- operations ----------
bool Account::deposit(double amount, string& message) {
    if (amount <= 0) {
        message = "Amount must be greater than zero.";
        return false;
    }
    balance += amount;
    record("Deposit", amount);
    return true;
}

bool Account::withdraw(double amount, string& message) {
    if (amount <= 0) {
        message = "Amount must be greater than zero.";
        return false;
    }
    // getWithdrawalLimit() and getAvailableFunds() are virtual:
    // the derived class decides the value (runtime polymorphism).
    if (amount > getWithdrawalLimit()) {
        message = "Amount exceeds the withdrawal limit of " +
                  Utils::formatMoney(getWithdrawalLimit()) + " per transaction.";
        return false;
    }
    if (amount > getAvailableFunds()) {
        message = "Insufficient balance. Available for withdrawal: " +
                  Utils::formatMoney(getAvailableFunds());
        return false;
    }
    balance -= amount;
    record("Withdrawal", -amount);
    return true;
}

bool Account::transferOut(double amount, string& message) {
    if (amount <= 0) {
        message = "Amount must be greater than zero.";
        return false;
    }
    if (amount > getAvailableFunds()) {
        message = "Insufficient balance. Available for transfer: " +
                  Utils::formatMoney(getAvailableFunds());
        return false;
    }
    balance -= amount;
    record("Transfer Out", -amount);
    return true;
}

void Account::transferIn(double amount) {
    balance += amount;
    record("Transfer In", amount);
}

// ---------- display ----------
void Account::showMiniStatement() const {
    showMiniStatement(5);
}

void Account::showMiniStatement(int count) const {
    Utils::printHeader("MINI STATEMENT");
    cout << left << setw(18) << "Date/Time" << setw(14) << "Type"
         << right << setw(12) << "Amount" << "\n";
    Utils::printLine();

    if (history.empty()) {
        cout << "No transactions yet.\n";
    } else {
        size_t total = history.size();
        size_t start = (total > static_cast<size_t>(count)) ? total - count : 0;
        for (size_t i = start; i < total; ++i)
            history[i].display();
    }
    Utils::printLine();
    cout << "Current Balance: " << Utils::formatMoney(balance) << "\n";
    Utils::printLine('=');
}

void Account::displayDetails() const {
    displayDetails(true);
}

void Account::displayDetails(bool showBalance) const {
    Utils::printHeader("ACCOUNT DETAILS");
    cout << left << setw(16) << "Account Number" << ": " << accountNumber << "\n"
         << setw(16) << "Name" << ": " << holderName << "\n"
         << setw(16) << "Account Type" << ": " << getAccountType() << "\n";
    if (showBalance)
        cout << setw(16) << "Balance" << ": " << Utils::formatMoney(balance) << "\n";
    Utils::printLine('=');
}

// ---------- SavingsAccount (implementation) ----------
namespace {
    const double SAVINGS_WITHDRAWAL_LIMIT = 20000.0;
    const double MINIMUM_BALANCE  = 500.0;
}

SavingsAccount::SavingsAccount(int accountNumber, const std::string& holderName,
                               const std::string& pin, double openingBalance)
    : Account(accountNumber, holderName, pin, openingBalance) {}

std::string SavingsAccount::getAccountType() const { return "Savings"; }

double SavingsAccount::getWithdrawalLimit() const { return SAVINGS_WITHDRAWAL_LIMIT; }

// Money that must stay in the account cannot be withdrawn.
double SavingsAccount::getAvailableFunds() const {
    double available = getBalance() - MINIMUM_BALANCE;
    return (available > 0) ? available : 0;
}

// ---------- CurrentAccount (implementation) ----------
namespace {
    const double CURRENT_WITHDRAWAL_LIMIT = 50000.0;
    const double OVERDRAFT_LIMIT  = 10000.0;   // balance may go down to -10000
}

CurrentAccount::CurrentAccount(int accountNumber, const std::string& holderName,
                               const std::string& pin, double openingBalance)
    : Account(accountNumber, holderName, pin, openingBalance) {}

std::string CurrentAccount::getAccountType() const { return "Current"; }

double CurrentAccount::getWithdrawalLimit() const { return CURRENT_WITHDRAWAL_LIMIT; }

double CurrentAccount::getAvailableFunds() const {
    return getBalance() + OVERDRAFT_LIMIT;
}

// ---------- Bank (implementation) ----------
Bank::Bank(const string& bankName) : bankName(bankName) {
    createDemoAccounts();
}

Bank::~Bank() {
    for (size_t i = 0; i < accounts.size(); ++i)
        delete accounts[i];             // virtual destructor makes this safe
}

void Bank::createDemoAccounts() {
    // DEMO DATA ONLY
    accounts.push_back(new SavingsAccount(1001, "Rahul Sharma", "1234", 25000));
    accounts.push_back(new SavingsAccount(1002, "Priya Verma",  "5678", 40000));
    accounts.push_back(new CurrentAccount(1003, "Arjun Mehta",  "4321", 60000));
}

string Bank::getBankName() const { return bankName; }

Account* Bank::findAccount(int accountNumber) const {
    for (size_t i = 0; i < accounts.size(); ++i)
        if (accounts[i]->getAccountNumber() == accountNumber)
            return accounts[i];
    return nullptr;
}

bool Bank::transfer(Account& sender, int receiverNumber, double amount, string& message) {
    Account* receiver = findAccount(receiverNumber);
    if (receiver == nullptr) {
        message = "Receiver account not found.";
        return false;
    }
    if (receiver == &sender) {
        message = "You cannot transfer money to your own account.";
        return false;
    }
    if (!sender.transferOut(amount, message))   // checks amount and balance
        return false;
    receiver->transferIn(amount);
    return true;
}

// ---------- ATM (implementation) ----------
ATM::ATM() : bank("ABC Bank"), currentAccount(nullptr) {}

void ATM::run() {
    while (true) {
        Utils::printHeader("ATM SIMULATOR");
        cout << "\nWelcome to " << bank.getBankName() << " ATM\n\n"
             << "1. Login\n2. Exit\n\n";
        int choice = Utils::readMenuChoice("Enter choice: ", 1, 2);

        if (choice == 2) {
            cout << "\nThank you for using " << bank.getBankName() << " ATM. Goodbye!\n";
            cout << "(Accounts in system: " << Account::getTotalAccounts()
                 << ", transactions recorded: " << Transaction::getTotalTransactions() << ")\n";
            return;
        }
        if (login())
            sessionMenu();
    }
}

bool ATM::login() {
    Utils::printHeader("LOGIN");
    int number = Utils::readInt("Enter account number: ");
    Account* account = bank.findAccount(number);

    if (account == nullptr) {
        cout << "Account not found.\n\n";
        return false;
    }
    if (account->isBlocked()) {
        cout << "Account blocked due to multiple incorrect PIN attempts.\n\n";
        return false;
    }

    while (!account->isBlocked()) {
        string pin = Utils::readLine("Enter 4-digit PIN: ");
        if (account->verifyPin(pin)) {
            currentAccount = account;
            cout << "\nLogin successful. Welcome, " << account->getHolderName() << "!\n";
            return true;
        }
        if (account->isBlocked())
            break;
        cout << "Invalid PIN. Attempts remaining: " << account->getRemainingAttempts() << "\n";
    }
    cout << "Account blocked due to multiple incorrect PIN attempts.\n\n";
    return false;
}

void ATM::showSessionMenu() const {
    cout << "\n";
    Utils::printHeader("ATM SIMULATOR");
    cout << "1. Check Balance\n2. Withdraw Money\n3. Deposit Money\n4. Transfer Money\n"
         << "5. Change PIN\n6. Mini Statement\n7. Account Details\n8. Logout\n\n";
}

void ATM::sessionMenu() {
    currentAccount->displayDetails(false);      // overloaded version: hides the balance
    bool loggedIn = true;
    while (loggedIn) {
        showSessionMenu();
        switch (Utils::readMenuChoice("Enter your choice: ", 1, 8)) {
            case 1: checkBalance(); break;
            case 2: withdrawMoney(); break;
            case 3: depositMoney(); break;
            case 4: transferMoney(); break;
            case 5: changePin(); break;
            case 6: currentAccount->showMiniStatement(); break;
            case 7: currentAccount->displayDetails(); break;
            case 8:
                currentAccount = nullptr;
                cout << "\nSuccessfully logged out.\n\n";
                loggedIn = false;
                break;
        }
    }
}

void ATM::checkBalance() const {
    cout << "\nCurrent Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
}

void ATM::withdrawMoney() {
    double amount = Utils::readDouble("Enter amount to withdraw: Rs. ");
    string message;
    if (currentAccount->withdraw(amount, message)) {
        cout << "\nTransaction Successful!\n\n"
             << "Amount Withdrawn: " << Utils::formatMoney(amount) << "\n"
             << "Remaining Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
    } else {
        cout << "\nTransaction Failed: " << message << "\n";
    }
}

void ATM::depositMoney() {
    double amount = Utils::readDouble("Enter amount to deposit: Rs. ");
    string message;
    if (currentAccount->deposit(amount, message)) {
        cout << "\nDeposit Successful!\n\n"
             << "Deposited Amount: " << Utils::formatMoney(amount) << "\n"
             << "Updated Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
    } else {
        cout << "\nDeposit Failed: " << message << "\n";
    }
}

void ATM::transferMoney() {
    int receiver = Utils::readInt("Enter receiver account number: ");
    double amount = Utils::readDouble("Enter transfer amount: Rs. ");
    string message;
    if (bank.transfer(*currentAccount, receiver, amount, message)) {
        cout << "\nTransfer Successful!\n\n"
             << "Transferred: " << Utils::formatMoney(amount) << "\n"
             << "Receiver Account: " << receiver << "\n"
             << "Remaining Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
    } else {
        cout << "\nTransfer Failed: " << message << "\n";
    }
}

void ATM::changePin() {
    string current = Utils::readLine("Enter current PIN: ");
    if (!currentAccount->checkPin(current)) {
        cout << "\nIncorrect current PIN. PIN not changed.\n";
        return;
    }
    string newPin = Utils::readLine("Enter new 4-digit PIN: ");
    if (!Account::isValidPinFormat(newPin)) {
        cout << "\nInvalid PIN format. PIN must be exactly 4 digits.\n";
        return;
    }
    string confirm = Utils::readLine("Confirm new PIN: ");
    if (confirm != newPin) {
        cout << "\nPINs do not match. PIN not changed.\n";
        return;
    }
    string message;
    if (currentAccount->changePin(newPin, message))
        cout << "\nPIN changed successfully.\n";
    else
        cout << "\n" << message << "\n";
}

// ---------- main (implementation) ----------
int main() {
    ATM atm;
    atm.run();
    return 0;
}
