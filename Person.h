// Person.h - abstract parent of Customer and Photographer (second inheritance level).
#ifndef PERSON_H
#define PERSON_H

#include "Record.h"
#include <string>

class Person : public Record {
private:
    std::string name;
    std::string phone;
    std::string email;

public:
    Person();                                                                   // default constructor
    Person(int id, const std::string& name, const std::string& phone, const std::string& email);
    ~Person() override;

    const std::string& getName() const;
    const std::string& getPhone() const;
    const std::string& getEmail() const;
    void setName(const std::string& n);
    void setPhone(const std::string& p);
    void setEmail(const std::string& e);

    virtual std::string roleDescription() const = 0;
    bool matches(const std::string& keyword) const override;
};

#endif
