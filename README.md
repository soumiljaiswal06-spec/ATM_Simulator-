# ATM Simulator (C++ / OOP)

A console-based ATM Simulator written in C++ for an Object-Oriented Programming college project.

## Description
The program simulates an ATM of a fictional bank ("ABC Bank"). A user logs in with an account number and PIN and can check balance, withdraw, deposit, transfer money, change PIN, view a mini statement and view account details. All data lives in memory; it resets when the program closes.

## Objective
To demonstrate classes, encapsulation, abstraction, inheritance, runtime polymorphism, constructors, destructors, function overloading, static members and the STL in one small but complete program.

## Features
- Login with account number + PIN (3 attempts, then the account is blocked for the session)
- Check balance, withdraw, deposit
- Transfer between accounts (receiver checked, no self-transfer)
- Change PIN (current PIN verified, 4-digit format, new PIN must differ)
- Mini statement (last 5 transactions with date/time) and account details (PIN never shown)
- Logout and log in to another account without restarting
- Input validation: letters, negative/zero amounts, bad menu choices, bad PIN format never crash the program
- Account-type rules: Savings (limit Rs. 20,000 per withdrawal, Rs. 500 minimum balance) and Current (limit Rs. 50,000, overdraft up to Rs. 10,000)

## Technologies
C++11, Standard Library only (`iostream`, `string`, `vector`, `iomanip`, `sstream`, `ctime`, `cmath`). Console only.

## OOP concepts demonstrated
Classes/objects, encapsulation, abstraction, inheritance (`Account` -> `SavingsAccount`/`CurrentAccount`), runtime polymorphism (virtual functions + `Account*`), constructors, (virtual) destructors, function overloading, static members, STL `vector`. See `PROJECT_DOCUMENT.md` for the exact location of each in the code.

## Class structure
```
                     Utils (helper functions)
                              
  ATM ──has──> Bank ──owns──> Account* (abstract) ──has──> vector<Transaction>
                                  ▲
                      ┌───────────┴───────────┐
               SavingsAccount          CurrentAccount
```

## How to compile
Put all files in one folder, then:
```
g++ -std=c++11 -Wall -Wextra -o atm *.cpp
```
(Works with g++/clang++, MinGW, or Visual Studio — add all `.cpp` files to the project.)

## How to run
```
./atm            # Linux / macOS
atm.exe          # Windows
```
To replay the full test session: `./atm < test_input.txt` (compare with `sample_output.txt`).

## Demo login credentials (DEMO ACCOUNTS ONLY — not real data)
| Account | Name | PIN | Type | Opening balance |
|---|---|---|---|---|
| 1001 | Rahul Sharma | 1234 | Savings | Rs. 25,000 |
| 1002 | Priya Verma | 5678 | Savings | Rs. 40,000 |
| 1003 | Arjun Mehta | 4321 | Current | Rs. 60,000 |

## Sample output
`sample_output.txt` contains the output of a full test run (`test_input.txt`): login success/failure, blocked account, all transaction types, validation errors, mini statement and logout.

## Notes
- Currency is printed as `Rs.` because many Windows consoles cannot display the rupee symbol. To use it, change one line in `Utils::formatMoney()` (Utils.cpp).
- Wrong *current* PIN inside "Change PIN" is rejected but does not count toward the 3-attempt block (only login attempts do).

## Future improvements
File storage for accounts/history, admin menu, receipt printing, hashed PINs, daily withdrawal limits, GUI, real database.
