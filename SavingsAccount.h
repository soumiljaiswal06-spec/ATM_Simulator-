#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

#include "Account.h"

// Savings account: lower withdrawal limit and a minimum balance must be kept.
class SavingsAccount : public Account {
public:
    SavingsAccount(int accountNumber, const std::string& holderName,
                   const std::string& pin, double openingBalance);

    std::string getAccountType() const override;
    double getWithdrawalLimit() const override;
    double getAvailableFunds() const override;
};

#endif
