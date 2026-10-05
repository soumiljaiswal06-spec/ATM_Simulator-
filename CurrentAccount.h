#ifndef CURRENTACCOUNT_H
#define CURRENTACCOUNT_H

#include "Account.h"

// Current account: higher withdrawal limit and an overdraft facility.
class CurrentAccount : public Account {
public:
    CurrentAccount(int accountNumber, const std::string& holderName,
                   const std::string& pin, double openingBalance);

    std::string getAccountType() const override;
    double getWithdrawalLimit() const override;
    double getAvailableFunds() const override;
};

#endif
