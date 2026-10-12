#include "Booking.h"
#include "Validator.h"
#include "Exceptions.h"
#include <iomanip>
#include <iostream>
#include <sstream>

Booking::Booking()
    : Record(), customerId(0), photographerId(0), packageId(0), date("2000-01-01"),
      startHour(9), durationHours(1), totalPrice(0.0), status("Confirmed") {}

Booking::Booking(int id, int cid, int pid, int pkgId, const std::string& d, int start,
                 int dur, double total, const std::string& st)
    : Record(id), customerId(0), photographerId(0), packageId(0), date("2000-01-01"),
      startHour(9), durationHours(1), totalPrice(0.0), status("Confirmed") {
    setCustomerId(cid);
    setPhotographerId(pid);
    setPackageId(pkgId);
    setDate(d);
    setStartHour(start);
    setDurationHours(dur);
    setTotalPrice(total);
    setStatus(st);
}

Booking::~Booking() {}

int Booking::getCustomerId() const { return customerId; }
int Booking::getPhotographerId() const { return photographerId; }
int Booking::getPackageId() const { return packageId; }
const std::string& Booking::getDate() const { return date; }
int Booking::getStartHour() const { return startHour; }
int Booking::getDurationHours() const { return durationHours; }
int Booking::getEndHour() const { return startHour + durationHours; }
double Booking::getTotalPrice() const { return totalPrice; }
const std::string& Booking::getStatus() const { return status; }

static void requirePositive(int v, const char* what) {
    if (v <= 0) throw ValidationException(std::string(what) + " ID must be positive.");
}

void Booking::setCustomerId(int v) { requirePositive(v, "Customer"); customerId = v; }
void Booking::setPhotographerId(int v) { requirePositive(v, "Photographer"); photographerId = v; }
void Booking::setPackageId(int v) { requirePositive(v, "Package"); packageId = v; }

void Booking::setDate(const std::string& d) {
    std::string t = Validator::trim(d);
    Validator::checkDate(t);
    date = t;
}
void Booking::setStartHour(int h) {
    if (h < 0 || h > 23) throw ValidationException("Start hour must be 0-23.");
    startHour = h;
}
void Booking::setDurationHours(int h) {
    if (h < 1 || h > 12) throw ValidationException("Duration must be 1-12 hours.");
    durationHours = h;
}
void Booking::setTotalPrice(double p) {
    if (p < 0) throw ValidationException("Total price cannot be negative.");
    totalPrice = p;
}
void Booking::setStatus(const std::string& s) {
    std::string t = Validator::trim(s);
    Validator::checkStatus(t);
    status = t;
}

bool Booking::isActive() const { return status == "Confirmed"; }

bool Booking::overlaps(const std::string& otherDate, int otherStart, int otherEnd) const {
    return date == otherDate && startHour < otherEnd && otherStart < getEndHour();
}

std::string Booking::typeName() const { return "Booking"; }

static std::string timeRange(int s, int e) {
    std::ostringstream o;
    o << std::setfill('0') << std::setw(2) << s << ":00-" << std::setw(2) << e << ":00";
    return o.str();
}

std::string Booking::header() const {
    return Validator::fit("ID", 6) + Validator::fit("Date", 12) + Validator::fit("Time", 13) +
           Validator::fit("Cust", 6) + Validator::fit("Phot", 6) + Validator::fit("Pkg", 6) +
           Validator::fit("Total (Rs.)", 13) + "Status";
}

void Booking::display() const {
    std::ostringstream t;
    t << std::fixed << std::setprecision(2) << totalPrice;
    std::cout << Validator::fit(std::to_string(getId()), 6) << Validator::fit(date, 12)
              << Validator::fit(timeRange(startHour, getEndHour()), 13)
              << Validator::fit(std::to_string(customerId), 6) << Validator::fit(std::to_string(photographerId), 6)
              << Validator::fit(std::to_string(packageId), 6) << Validator::fit(t.str(), 13) << status << "\n";
}

std::string Booking::toFileLine() const {
    std::ostringstream o;
    o << getId() << "|" << customerId << "|" << photographerId << "|" << packageId << "|" << date << "|"
      << startHour << "|" << durationHours << "|" << std::fixed << std::setprecision(2) << totalPrice
      << "|" << status;
    return o.str();
}

bool Booking::matches(const std::string& keyword) const {
    std::string k = Validator::toLower(Validator::trim(keyword));
    if (k.empty()) return false;
    return std::to_string(getId()) == k || date.find(k) != std::string::npos ||
           Validator::toLower(status) == k;
}

Booking Booking::fromFileLine(const std::string& line) {
    std::vector<std::string> f = Validator::split(line, '|');
    if (f.size() != 9) throw ValidationException("Booking record needs 9 fields, found " + std::to_string(f.size()));
    return Booking(Validator::parseInt(f[0], 1, 999999999), Validator::parseInt(f[1], 1, 999999999),
                   Validator::parseInt(f[2], 1, 999999999), Validator::parseInt(f[3], 1, 999999999), f[4],
                   Validator::parseInt(f[5], 0, 23), Validator::parseInt(f[6], 1, 12),
                   Validator::parseDouble(f[7], 0, 100000000), f[8]);
}
