// Photographer.h - a staff photographer (derived from Person)
#ifndef PHOTOGRAPHER_H
#define PHOTOGRAPHER_H

#include "Person.h"

class Photographer : public Person {
private:
    std::string specialty;                  // Wedding, Portrait, Product ...
    int         experienceYears;

public:
    Photographer();
    Photographer(int id, const std::string& name, const std::string& phone,
                 const std::string& email, const std::string& specialty, int experienceYears);
    ~Photographer() override;

    const std::string& getSpecialty() const;
    int  getExperienceYears() const;
    void setSpecialty(const std::string& s);
    void setExperienceYears(int y);

    std::string roleDescription() const override;
    std::string typeName() const override;
    std::string header() const override;
    void        display() const override;
    std::string toFileLine() const override;
    bool        matches(const std::string& keyword) const override;   // also searches specialty

    static Photographer fromFileLine(const std::string& line);
};

#endif
