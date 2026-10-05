#include "Utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

using namespace std;

namespace Utils {

string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {          // input stream closed (Ctrl+D / Ctrl+Z / end of file)
        cout << "\n\nInput ended. Exiting ATM Simulator.\n";
        exit(0);
    }
    return line;
}

int readInt(const string& prompt) {
    while (true) {
        istringstream iss(readLine(prompt));
        int value;
        char extra;
        // must read a number AND have nothing else after it ("12abc" is rejected)
        if ((iss >> value) && !(iss >> extra))
            return value;
        cout << "Invalid input. Please enter a whole number.\n";
    }
}

double readDouble(const string& prompt) {
    while (true) {
        istringstream iss(readLine(prompt));
        double value;
        char extra;
        if ((iss >> value) && !(iss >> extra) && isfinite(value) && fabs(value) <= 1e9)
            return value;
        cout << "Invalid input. Please enter a valid numeric amount.\n";
    }
}

int readMenuChoice(const string& prompt, int minChoice, int maxChoice) {
    while (true) {
        int choice = readInt(prompt);
        if (choice >= minChoice && choice <= maxChoice)
            return choice;
        cout << "Invalid choice. Please select " << minChoice << " to " << maxChoice << ".\n";
    }
}

string formatMoney(double value) {
    ostringstream out;
    out << "Rs. " << fixed << setprecision(2) << value;
    return out.str();
}

void printLine(char symbol, int width) {
    cout << string(width, symbol) << "\n";
}

void printHeader(const string& title) {
    const int width = 44;
    int pad = (width - static_cast<int>(title.size())) / 2;
    printLine('=', width);
    cout << string(pad > 0 ? pad : 0, ' ') << title << "\n";
    printLine('=', width);
}

} // namespace Utils
