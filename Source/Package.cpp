#include "Package.h"
#include "Validator.h"
#include "Exceptions.h"
#include <iomanip>
#include <iostream>
#include <sstream>

Package::Package() : Record(), name("Basic"), price(1.0), durationHours(1), numPhotos(1) {}

Package::Package(int id, const std::string& n, double p, int h, int photos)
    : Record(id), name("Basic"), price(1.0), durationHours(1), numPhotos(1) {
    setName(n);
    setPrice(p);
    setDurationHours(h);
    setNumPhotos(photos);
}

Package::~Package() {}

const std::string& Package::getName() const { return name; }
double Package::getPrice() const { return price; }
int Package::getDurationHours() const { return durationHours; }
int Package::getNumPhotos() const { return numPhotos; }

void Package::setName(const std::string& n) {
    std::string t = Validator::trim(n);
    Validator::checkLabel(t);
    name = t;
}
void Package::setPrice(double p) {
    if (p <= 0 || p > 1000000) throw ValidationException("Price must be greater than 0 and at most 1,000,000.");
    price = p;
}
void Package::setDurationHours(int h) {
    if (h < 1 || h > 8) throw ValidationException("Duration must be 1-8 hours.");
    durationHours = h;
}
void Package::setNumPhotos(int n) {
    if (n < 1 || n > 1000) throw ValidationException("Number of photos must be 1-1000.");
    numPhotos = n;
}

std::string Package::typeName() const { return "Package"; }

std::string Package::header() const {
    return Validator::fit("ID", 6) + Validator::fit("Package", 26) + Validator::fit("Price (Rs.)", 14) +
           Validator::fit("Hours", 7) + "Photos";
}

void Package::display() const {
    std::ostringstream p;
    p << std::fixed << std::setprecision(2) << price;
    std::cout << Validator::fit(std::to_string(getId()), 6) << Validator::fit(name, 26)
              << Validator::fit(p.str(), 14) << Validator::fit(std::to_string(durationHours), 7)
              << numPhotos << "\n";
}

std::string Package::toFileLine() const {
    std::ostringstream o;
    o << getId() << "|" << name << "|" << std::fixed << std::setprecision(2) << price << "|"
      << durationHours << "|" << numPhotos;
    return o.str();
}

bool Package::matches(const std::string& keyword) const {
    std::string k = Validator::toLower(Validator::trim(keyword));
    if (k.empty()) return false;
    return std::to_string(getId()) == k || Validator::toLower(name).find(k) != std::string::npos;
}

Package Package::fromFileLine(const std::string& line) {
    std::vector<std::string> f = Validator::split(line, '|');
    if (f.size() != 5) throw ValidationException("Package record needs 5 fields, found " + std::to_string(f.size()));
    return Package(Validator::parseInt(f[0], 1, 999999999), f[1], Validator::parseDouble(f[2], 0.01, 1000000),
                   Validator::parseInt(f[3], 1, 8), Validator::parseInt(f[4], 1, 1000));
}
