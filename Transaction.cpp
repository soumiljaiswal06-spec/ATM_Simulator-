#include "Transaction.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cmath>

using namespace std;

int Transaction::totalTransactions = 0;     // static member must be defined once

Transaction::Transaction(const string& type, double amount, double balanceAfter)
    : type(type), amount(amount), balanceAfter(balanceAfter) {
    time_t now = time(nullptr);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d-%b %I:%M %p", localtime(&now));
    timestamp = buffer;
    ++totalTransactions;
}

string Transaction::getType() const { return type; }
double Transaction::getAmount() const { return amount; }
double Transaction::getBalanceAfter() const { return balanceAfter; }
string Transaction::getTimestamp() const { return timestamp; }

void Transaction::display() const {
    string sign = (amount >= 0) ? "+" : "-";
    cout << left  << setw(18) << timestamp
         << setw(14) << type
         << right << setw(12) << (sign + Utils::formatMoney(fabs(amount))) << "\n";
}

int Transaction::getTotalTransactions() { return totalTransactions; }
