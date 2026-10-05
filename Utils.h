#ifndef UTILS_H
#define UTILS_H

#include <string>

// Reusable console helpers (input validation + formatting).
// Kept in a namespace because these functions need no object state.
namespace Utils {
    std::string readLine(const std::string& prompt);            // reads one full line
    int         readInt(const std::string& prompt);             // loops until a valid whole number
    double      readDouble(const std::string& prompt);          // loops until a valid number
    int         readMenuChoice(const std::string& prompt, int minChoice, int maxChoice);
    std::string formatMoney(double value);                      // e.g. "Rs. 25000.00"
    void        printHeader(const std::string& title);          // boxed heading
    void        printLine(char symbol = '-', int width = 44);
}

#endif
