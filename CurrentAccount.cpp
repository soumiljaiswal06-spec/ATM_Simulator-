#include "CurrentAccount.h"

namespace {
    const double WITHDRAWAL_LIMIT = 50000.0;
    const double OVERDRAFT_LIMIT  = 10000.0;   // balance may go down to -10000
}

CurrentAccount::CurrentAccount(int accountNumber, const std::string& holderName,
                               const std::string& pin, double openingBalance)
    : Account(accountNumber, holderName, pin, openingBalance) {}

std::string CurrentAccount::getAccountType() const { return "Current"; }

double CurrentAccount::getWithdrawalLimit() const { return WITHDRAWAL_LIMIT; }

double CurrentAccount::getAvailableFunds() const {
    return getBalance() + OVERDRAFT_LIMIT;
}
