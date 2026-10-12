// Studio.h - the SYSTEM class. It owns every collection (dynamic memory) and
// contains all business logic; main.cpp only shows the menu and calls these functions.
#ifndef STUDIO_H
#define STUDIO_H

#include "Booking.h"
#include "Customer.h"
#include "DynArray.h"
#include "FileManager.h"
#include "Package.h"
#include "Photographer.h"
#include <string>
#include <vector>

class Studio {
private:
    std::string name;
    int         openHour;
    int         closeHour;
    int         rooms;                       // sessions that can run at the same time
    std::string dataDir;
    bool        unsaved;

    DynArray<Customer>     customers;        // each wraps `new T[capacity]`
    DynArray<Photographer> photographers;
    DynArray<Package>      packages;
    DynArray<Booking>      bookings;

    // --- helpers ---
    std::string filePath(const std::string& file) const;
    int  chooseEntity(const std::string& action, bool allowBooking) const;
    std::string customerName(int id) const;
    std::string photographerName(int id) const;
    std::string packageName(int id) const;
    void printBookings(const std::vector<int>& indexes) const;
    std::string askFutureDate(const std::string& prompt) const;
    void checkAvailability(const std::string& date, int start, int end, int photographerId, int excludeBookingId) const;

    // --- add ---
    void addCustomer();
    void addPhotographer();
    void addPackage();
    // --- update ---
    void updateCustomer();
    void updatePhotographer();
    void updatePackage();
    void updateBooking();
    // --- main transaction ---
    void bookSession();
    void cancelBooking();
    void completeBooking();
    void viewSchedule();

public:
    Studio();                                                        // default constructor
    Studio(const std::string& name, int openHour, int closeHour, int rooms, const std::string& dataDir);
    ~Studio();

    static std::string findDataDir();
    void setDataDir(const std::string& dir);
    bool hasUnsavedChanges() const;
    const std::string& getName() const;
    int getOpenHour() const;
    int getCloseHour() const;

    // --- menu operations (1-8) ---
    void addRecord();
    void displayRecords() const;
    void searchRecord() const;
    void updateRecord();
    void deleteRecord();
    void mainTransaction();
    void report() const;
    void saveLoadMenu();

    // --- persistence ---
    void load();
    void save();
};

#endif
