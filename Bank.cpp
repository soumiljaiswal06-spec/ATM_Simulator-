#include "Bank.h"
#include "SavingsAccount.h"
#include "CurrentAccount.h"

using namespace std;

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
