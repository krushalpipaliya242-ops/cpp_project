#include "Photographer.h"
#include "Validator.h"
#include "Exceptions.h"
#include <iostream>

Photographer::Photographer() : Person(), specialty("General"), experienceYears(0) {}

Photographer::Photographer(int id, const std::string& name, const std::string& phone,
                           const std::string& email, const std::string& s, int years)
    : Person(id, name, phone, email), specialty("General"), experienceYears(0) {
    setSpecialty(s);
    setExperienceYears(years);
}

Photographer::~Photographer() {}

const std::string& Photographer::getSpecialty() const { return specialty; }
int Photographer::getExperienceYears() const { return experienceYears; }

void Photographer::setSpecialty(const std::string& s) {
    std::string t = Validator::trim(s);
    Validator::checkLabel(t);
    specialty = t;
}

void Photographer::setExperienceYears(int y) {
    if (y < 0 || y > 60) throw ValidationException("Experience must be between 0 and 60 years.");
    experienceYears = y;
}

std::string Photographer::roleDescription() const { return specialty + " photographer"; }
std::string Photographer::typeName() const { return "Photographer"; }

std::string Photographer::header() const {
    return Validator::fit("ID", 6) + Validator::fit("Name", 22) + Validator::fit("Phone", 13) +
           Validator::fit("Email", 32) + Validator::fit("Specialty", 16) + "Yrs";
}

void Photographer::display() const {
    std::cout << Validator::fit(std::to_string(getId()), 6) << Validator::fit(getName(), 22)
              << Validator::fit(getPhone(), 13) << Validator::fit(getEmail(), 32)
              << Validator::fit(specialty, 16) << experienceYears << "\n";
}

std::string Photographer::toFileLine() const {
    return std::to_string(getId()) + "|" + getName() + "|" + getPhone() + "|" + getEmail() + "|" +
           specialty + "|" + std::to_string(experienceYears);
}

bool Photographer::matches(const std::string& keyword) const {
    if (Person::matches(keyword)) return true;
    std::string k = Validator::toLower(Validator::trim(keyword));
    return !k.empty() && Validator::toLower(specialty).find(k) != std::string::npos;
}

Photographer Photographer::fromFileLine(const std::string& line) {
    std::vector<std::string> f = Validator::split(line, '|');
    if (f.size() != 6) throw ValidationException("Photographer record needs 6 fields, found " + std::to_string(f.size()));
    return Photographer(Validator::parseInt(f[0], 1, 999999999), f[1], f[2], f[3], f[4],
                        Validator::parseInt(f[5], 0, 60));
}
