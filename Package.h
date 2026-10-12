// Package.h - a photo-shoot package that can be booked (derived from Record)
#ifndef PACKAGE_H
#define PACKAGE_H

#include "Record.h"
#include <string>

class Package : public Record {
private:
    std::string name;
    double      price;
    int         durationHours;
    int         numPhotos;                  // edited photos delivered

public:
    Package();
    Package(int id, const std::string& name, double price, int durationHours, int numPhotos);
    ~Package() override;

    const std::string& getName() const;
    double getPrice() const;
    int    getDurationHours() const;
    int    getNumPhotos() const;
    void setName(const std::string& n);
    void setPrice(double p);
    void setDurationHours(int h);
    void setNumPhotos(int n);

    std::string typeName() const override;
    std::string header() const override;
    void        display() const override;
    std::string toFileLine() const override;
    bool        matches(const std::string& keyword) const override;

    static Package fromFileLine(const std::string& line);
};

#endif
