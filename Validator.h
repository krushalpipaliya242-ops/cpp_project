// Validator.h - validation rules (Validator) and safe console input helpers (Input)
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include <vector>

class Validator {
public:
    // ---- string utilities ----
    static std::string trim(const std::string& s);
    static std::string toLower(const std::string& s);
    static std::string fit(const std::string& s, size_t width);   // pad / truncate for tables
    static std::vector<std::string> split(const std::string& s, char delim);

    // ---- field checks: each throws ValidationException when the value is invalid ----
    static void checkName(const std::string& s);        // letters, spaces . ' -  (2..40)
    static void checkLabel(const std::string& s);       // generic text (2..40, no '|')
    static void checkPhone(const std::string& s);       // exactly 10 digits
    static void checkEmail(const std::string& s);
    static void checkDate(const std::string& s);        // YYYY-MM-DD, real calendar date
    static void checkMembership(const std::string& s);  // Regular / Silver / Gold
    static void checkStatus(const std::string& s);      // Confirmed / Completed / Cancelled

    // ---- numeric parsing (throws ValidationException) ----
    static int    parseInt(const std::string& s, int lo, int hi);
    static double parseDouble(const std::string& s, double lo, double hi);

    // ---- dates ----
    static bool isLeapYear(int y);
    static std::string today();                          // current date as YYYY-MM-DD
};

// Console input that re-prompts until the value is valid.
class Input {
public:
    static std::string line(const std::string& prompt);  // throws InputClosedException on EOF
    static std::string askText(const std::string& prompt, void (*check)(const std::string&));
    static std::string askOptional(const std::string& prompt, void (*check)(const std::string&));
    static int    askInt(const std::string& prompt, int lo, int hi);
    static double askDouble(const std::string& prompt, double lo, double hi);
    static bool   askIntOptional(const std::string& prompt, int lo, int hi, int& value);       // true if changed
    static bool   askDoubleOptional(const std::string& prompt, double lo, double hi, double& value);
    static bool   askYesNo(const std::string& prompt);
};

#endif
