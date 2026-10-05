#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

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

#endif
