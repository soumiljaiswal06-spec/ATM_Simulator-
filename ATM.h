#ifndef ATM_H
#define ATM_H

#include "Bank.h"

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

#endif
