#include "Account.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <cctype>

using namespace std;

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
