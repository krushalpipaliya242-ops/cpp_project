#include "Customer.h"
#include "Validator.h"
#include "Exceptions.h"
#include <iostream>

Customer::Customer() : Person(), membership("Regular") {}

Customer::Customer(int id, const std::string& name, const std::string& phone,
                   const std::string& email, const std::string& m)
    : Person(id, name, phone, email), membership("Regular") {
    setMembership(m);
}

Customer::~Customer() {}

const std::string& Customer::getMembership() const { return membership; }

void Customer::setMembership(const std::string& m) {
    std::string t = Validator::trim(m);
    Validator::checkMembership(t);
    membership = t;
}

double Customer::discountRate() const {
    if (membership == "Gold") return 0.10;
    if (membership == "Silver") return 0.05;
    return 0.0;
}

std::string Customer::roleDescription() const { return "Client (" + membership + " member)"; }
std::string Customer::typeName() const { return "Customer"; }

std::string Customer::header() const {
    return Validator::fit("ID", 6) + Validator::fit("Name", 24) + Validator::fit("Phone", 13) +
           Validator::fit("Email", 30) + "Membership";
}

void Customer::display() const {
    std::cout << Validator::fit(std::to_string(getId()), 6) << Validator::fit(getName(), 24)
              << Validator::fit(getPhone(), 13) << Validator::fit(getEmail(), 30) << membership << "\n";
}

std::string Customer::toFileLine() const {
    return std::to_string(getId()) + "|" + getName() + "|" + getPhone() + "|" + getEmail() + "|" + membership;
}

Customer Customer::fromFileLine(const std::string& line) {
    std::vector<std::string> f = Validator::split(line, '|');
    if (f.size() != 5) throw ValidationException("Customer record needs 5 fields, found " + std::to_string(f.size()));
    return Customer(Validator::parseInt(f[0], 1, 999999999), f[1], f[2], f[3], f[4]);
}
