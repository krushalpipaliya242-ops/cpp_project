// Booking.h - a reserved photo session linking Customer, Photographer and Package
#ifndef BOOKING_H
#define BOOKING_H

#include "Record.h"
#include <string>

class Booking : public Record {
private:
    int         customerId;
    int         photographerId;
    int         packageId;
    std::string date;                       // YYYY-MM-DD
    int         startHour;                  // 0-23
    int         durationHours;
    double      totalPrice;                 // after discount
    std::string status;                     // Confirmed / Completed / Cancelled

public:
    Booking();
    Booking(int id, int customerId, int photographerId, int packageId, const std::string& date,
            int startHour, int durationHours, double totalPrice, const std::string& status = "Confirmed");
    ~Booking() override;

    int getCustomerId() const;
    int getPhotographerId() const;
    int getPackageId() const;
    const std::string& getDate() const;
    int getStartHour() const;
    int getDurationHours() const;
    int getEndHour() const;
    double getTotalPrice() const;
    const std::string& getStatus() const;

    void setCustomerId(int v);
    void setPhotographerId(int v);
    void setPackageId(int v);
    void setDate(const std::string& d);
    void setStartHour(int h);
    void setDurationHours(int h);
    void setTotalPrice(double p);
    void setStatus(const std::string& s);

    bool isActive() const;                                   // status == Confirmed
    bool overlaps(const std::string& otherDate, int otherStart, int otherEnd) const;

    std::string typeName() const override;
    std::string header() const override;
    void        display() const override;
    std::string toFileLine() const override;
    bool        matches(const std::string& keyword) const override;

    static Booking fromFileLine(const std::string& line);
};

#endif
