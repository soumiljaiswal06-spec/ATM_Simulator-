# ATM Simulator — Design, Report, Viva & Demo Guide

Everything here matches the code in this folder (compiled with `g++ -std=c++11 -Wall -Wextra -pedantic`, zero warnings; run clean under AddressSanitizer/UBSan).

---

# A. Architecture

## Stage 1 — Requirement analysis
**Functional:** login, 3-attempt PIN check with blocking, 8-option menu, balance, withdraw, deposit, transfer, change PIN, mini statement, account details, logout/re-login.
**OOP:** classes, encapsulation, abstraction, inheritance, polymorphism, constructors, destructor, overloading, static, STL.
**Validation:** letters instead of numbers, negative/zero amounts, bad menu choice, unknown account, wrong PIN, bad PIN format, insufficient balance, bad/self receiver.
**Edge cases:** blocked account trying again, savings minimum balance, current-account overdraft, empty mini statement, input stream closing (Ctrl+D), "12abc" typed as a number.

## Stage 2 — Classes and responsibilities

| Class | Responsibility | Key data | Key functions |
|---|---|---|---|
| `Account` (abstract) | One bank account: PIN, balance, history, common rules | `accountNumber`, `holderName`, `pin`, `balance`, `history`, `failedAttempts`, `blocked`, static `totalAccounts` | `verifyPin`, `deposit`, `withdraw`, `transferOut/In`, `changePin`, `showMiniStatement`, `displayDetails` |
| `SavingsAccount` | Savings rules | — | overrides `getAccountType`, `getWithdrawalLimit` (20,000), `getAvailableFunds` (balance − 500 minimum) |
| `CurrentAccount` | Current rules | — | overrides same three (limit 50,000, balance + 10,000 overdraft) |
| `Transaction` | One history entry | `type`, signed `amount`, `balanceAfter`, `timestamp`, static `totalTransactions` | `display()`, getters |
| `Bank` | Owns all accounts; operations involving two accounts | `vector<Account*>` | `findAccount`, `transfer`, destructor deletes accounts |
| `ATM` | User interface and menu flow only | `Bank`, `currentAccount` | `run`, `login`, `sessionMenu`, `withdrawMoney`, ... |
| `Utils` (namespace) | Reusable input validation + formatting | — | `readInt`, `readDouble`, `readMenuChoice`, `formatMoney` |

## Class diagram (ASCII)
```
 main() ──> ATM ─────────────> Bank
             │  currentAccount   │ owns (vector<Account*>)
             │  (pointer)        ▼
             │              ┌──────────────┐        ┌─────────────┐
             │              │   Account    │ 1    * │ Transaction │
             │              │  <<abstract>>│───────>│             │
             │              └──────┬───────┘ history└─────────────┘
             │                     │ inherits (is-a)
             │          ┌──────────┴───────────┐
             │   SavingsAccount          CurrentAccount
             └──> Utils (input/output helpers, used by ATM, Account, Transaction)
```

## Where each idea is used
- **Encapsulation:** all `Account` and `Transaction` data is `private`. Balance and PIN can only change through `deposit/withdraw/transferOut/transferIn/changePin`, which validate first. The PIN has no getter, so it can never be displayed.
- **Abstraction:** `ATM::withdrawMoney()` just calls `currentAccount->withdraw(amount, message)`. It does not know about the limit, the minimum balance, or how the history entry is made.
- **Inheritance:** `SavingsAccount` and `CurrentAccount` inherit everything common (PIN, history, deposit...) from `Account` and only supply what truly differs. Purpose: the two account types genuinely have different rules.
- **Polymorphism:** `Account::withdraw()` calls the pure virtual `getWithdrawalLimit()` and `getAvailableFunds()`. The same line of code gives different results for a Savings object and a Current object (e.g. Rs. 50,000 is refused for Savings but allowed for Current). `Bank` stores `Account*` pointing to both types.

## How a transaction flows (withdrawal)
```
User picks 2 ─> ATM::withdrawMoney() ─> Utils::readDouble() (validates it is a number)
   ─> Account::withdraw(amount, msg)
         1. amount > 0 ?
         2. amount <= getWithdrawalLimit()   [virtual -> Savings/Current value]
         3. amount <= getAvailableFunds()    [virtual -> Savings/Current value]
         4. balance -= amount ; record("Withdrawal") -> history.push_back(Transaction)
   <─ true/false + message ─ ATM prints success or the reason for failure
```
Transfer: `ATM` -> `Bank::transfer()` finds receiver, rejects self-transfer, calls `sender.transferOut()` then `receiver->transferIn()`. Each side records its own history entry.

---

# H. OOP Concept Mapping (Stage 3)

| OOP Concept | Where Used |
|---|---|
| Class | `Account`, `SavingsAccount`, `CurrentAccount`, `Transaction`, `Bank`, `ATM` |
| Object | `ATM atm;` in `main`; `Bank` inside `ATM`; `new SavingsAccount(...)` in `Bank::createDemoAccounts`; `Transaction` objects in each account's vector |
| Encapsulation | Private members in `Account` (balance, pin, history, attempts) and `Transaction`; public member functions as the only way in |
| Abstraction | ATM calls `withdraw()`, `deposit()`, `transfer()` without knowing the internals; `Account` is an abstract class |
| Inheritance | `SavingsAccount : public Account`, `CurrentAccount : public Account` |
| Polymorphism | Pure virtual `getAccountType/getWithdrawalLimit/getAvailableFunds` overridden in derived classes; called through `Account*` (in `Bank`, `ATM`) and inside `Account::withdraw` |
| Constructor | `Account(...)` with initializer list; derived constructors call the base constructor; `Transaction(...)`; `Bank(name)`; `ATM()` |
| Destructor | `Bank::~Bank()` deletes the `new`-created accounts (needed). `Account::~Account()` is `virtual` so `delete` through `Account*` is safe, and it decrements the static counter |
| Function Overloading | `Account::showMiniStatement()` / `showMiniStatement(int count)`; `Account::displayDetails()` / `displayDetails(bool showBalance)` (login uses `displayDetails(false)`, menu option 7 uses `displayDetails()`) |
| Function Overriding | The three virtual functions in `SavingsAccount` and `CurrentAccount` (marked `override`) |
| Static Member | `Account::totalAccounts` (one count shared by all accounts), `Transaction::totalTransactions` (shared transaction counter); both printed when the program exits |
| STL | `vector<Account*>` in `Bank`, `vector<Transaction>` in `Account`, `string`, `iomanip` (`setw`, `setprecision`), `sstream` for validation |

---

# C. Source code & D. Compilation
Source files are in this folder: `main.cpp, ATM.h/.cpp, Bank.h/.cpp, Account.h/.cpp, SavingsAccount.h/.cpp, CurrentAccount.h/.cpp, Transaction.h/.cpp, Utils.h/.cpp`.

```
g++ -std=c++11 -Wall -Wextra -o atm *.cpp
./atm
```
Windows (MinGW): `g++ -std=c++11 -o atm.exe *.cpp` then `atm.exe`. Visual Studio: create an empty console project and add all files.

---

# E. Test cases (all executed — see `test_input.txt` / `sample_output.txt`)

| # | Area | Input | Expected result | Result |
|---|---|---|---|---|
| 1 | Login | 1001 / 1234 | "Login successful" | Pass |
| 2 | Login | 1001 / 0000 | "Invalid PIN. Attempts remaining: 2" | Pass |
| 3 | Login | 1002 with 3 wrong PINs | "Account blocked due to multiple incorrect PIN attempts." | Pass |
| 4 | Login | 1002 again | Blocked message immediately | Pass |
| 5 | Login | 5555 | "Account not found." | Pass |
| 6 | Menu | 9 at login menu / "abc" | Invalid choice / Invalid input, re-prompt | Pass |
| 7 | Withdraw | 5000 | Success, balance reduced | Pass |
| 8 | Withdraw | 20000+ on Savings | Refused: limit Rs. 20,000 | Pass |
| 9 | Withdraw | -500 / 0 | "Amount must be greater than zero." | Pass |
| 10 | Withdraw | "abc" | "Invalid input", asks again | Pass |
| 11 | Withdraw | more than available (Current, 1003: 50000 twice) | Second one: "Insufficient balance. Available for withdrawal: Rs. 20000.00" (balance 10,000 + overdraft 10,000) | Pass |
| 12 | Deposit | 3000 | Success, balance increased | Pass |
| 13 | Deposit | -10 | Rejected | Pass |
| 14 | Transfer | 1002, 2000 | Success, sender reduced, receiver credited | Pass |
| 15 | Transfer | receiver 9999 | "Receiver account not found." | Pass |
| 16 | Transfer | receiver = own account | "You cannot transfer money to your own account." | Pass |
| 17 | Transfer | 999999 | "Insufficient balance..." | Pass |
| 18 | PIN | wrong current PIN | "Incorrect current PIN. PIN not changed." | Pass |
| 19 | PIN | new PIN "12" | "Invalid PIN format..." | Pass |
| 20 | PIN | new/confirm differ | "PINs do not match." | Pass |
| 21 | PIN | new = old | "New PIN cannot be the same as the old PIN." | Pass |
| 22 | PIN | valid change to 4321 | "PIN changed successfully."; old PIN 1234 then rejected, 4321 accepted | Pass |
| 23 | Statement | option 6 | Last transactions + current balance | Pass |
| 24 | Details | option 7 | Number, name, type, balance (no PIN) | Pass |
| 25 | Logout | option 8 | "Successfully logged out." and back to login; other account can log in | Pass |

---

# F. README
See `README.md`.

---

# G. Project Report Content

**1. Title:** ATM Simulator Using C++ and Object-Oriented Programming

**2. Abstract**
This project is a console-based ATM Simulator developed in C++ using Object-Oriented Programming. It models a bank with Savings and Current accounts and lets a user log in with an account number and PIN, check balance, withdraw, deposit, transfer money, change PIN, view a mini statement and view account details. The design uses classes, encapsulation, inheritance, polymorphism, constructors, destructors, function overloading, static members and STL containers. All data is stored in memory and is lost when the program ends.

**3. Introduction**
An Automated Teller Machine (ATM) lets bank customers perform basic banking operations without visiting a branch. The customer proves identity with a card and PIN, then chooses services from a menu. This simulator imitates that behaviour in software so that OOP concepts can be learnt on a real-world problem.

**4. Problem Statement**
Beginners often write programs as one long `main()` function, which is hard to read and extend. This project solves a familiar real-world problem (an ATM) while organising the solution into objects that each own their data and behaviour.

**5. Objectives**
- Build a working menu-driven ATM in C++.
- Apply the main OOP concepts meaningfully.
- Validate all user input so the program does not crash.
- Keep a transaction history per account.
- Keep the code modular across multiple files.

**6. Technologies Used**
C++ (C++11) with the Standard Library only; compiled with g++; console interface.

**7. OOP Concepts Used** (with code reference)
- *Encapsulation:* private `balance`, `pin`, `history` in `Account`; changed only via `deposit()`, `withdraw()`, etc.
- *Abstraction:* `ATM::withdrawMoney()` calls `Account::withdraw()` without knowing its internals; `Account` itself is abstract.
- *Inheritance:* `SavingsAccount` and `CurrentAccount` derive from `Account`.
- *Polymorphism:* `getWithdrawalLimit()` and `getAvailableFunds()` are pure virtual in `Account` and overridden in each subclass; `Account::withdraw()` uses them.
- *Constructors:* parameterized constructors with initializer lists; derived constructors call the base constructor.
- *Destructor:* `Bank::~Bank()` frees dynamically created accounts; `Account`'s destructor is virtual.
- *Overloading:* `showMiniStatement()` / `showMiniStatement(int)`, `displayDetails()` / `displayDetails(bool)`.
- *Static members:* `Account::totalAccounts`, `Transaction::totalTransactions`.
- *STL:* `vector`, `string`, `iomanip`, `sstream`.

**8. System Design**
`ATM` (user interface) uses `Bank`. `Bank` owns many `Account` objects (Savings or Current). Each `Account` owns many `Transaction` objects. `Utils` provides input validation. (See class diagram above.)

**9. Features**
Login, PIN verification with blocking after 3 failures, check balance, withdraw (with per-type limits), deposit, transfer, change PIN, mini statement, account details, logout, input validation.

**10. Algorithm / Working**
1. Show welcome menu; user chooses Login or Exit.
2. Read account number; if not found or blocked, return to welcome menu.
3. Read PIN; wrong PIN reduces remaining attempts; after 3 wrong attempts the account is blocked.
4. On success show the main menu and repeat: read choice, call the matching function, print result.
5. Each money operation validates the amount, checks the rules, updates the balance and records a `Transaction`.
6. Logout returns to step 1 so another account can log in.

**11. Testing**
See section E (25 test cases, all passed).

**12. Advantages**
Simple and easy to explain; modular multi-file design; robust input validation; each account type's rules live in its own class, so a new account type needs no change to the ATM code.

**13. Limitations**
No connection to a real bank; no database or file storage, so all changes are lost when the program closes; console interface only; demo authentication (PIN stored as plain text in memory, no encryption); blocking lasts only for the running session; money is stored as `double`, which is acceptable for learning but not exact enough for real banking; no daily withdrawal limit.

**14. Future Scope** (not implemented)
Database or file storage, GUI, real authentication and encryption, receipt printing, admin panel, persistent transaction history, integration with real banking APIs.

**15. Conclusion**
The ATM Simulator shows how real-world entities map naturally to classes. Working on it gave practical experience of encapsulation, inheritance and polymorphism and of writing validated, modular C++ code. The design can be extended with persistence and a GUI in future.

---

# I. Viva Questions and Answers

1. **What is OOP?** A way of programming where we model things as objects that combine data and the functions that work on that data. Main ideas: encapsulation, abstraction, inheritance, polymorphism.
2. **Why did you use classes?** Each real-world thing (account, transaction, bank, ATM) becomes a class, so code is organised, reusable and easier to maintain than one long `main()`.
3. **What is encapsulation?** Binding data and functions together and hiding the data using `private`, so it is changed only through controlled public functions.
4. **Where is encapsulation used in your project?** In `Account`: `balance`, `pin`, `history` are private; `withdraw()`, `deposit()` and `changePin()` validate before changing them. There is no getter for the PIN.
5. **Why are data members private?** So no other part of the program can set the balance to any value or read the PIN. All changes go through functions that check the rules.
6. **What is abstraction?** Showing only what is needed and hiding how it works. The ATM calls `withdraw(amount)` without knowing how the balance and history are updated.
7. **What is inheritance?** A new class reuses members of an existing class using `: public Base`.
8. **Why did you use inheritance?** Savings and Current accounts share most things (PIN, history, deposit) but differ in rules. The common part is in `Account`; only the differences are in the derived classes. Duplicate code is avoided.
9. **What is polymorphism?** One interface, many behaviours. The same function call behaves differently depending on the actual object type.
10. **Where is runtime polymorphism used?** `Account::withdraw()` calls `getWithdrawalLimit()` and `getAvailableFunds()`. For a Savings object they return 20,000 and balance−500; for a Current object 50,000 and balance+10,000. `Bank` holds `Account*` for both types.
11. **Why are some functions virtual?** So the version in the real object's class is called at runtime, even through a base-class pointer. Without `virtual`, the base version would be used.
12. **What is a pure virtual function / abstract class?** A virtual function with `= 0` and no body in the base class. A class with one is abstract: you can't create objects of it, only of derived classes that implement it. `Account` is abstract because an account must be Savings or Current.
13. **What is a constructor?** A special function with the class name, called automatically when an object is created, used to initialize data. Example: `Account(int, string, string, double)`.
14. **What is a destructor?** A function (`~ClassName`) called automatically when an object is destroyed, used for cleanup. `Bank::~Bank()` deletes the accounts created with `new`.
15. **Why is `Account`'s destructor virtual?** We delete accounts through an `Account*`. A virtual destructor ensures the derived class's destructor also runs and the object is destroyed properly.
16. **Why use a vector?** It grows automatically, is safe, and has useful functions like `push_back` and `size()`. Better than a fixed C array. Used for the accounts and for each account's history.
17. **What is function overloading?** Several functions with the same name but different parameters. Example: `displayDetails()` and `displayDetails(bool showBalance)`. Decided at compile time.
18. **What is function overriding?** A derived class gives its own version of a base class virtual function with the same signature. Example: `SavingsAccount::getWithdrawalLimit()`.
19. **Difference between overloading and overriding?** Overloading: same class, same name, different parameters, compile-time. Overriding: base and derived class, same signature, needs `virtual`, runtime.
20. **What is a static member and why did you use one?** One variable shared by all objects of the class. `Account::totalAccounts` counts how many accounts exist, which belongs to the class and not to a single account.
21. **What happens after three wrong PIN attempts?** `verifyPin()` counts failures; at 3 it sets `blocked = true`. The ATM prints "Account blocked due to multiple incorrect PIN attempts." and further login attempts for that account are refused until the program restarts.
22. **How is transaction history stored?** Every deposit, withdrawal and transfer creates a `Transaction` object (type, signed amount, time, balance after) pushed into that account's `vector<Transaction>`. The mini statement prints the last 5.
23. **How does money transfer work?** `Bank::transfer()` finds the receiver, rejects a missing receiver or self-transfer, then calls `sender.transferOut()` (checks amount and funds, deducts) and `receiver->transferIn()` (adds). Both accounts record a history entry.
24. **What happens when the balance is insufficient?** `withdraw()`/`transferOut()` compare the amount with `getAvailableFunds()`, return `false` with a message, and nothing changes. For Savings the available amount excludes the Rs. 500 minimum balance.
25. **How do you stop the program crashing on wrong input?** `Utils::readInt/readDouble` read a whole line, try to convert it with `stringstream`, reject leftovers like "12abc", and ask again.
26. **What are the limitations of your project?** Data is only in memory and lost on exit; no database; console only; plain-text PIN; `double` for money; the block only lasts for the session.
27. **How would you connect this to a database?** Add a data-access class that loads accounts and transactions from the database (or a file) on start and saves after every operation. `Bank` would use it instead of `createDemoAccounts()`. The other classes stay nearly the same.
28. **How would you improve security?** Store a hash of the PIN (not the PIN), keep failed attempts and blocking in persistent storage, add card/OTP verification, session timeout and audit logs.
29. **What would you change for a real banking application?** Use exact money types (integer paise / decimal), database transactions so a transfer is all-or-nothing, concurrency control, encryption, real authentication, regulatory logging.
30. **Why does `Bank` use raw pointers?** Polymorphism needs pointers (or references) to the base class. A `vector<Account>` would slice derived objects. The `Bank` destructor deletes them, and copying `Bank` is disabled so pointers are never double-deleted.
31. **Why is `main()` so small?** All logic is in the classes. `main()` creates one `ATM` object and calls `run()`.

---

# J. Suggestions for demonstrating to the professor

1. **Start with the diagram** (above) and say in one line what each class is responsible for.
2. **Run the program** with 1001 / 1234. Show: check balance → withdraw 5000 → deposit 3000 → transfer 2000 to 1002 → mini statement.
3. **Show validation:** type letters, -500, 0, a wrong account, a bad PIN. Nothing crashes.
4. **Show blocking:** three wrong PINs on 1002 → blocked → try again → still blocked.
5. **Show polymorphism live:** withdraw 25000 on 1001 (Savings → refused, limit 20,000); then log in as 1003 (Current) and withdraw 25000 (allowed). Open `Account::withdraw()` and point to the virtual calls, then `SavingsAccount.cpp`/`CurrentAccount.cpp`.
6. **Show encapsulation:** point out the private section of `Account.h` and that there is no PIN getter.
7. **Show a static member:** exit the program; it prints the account and transaction counts.
8. **Show you can replay everything:** `./atm < test_input.txt`.
9. Be ready to say the limitations honestly (in-memory only, plain-text PIN, `double` for money).
10. Be ready for a small change request, e.g. "add a `PremiumAccount`": create a new derived class with its own limit/overdraft and add one line in `Bank::createDemoAccounts()`. No change to `ATM` is needed, which proves the polymorphic design.
