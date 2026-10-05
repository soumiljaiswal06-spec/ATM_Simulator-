#ifndef BANK_H
#define BANK_H

#include <string>
#include <vector>
#include "Account.h"

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

#endif
