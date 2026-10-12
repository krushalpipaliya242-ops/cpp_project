#include "Validator.h"
#include "Exceptions.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

// ------------------------------------------------------------ string utilities
string Validator::trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string Validator::toLower(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.size(); ++i) r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

string Validator::fit(const string& s, size_t width) {
    if (s.size() + 1 > width) return s.substr(0, width - 3) + ".. ";   // always keeps one space gap
    return s + string(width - s.size(), ' ');
}

vector<string> Validator::split(const string& s, char delim) {
    vector<string> parts;
    string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == delim) { parts.push_back(cur); cur.clear(); }
        else cur += s[i];
    }
    parts.push_back(cur);
    return parts;
}

// ------------------------------------------------------------ field checks
void Validator::checkName(const string& s) {
    if (s.size() < 2 || s.size() > 40)
        throw ValidationException("Name must be 2-40 characters long.");
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = (unsigned char)s[i];
        if (!(isalpha(c) || c == ' ' || c == '.' || c == '\'' || c == '-'))
            throw ValidationException("Name may contain only letters, spaces, . ' and -");
    }
}

void Validator::checkLabel(const string& s) {
    if (s.size() < 2 || s.size() > 40)
        throw ValidationException("Text must be 2-40 characters long.");
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = (unsigned char)s[i];
        if (c == '|' || iscntrl(c)) throw ValidationException("Text must not contain '|' or control characters.");
    }
}

void Validator::checkPhone(const string& s) {
    if (s.size() != 10) throw ValidationException("Phone number must be exactly 10 digits.");
    for (size_t i = 0; i < s.size(); ++i)
        if (!isdigit((unsigned char)s[i])) throw ValidationException("Phone number must contain digits only.");
}

void Validator::checkEmail(const string& s) {
    if (s.empty() || s.size() > 50) throw ValidationException("Email must be 1-50 characters long.");
    if (s.find(' ') != string::npos || s.find('|') != string::npos)
        throw ValidationException("Email must not contain spaces or '|'.");
    size_t at = s.find('@');
    if (at == string::npos || at == 0 || at != s.rfind('@'))
        throw ValidationException("Email must contain exactly one '@' with text before it.");
    size_t dot = s.find('.', at);
    if (dot == string::npos || dot == at + 1 || dot + 1 >= s.size())
        throw ValidationException("Email domain must look like name.com");
}

bool Validator::isLeapYear(int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }

void Validator::checkDate(const string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-')
        throw ValidationException("Date must be in the format YYYY-MM-DD.");
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)s[i])) throw ValidationException("Date must be in the format YYYY-MM-DD.");
    }
    int y = atoi(s.substr(0, 4).c_str());
    int m = atoi(s.substr(5, 2).c_str());
    int d = atoi(s.substr(8, 2).c_str());
    if (y < 2000 || y > 2100) throw ValidationException("Year must be between 2000 and 2100.");
    if (m < 1 || m > 12) throw ValidationException("Month must be 01-12.");
    int dim[] = {31, isLeapYear(y) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (d < 1 || d > dim[m - 1]) throw ValidationException("That day does not exist in the given month.");
}

void Validator::checkMembership(const string& s) {
    if (s != "Regular" && s != "Silver" && s != "Gold")
        throw ValidationException("Membership must be Regular, Silver or Gold.");
}

void Validator::checkStatus(const string& s) {
    if (s != "Confirmed" && s != "Completed" && s != "Cancelled")
        throw ValidationException("Status must be Confirmed, Completed or Cancelled.");
}

// ------------------------------------------------------------ numeric parsing
int Validator::parseInt(const string& raw, int lo, int hi) {
    string s = trim(raw);
    if (s.empty()) throw ValidationException("A number is required.");
    size_t i = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    if (i == s.size()) throw ValidationException("'" + s + "' is not a valid whole number.");
    for (size_t k = i; k < s.size(); ++k)
        if (!isdigit((unsigned char)s[k])) throw ValidationException("'" + s + "' is not a valid whole number.");
    if (s.size() > 9) throw ValidationException("Number is too large.");
    long v = atol(s.c_str());
    if (v < lo || v > hi)
        throw ValidationException("Value must be between " + to_string(lo) + " and " + to_string(hi) + ".");
    return (int)v;
}

double Validator::parseDouble(const string& raw, double lo, double hi) {
    string s = trim(raw);
    if (s.empty()) throw ValidationException("A number is required.");
    char* end = nullptr;
    double v = strtod(s.c_str(), &end);
    if (*end != '\0') throw ValidationException("'" + s + "' is not a valid number.");
    if (v < lo || v > hi)
        throw ValidationException("Value must be between " + to_string((long)lo) + " and " + to_string((long)hi) + ".");
    return v;
}

string Validator::today() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return string(buf);
}

// ------------------------------------------------------------ Input helpers
string Input::line(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) throw InputClosedException();
    return s;
}

string Input::askText(const string& prompt, void (*check)(const string&)) {
    while (true) {
        string s = Validator::trim(line(prompt));
        try { check(s); return s; }
        catch (const ValidationException& e) { cout << "  ! " << e.what() << "\n"; }
    }
}

string Input::askOptional(const string& prompt, void (*check)(const string&)) {
    while (true) {
        string s = Validator::trim(line(prompt));
        if (s.empty()) return "";
        try { check(s); return s; }
        catch (const ValidationException& e) { cout << "  ! " << e.what() << "\n"; }
    }
}

int Input::askInt(const string& prompt, int lo, int hi) {
    while (true) {
        try { return Validator::parseInt(line(prompt), lo, hi); }
        catch (const ValidationException& e) { cout << "  ! " << e.what() << "\n"; }
    }
}

double Input::askDouble(const string& prompt, double lo, double hi) {
    while (true) {
        try { return Validator::parseDouble(line(prompt), lo, hi); }
        catch (const ValidationException& e) { cout << "  ! " << e.what() << "\n"; }
    }
}

bool Input::askIntOptional(const string& prompt, int lo, int hi, int& value) {
    while (true) {
        string s = Validator::trim(line(prompt));
        if (s.empty()) return false;
        try { value = Validator::parseInt(s, lo, hi); return true; }
        catch (const ValidationException& e) { cout << "  ! " << e.what() << "\n"; }
    }
}

bool Input::askDoubleOptional(const string& prompt, double lo, double hi, double& value) {
    while (true) {
        string s = Validator::trim(line(prompt));
        if (s.empty()) return false;
        try { value = Validator::parseDouble(s, lo, hi); return true; }
        catch (const ValidationException& e) { cout << "  ! " << e.what() << "\n"; }
    }
}

bool Input::askYesNo(const string& prompt) {
    while (true) {
        string s = Validator::toLower(Validator::trim(line(prompt)));
        if (s == "y" || s == "yes") return true;
        if (s == "n" || s == "no") return false;
        cout << "  ! Please answer y or n.\n";
    }
}
