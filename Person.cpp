#include "Person.h"
#include "Validator.h"

Person::Person() : Record(), name("Unknown"), phone("0000000000"), email("none@none.com") {}

Person::Person(int id, const std::string& n, const std::string& p, const std::string& e)
    : Record(id), name("Unknown"), phone("0000000000"), email("none@none.com") {
    setName(n);
    setPhone(p);
    setEmail(e);
}

Person::~Person() {}

const std::string& Person::getName() const  { return name; }
const std::string& Person::getPhone() const { return phone; }
const std::string& Person::getEmail() const { return email; }

void Person::setName(const std::string& n) {
    std::string t = Validator::trim(n);
    Validator::checkName(t);
    name = t;
}
void Person::setPhone(const std::string& p) {
    std::string t = Validator::trim(p);
    Validator::checkPhone(t);
    phone = t;
}
void Person::setEmail(const std::string& e) {
    std::string t = Validator::trim(e);
    Validator::checkEmail(t);
    email = t;
}

bool Person::matches(const std::string& keyword) const {
    std::string k = Validator::toLower(Validator::trim(keyword));
    if (k.empty()) return false;
    return std::to_string(getId()) == k ||
           Validator::toLower(name).find(k) != std::string::npos ||
           phone.find(k) != std::string::npos ||
           Validator::toLower(email).find(k) != std::string::npos;
}
