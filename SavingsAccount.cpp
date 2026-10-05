#include "SavingsAccount.h"

namespace {
    const double WITHDRAWAL_LIMIT = 20000.0;
    const double MINIMUM_BALANCE  = 500.0;
}

SavingsAccount::SavingsAccount(int accountNumber, const std::string& holderName,
                               const std::string& pin, double openingBalance)
    : Account(accountNumber, holderName, pin, openingBalance) {}

std::string SavingsAccount::getAccountType() const { return "Savings"; }

double SavingsAccount::getWithdrawalLimit() const { return WITHDRAWAL_LIMIT; }

// Money that must stay in the account cannot be withdrawn.
double SavingsAccount::getAvailableFunds() const {
    double available = getBalance() - MINIMUM_BALANCE;
    return (available > 0) ? available : 0;
}
