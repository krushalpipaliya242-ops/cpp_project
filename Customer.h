// Customer.h - a client who books photo sessions (derived from Person)
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Person.h"

class Customer : public Person {
private:
    std::string membership;                 // Regular / Silver / Gold

public:
    Customer();
    Customer(int id, const std::string& name, const std::string& phone,
             const std::string& email, const std::string& membership = "Regular");
    ~Customer() override;

    const std::string& getMembership() const;
    void   setMembership(const std::string& m);
    double discountRate() const;            // Silver 5 %, Gold 10 %

    std::string roleDescription() const override;
    std::string typeName() const override;
    std::string header() const override;
    void        display() const override;
    std::string toFileLine() const override;

    static Customer fromFileLine(const std::string& line);
};

#endif
