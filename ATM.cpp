#include "ATM.h"
#include "Utils.h"
#include <iostream>

using namespace std;

ATM::ATM() : bank("ABC Bank"), currentAccount(nullptr) {}

void ATM::run() {
    while (true) {
        Utils::printHeader("ATM SIMULATOR");
        cout << "\nWelcome to " << bank.getBankName() << " ATM\n\n"
             << "1. Login\n2. Exit\n\n";
        int choice = Utils::readMenuChoice("Enter choice: ", 1, 2);

        if (choice == 2) {
            cout << "\nThank you for using " << bank.getBankName() << " ATM. Goodbye!\n";
            cout << "(Accounts in system: " << Account::getTotalAccounts()
                 << ", transactions recorded: " << Transaction::getTotalTransactions() << ")\n";
            return;
        }
        if (login())
            sessionMenu();
    }
}

bool ATM::login() {
    Utils::printHeader("LOGIN");
    int number = Utils::readInt("Enter account number: ");
    Account* account = bank.findAccount(number);

    if (account == nullptr) {
        cout << "Account not found.\n\n";
        return false;
    }
    if (account->isBlocked()) {
        cout << "Account blocked due to multiple incorrect PIN attempts.\n\n";
        return false;
    }

    while (!account->isBlocked()) {
        string pin = Utils::readLine("Enter 4-digit PIN: ");
        if (account->verifyPin(pin)) {
            currentAccount = account;
            cout << "\nLogin successful. Welcome, " << account->getHolderName() << "!\n";
            return true;
        }
        if (account->isBlocked())
            break;
        cout << "Invalid PIN. Attempts remaining: " << account->getRemainingAttempts() << "\n";
    }
    cout << "Account blocked due to multiple incorrect PIN attempts.\n\n";
    return false;
}

void ATM::showSessionMenu() const {
    cout << "\n";
    Utils::printHeader("ATM SIMULATOR");
    cout << "1. Check Balance\n2. Withdraw Money\n3. Deposit Money\n4. Transfer Money\n"
         << "5. Change PIN\n6. Mini Statement\n7. Account Details\n8. Logout\n\n";
}

void ATM::sessionMenu() {
    currentAccount->displayDetails(false);      // overloaded version: hides the balance
    bool loggedIn = true;
    while (loggedIn) {
        showSessionMenu();
        switch (Utils::readMenuChoice("Enter your choice: ", 1, 8)) {
            case 1: checkBalance(); break;
            case 2: withdrawMoney(); break;
            case 3: depositMoney(); break;
            case 4: transferMoney(); break;
            case 5: changePin(); break;
            case 6: currentAccount->showMiniStatement(); break;
            case 7: currentAccount->displayDetails(); break;
            case 8:
                currentAccount = nullptr;
                cout << "\nSuccessfully logged out.\n\n";
                loggedIn = false;
                break;
        }
    }
}

void ATM::checkBalance() const {
    cout << "\nCurrent Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
}

void ATM::withdrawMoney() {
    double amount = Utils::readDouble("Enter amount to withdraw: Rs. ");
    string message;
    if (currentAccount->withdraw(amount, message)) {
        cout << "\nTransaction Successful!\n\n"
             << "Amount Withdrawn: " << Utils::formatMoney(amount) << "\n"
             << "Remaining Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
    } else {
        cout << "\nTransaction Failed: " << message << "\n";
    }
}

void ATM::depositMoney() {
    double amount = Utils::readDouble("Enter amount to deposit: Rs. ");
    string message;
    if (currentAccount->deposit(amount, message)) {
        cout << "\nDeposit Successful!\n\n"
             << "Deposited Amount: " << Utils::formatMoney(amount) << "\n"
             << "Updated Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
    } else {
        cout << "\nDeposit Failed: " << message << "\n";
    }
}

void ATM::transferMoney() {
    int receiver = Utils::readInt("Enter receiver account number: ");
    double amount = Utils::readDouble("Enter transfer amount: Rs. ");
    string message;
    if (bank.transfer(*currentAccount, receiver, amount, message)) {
        cout << "\nTransfer Successful!\n\n"
             << "Transferred: " << Utils::formatMoney(amount) << "\n"
             << "Receiver Account: " << receiver << "\n"
             << "Remaining Balance: " << Utils::formatMoney(currentAccount->getBalance()) << "\n";
    } else {
        cout << "\nTransfer Failed: " << message << "\n";
    }
}

void ATM::changePin() {
    string current = Utils::readLine("Enter current PIN: ");
    if (!currentAccount->checkPin(current)) {
        cout << "\nIncorrect current PIN. PIN not changed.\n";
        return;
    }
    string newPin = Utils::readLine("Enter new 4-digit PIN: ");
    if (!Account::isValidPinFormat(newPin)) {
        cout << "\nInvalid PIN format. PIN must be exactly 4 digits.\n";
        return;
    }
    string confirm = Utils::readLine("Confirm new PIN: ");
    if (confirm != newPin) {
        cout << "\nPINs do not match. PIN not changed.\n";
        return;
    }
    string message;
    if (currentAccount->changePin(newPin, message))
        cout << "\nPIN changed successfully.\n";
    else
        cout << "\n" << message << "\n";
}
