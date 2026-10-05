#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>
#include "Transaction.h"

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

#endif
