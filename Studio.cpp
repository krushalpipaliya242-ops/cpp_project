#include "Studio.h"
#include "Exceptions.h"
#include "Validator.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

// ============================================================ file-local helpers
namespace {

template <class T>
int nextId(const DynArray<T>& a) {
    int m = 0;
    for (int i = 0; i < a.size(); ++i) m = max(m, a.at(i).getId());
    return m + 1;
}

template <class T>
int indexOrThrow(const DynArray<T>& a, int id, const string& what) {
    int idx = a.indexOfId(id);
    if (idx < 0) throw RecordNotFoundException(what + " with ID " + to_string(id) + " was not found.");
    return idx;
}

// Polymorphism: works for Customer, Photographer, Package ... through the Record base class.
template <class T>
void printAll(const DynArray<T>& a) {
    if (a.size() == 0) { cout << "  (no records)\n"; return; }
    string h = a.at(0).header();
    cout << h << "\n" << string(h.size(), '-') << "\n";
    for (int i = 0; i < a.size(); ++i) {
        const Record& r = a.at(i);
        r.display();
    }
}

template <class T>
vector<int> findMatches(const DynArray<T>& a, const string& kw) {
    vector<int> hits;
    for (int i = 0; i < a.size(); ++i) {
        const Record& r = a.at(i);
        if (r.matches(kw)) hits.push_back(i);
    }
    return hits;
}

template <class T>
void printSelected(const DynArray<T>& a, const vector<int>& idx) {
    string h = a.at(idx[0]).header();
    cout << h << "\n" << string(h.size(), '-') << "\n";
    for (size_t i = 0; i < idx.size(); ++i) a.at(idx[i]).display();
}

string money(double v) {
    ostringstream o;
    o << fixed << setprecision(2) << v;
    return o.str();
}

string hh(int h) {
    ostringstream o;
    o << setfill('0') << setw(2) << h << ":00";
    return o.str();
}

double round2(double v) { return floor(v * 100.0 + 0.5) / 100.0; }

void line(char c = '=', int n = 70) { cout << string(n, c) << "\n"; }

int pickMembership() {
    cout << "  Membership: 1) Regular  2) Silver (5% off)  3) Gold (10% off)\n";
    return Input::askInt("  Choose 1-3: ", 1, 3);
}

string membershipName(int choice) { return choice == 3 ? "Gold" : (choice == 2 ? "Silver" : "Regular"); }

}  // namespace

// ============================================================ construction / destruction
Studio::Studio()
    : name("Lens & Light Photography Studio"), openHour(9), closeHour(20), rooms(2),
      dataDir(findDataDir()), unsaved(false), customers(8), photographers(4), packages(4), bookings(16) {}

Studio::Studio(const string& n, int open, int close, int r, const string& dir)
    : name(n), openHour(open), closeHour(close), rooms(r), dataDir(dir), unsaved(false),
      customers(8), photographers(4), packages(4), bookings(16) {
    if (open < 0 || close > 24 || open >= close) throw ValidationException("Invalid studio opening hours.");
    if (r < 1 || r > 20) throw ValidationException("Studio rooms must be 1-20.");
    if (dataDir.empty()) dataDir = findDataDir();
}

Studio::~Studio() {
    // The four DynArray members release their heap blocks with delete[] automatically.
    cout << "[Studio '" << name << "' closed - all dynamic memory released]\n";
}

string Studio::findDataDir() {
    if (filesystem::exists("../Data")) return "../Data/";
    return "Data/";
}

void Studio::setDataDir(const string& dir) {
    dataDir = dir;
    if (!dataDir.empty() && dataDir.back() != '/' && dataDir.back() != '\\') dataDir += "/";
}

bool Studio::hasUnsavedChanges() const { return unsaved; }
const string& Studio::getName() const { return name; }
int Studio::getOpenHour() const { return openHour; }
int Studio::getCloseHour() const { return closeHour; }

string Studio::filePath(const string& file) const { return dataDir + file; }

// ============================================================ persistence
void Studio::load() {
    int skipped = 0;
    skipped += FileManager::load(filePath("customers.txt"), customers);
    skipped += FileManager::load(filePath("photographers.txt"), photographers);
    skipped += FileManager::load(filePath("packages.txt"), packages);
    skipped += FileManager::load(filePath("bookings.txt"), bookings);
    unsaved = false;
    cout << "Loaded " << customers.size() << " customers, " << photographers.size() << " photographers, "
         << packages.size() << " packages, " << bookings.size() << " bookings from '" << dataDir << "'.\n";
    if (skipped > 0) cout << "  (" << skipped << " corrupt line(s) were skipped - see warnings above)\n";
}

void Studio::save() {
    filesystem::create_directories(dataDir);
    FileManager::save(filePath("customers.txt"), "id|name|phone|email|membership", customers);
    FileManager::save(filePath("photographers.txt"), "id|name|phone|email|specialty|experienceYears", photographers);
    FileManager::save(filePath("packages.txt"), "id|name|price|durationHours|numPhotos", packages);
    FileManager::save(filePath("bookings.txt"),
                      "id|customerId|photographerId|packageId|date|startHour|durationHours|totalPrice|status", bookings);
    unsaved = false;
    cout << "Saved " << customers.size() << " customers, " << photographers.size() << " photographers, "
         << packages.size() << " packages, " << bookings.size() << " bookings to '" << dataDir << "'.\n";
}

void Studio::saveLoadMenu() {
    cout << "\n--- Save / Load ---\n  1) Save all records to files\n  2) Reload records from files (discards unsaved changes)\n  0) Back\n";
    int c = Input::askInt("Choice: ", 0, 2);
    if (c == 1) save();
    else if (c == 2) {
        if (unsaved && !Input::askYesNo("You have unsaved changes that will be lost. Continue? (y/n): ")) return;
        load();
    }
}

// ============================================================ lookup helpers
string Studio::customerName(int id) const {
    int i = customers.indexOfId(id);
    return i < 0 ? "(deleted)" : customers.at(i).getName();
}
string Studio::photographerName(int id) const {
    int i = photographers.indexOfId(id);
    return i < 0 ? "(deleted)" : photographers.at(i).getName();
}
string Studio::packageName(int id) const {
    int i = packages.indexOfId(id);
    return i < 0 ? "(deleted)" : packages.at(i).getName();
}

int Studio::chooseEntity(const string& action, bool allowBooking) const {
    cout << "\n" << action << " which record type?\n  1) Customer\n  2) Photographer\n  3) Package\n";
    if (allowBooking) cout << "  4) Booking\n";
    cout << "  0) Back\n";
    return Input::askInt("Choice: ", 0, allowBooking ? 4 : 3);
}

void Studio::printBookings(const vector<int>& idx) const {
    if (idx.empty()) { cout << "  (no records)\n"; return; }
    string h = Validator::fit("ID", 6) + Validator::fit("Date", 12) + Validator::fit("Time", 13) +
               Validator::fit("Customer", 18) + Validator::fit("Photographer", 18) +
               Validator::fit("Package", 20) + Validator::fit("Total (Rs.)", 13) + "Status";
    cout << h << "\n" << string(h.size(), '-') << "\n";
    for (size_t i = 0; i < idx.size(); ++i) {
        const Booking& b = bookings.at(idx[i]);
        cout << Validator::fit(to_string(b.getId()), 6) << Validator::fit(b.getDate(), 12)
             << Validator::fit(hh(b.getStartHour()) + "-" + hh(b.getEndHour()), 13)
             << Validator::fit(customerName(b.getCustomerId()), 18)
             << Validator::fit(photographerName(b.getPhotographerId()), 18)
             << Validator::fit(packageName(b.getPackageId()), 20)
             << Validator::fit(money(b.getTotalPrice()), 13) << b.getStatus() << "\n";
    }
}

string Studio::askFutureDate(const string& prompt) const {
    while (true) {
        string d = Input::askText(prompt, Validator::checkDate);
        if (d < Validator::today()) { cout << "  ! Date cannot be in the past (today is " << Validator::today() << ").\n"; continue; }
        return d;
    }
}

// ============================================================ 1. ADD
void Studio::addRecord() {
    int c = chooseEntity("Add", false);
    if (c == 1) addCustomer();
    else if (c == 2) addPhotographer();
    else if (c == 3) addPackage();
    // bookings are created through menu 6 (main transaction)
}

void Studio::addCustomer() {
    cout << "\n-- New customer --\n";
    string n = Input::askText("  Name: ", Validator::checkName);
    string p = Input::askText("  Phone (10 digits): ", Validator::checkPhone);
    string e = Input::askText("  Email: ", Validator::checkEmail);
    string m = membershipName(pickMembership());
    Customer c(nextId(customers), n, p, e, m);
    customers.add(c);
    unsaved = true;
    cout << "Customer added with ID " << c.getId() << ".\n";
}

void Studio::addPhotographer() {
    cout << "\n-- New photographer --\n";
    string n = Input::askText("  Name: ", Validator::checkName);
    string p = Input::askText("  Phone (10 digits): ", Validator::checkPhone);
    string e = Input::askText("  Email: ", Validator::checkEmail);
    string s = Input::askText("  Specialty (e.g. Wedding, Portrait): ", Validator::checkLabel);
    int y = Input::askInt("  Years of experience (0-60): ", 0, 60);
    Photographer ph(nextId(photographers), n, p, e, s, y);
    photographers.add(ph);
    unsaved = true;
    cout << "Photographer added with ID " << ph.getId() << ".\n";
}

void Studio::addPackage() {
    cout << "\n-- New package --\n";
    string n = Input::askText("  Package name: ", Validator::checkLabel);
    double price = Input::askDouble("  Price in Rs.: ", 0.01, 1000000);
    int hours = Input::askInt("  Duration in hours (1-8): ", 1, 8);
    int photos = Input::askInt("  Edited photos delivered (1-1000): ", 1, 1000);
    Package pk(nextId(packages), n, price, hours, photos);
    packages.add(pk);
    unsaved = true;
    cout << "Package added with ID " << pk.getId() << ".\n";
}

// ============================================================ 2. DISPLAY
void Studio::displayRecords() const {
    int c = chooseEntity("Display", true);
    cout << "\n";
    if (c == 1) printAll(customers);
    else if (c == 2) printAll(photographers);
    else if (c == 3) printAll(packages);
    else if (c == 4) {
        vector<int> all;
        for (int i = 0; i < bookings.size(); ++i) all.push_back(i);
        printBookings(all);
    }
}

// ============================================================ 3. SEARCH
void Studio::searchRecord() const {
    int c = chooseEntity("Search", true);
    if (c == 0) return;
    string kw = Input::line("Keyword (ID, name, phone, email, date, status ...): ");
    kw = Validator::trim(kw);
    if (kw.empty()) throw ValidationException("Please type a keyword to search for.");
    cout << "\n";
    if (c == 1) {
        vector<int> h = findMatches(customers, kw);
        if (h.empty()) cout << "  No matching customers.\n"; else printSelected(customers, h);
    } else if (c == 2) {
        vector<int> h = findMatches(photographers, kw);
        if (h.empty()) cout << "  No matching photographers.\n"; else printSelected(photographers, h);
    } else if (c == 3) {
        vector<int> h = findMatches(packages, kw);
        if (h.empty()) cout << "  No matching packages.\n"; else printSelected(packages, h);
    } else {
        string k = Validator::toLower(kw);
        vector<int> h;
        for (int i = 0; i < bookings.size(); ++i) {
            const Booking& b = bookings.at(i);
            const Record& r = b;
            bool hit = r.matches(kw) ||
                       Validator::toLower(customerName(b.getCustomerId())).find(k) != string::npos ||
                       Validator::toLower(photographerName(b.getPhotographerId())).find(k) != string::npos ||
                       Validator::toLower(packageName(b.getPackageId())).find(k) != string::npos;
            if (hit) h.push_back(i);
        }
        if (h.empty()) cout << "  No matching bookings.\n"; else printBookings(h);
    }
}

// ============================================================ 4. UPDATE
void Studio::updateRecord() {
    int c = chooseEntity("Update", true);
    if (c == 1) updateCustomer();
    else if (c == 2) updatePhotographer();
    else if (c == 3) updatePackage();
    else if (c == 4) updateBooking();
}

void Studio::updateCustomer() {
    int id = Input::askInt("Customer ID to update: ", 1, 999999999);
    Customer& c = customers.at(indexOrThrow(customers, id, "Customer"));
    cout << "Current record:\n" << c.header() << "\n"; c.display();
    cout << "Press Enter to keep a value.\n";
    string v;
    v = Input::askOptional("  New name: ", Validator::checkName);  if (!v.empty()) c.setName(v);
    v = Input::askOptional("  New phone: ", Validator::checkPhone); if (!v.empty()) c.setPhone(v);
    v = Input::askOptional("  New email: ", Validator::checkEmail); if (!v.empty()) c.setEmail(v);
    cout << "  Change membership? (currently " << c.getMembership() << ")\n";
    if (Input::askYesNo("  Change it? (y/n): ")) c.setMembership(membershipName(pickMembership()));
    unsaved = true;
    cout << "Customer updated.\n";
}

void Studio::updatePhotographer() {
    int id = Input::askInt("Photographer ID to update: ", 1, 999999999);
    Photographer& p = photographers.at(indexOrThrow(photographers, id, "Photographer"));
    cout << "Current record:\n" << p.header() << "\n"; p.display();
    cout << "Press Enter to keep a value.\n";
    string v;
    v = Input::askOptional("  New name: ", Validator::checkName);  if (!v.empty()) p.setName(v);
    v = Input::askOptional("  New phone: ", Validator::checkPhone); if (!v.empty()) p.setPhone(v);
    v = Input::askOptional("  New email: ", Validator::checkEmail); if (!v.empty()) p.setEmail(v);
    v = Input::askOptional("  New specialty: ", Validator::checkLabel); if (!v.empty()) p.setSpecialty(v);
    int y = p.getExperienceYears();
    if (Input::askIntOptional("  New years of experience: ", 0, 60, y)) p.setExperienceYears(y);
    unsaved = true;
    cout << "Photographer updated.\n";
}

void Studio::updatePackage() {
    int id = Input::askInt("Package ID to update: ", 1, 999999999);
    Package& p = packages.at(indexOrThrow(packages, id, "Package"));
    cout << "Current record:\n" << p.header() << "\n"; p.display();
    cout << "Press Enter to keep a value. (Existing bookings keep the price they were made at.)\n";
    string v = Input::askOptional("  New name: ", Validator::checkLabel);
    if (!v.empty()) p.setName(v);
    double price = p.getPrice();
    if (Input::askDoubleOptional("  New price: ", 0.01, 1000000, price)) p.setPrice(price);
    int h = p.getDurationHours();
    if (Input::askIntOptional("  New duration (hours): ", 1, 8, h)) p.setDurationHours(h);
    int n = p.getNumPhotos();
    if (Input::askIntOptional("  New photo count: ", 1, 1000, n)) p.setNumPhotos(n);
    unsaved = true;
    cout << "Package updated.\n";
}

void Studio::updateBooking() {
    int id = Input::askInt("Booking ID to reschedule: ", 1, 999999999);
    Booking& b = bookings.at(indexOrThrow(bookings, id, "Booking"));
    if (!b.isActive()) throw BookingException("Only Confirmed bookings can be rescheduled (this one is " + b.getStatus() + ").");
    printBookings(vector<int>(1, bookings.indexOfId(id)));
    cout << "Press Enter to keep a value.\n";
    string d = b.getDate();
    string nd;
    while (true) {
        nd = Input::askOptional("  New date (YYYY-MM-DD): ", Validator::checkDate);
        if (nd.empty() || nd >= Validator::today()) break;
        cout << "  ! Date cannot be in the past.\n";
    }
    if (!nd.empty()) d = nd;
    int h = b.getStartHour();
    Input::askIntOptional("  New start hour (" + to_string(openHour) + "-" + to_string(closeHour - 1) + "): ", openHour, closeHour - 1, h);
    // validate BEFORE changing anything so a failed reschedule leaves the booking untouched
    checkAvailability(d, h, h + b.getDurationHours(), b.getPhotographerId(), b.getId());
    b.setDate(d);
    b.setStartHour(h);
    unsaved = true;
    cout << "Booking " << id << " rescheduled to " << d << " at " << hh(h) << ".\n";
}

// ============================================================ 5. DELETE
void Studio::deleteRecord() {
    int c = chooseEntity("Delete", true);
    if (c == 0) return;
    int id = Input::askInt("ID to delete: ", 1, 999999999);

    if (c == 4) {
        int idx = indexOrThrow(bookings, id, "Booking");
        printBookings(vector<int>(1, idx));
        if (!Input::askYesNo("Permanently delete this booking? (y/n): ")) { cout << "Cancelled.\n"; return; }
        bookings.removeAt(idx);
        unsaved = true;
        cout << "Booking deleted.\n";
        return;
    }

    // Referential integrity: a record that is used by a booking cannot be removed.
    int refs = 0;
    for (int i = 0; i < bookings.size(); ++i) {
        const Booking& b = bookings.at(i);
        if ((c == 1 && b.getCustomerId() == id) || (c == 2 && b.getPhotographerId() == id) ||
            (c == 3 && b.getPackageId() == id)) ++refs;
    }

    int idx;
    if (c == 1) idx = indexOrThrow(customers, id, "Customer");
    else if (c == 2) idx = indexOrThrow(photographers, id, "Photographer");
    else idx = indexOrThrow(packages, id, "Package");
    if (refs > 0)
        throw BookingException("Cannot delete: " + to_string(refs) + " booking(s) still refer to this record. Delete those bookings first.");

    if (!Input::askYesNo("Permanently delete this record? (y/n): ")) { cout << "Cancelled.\n"; return; }
    if (c == 1) customers.removeAt(idx);
    else if (c == 2) photographers.removeAt(idx);
    else packages.removeAt(idx);
    unsaved = true;
    cout << "Record deleted.\n";
}

// ============================================================ 6. MAIN TRANSACTION
void Studio::mainTransaction() {
    cout << "\n--- Booking Transaction ---\n"
         << "  1) Book a photo session\n  2) Cancel a booking\n  3) Mark a booking as completed\n"
         << "  4) View schedule for a date\n  0) Back\n";
    int c = Input::askInt("Choice: ", 0, 4);
    if (c == 1) bookSession();
    else if (c == 2) cancelBooking();
    else if (c == 3) completeBooking();
    else if (c == 4) viewSchedule();
}

void Studio::checkAvailability(const string& date, int start, int end, int photographerId, int excludeBookingId) const {
    if (start < openHour || end > closeHour)
        throw BookingException("Studio hours are " + hh(openHour) + " to " + hh(closeHour) +
                               "; this session would run " + hh(start) + " to " + hh(end) + ".");
    int roomsUsed = 0;
    for (int i = 0; i < bookings.size(); ++i) {
        const Booking& b = bookings.at(i);
        if (!b.isActive() || b.getId() == excludeBookingId || !b.overlaps(date, start, end)) continue;
        if (b.getPhotographerId() == photographerId)
            throw BookingException(photographerName(photographerId) + " is already booked on " + date + " from " +
                                   hh(b.getStartHour()) + " to " + hh(b.getEndHour()) + " (booking " + to_string(b.getId()) + ").");
        ++roomsUsed;
    }
    if (roomsUsed >= rooms)
        throw BookingException("All " + to_string(rooms) + " studio rooms are occupied in that time slot.");
}

void Studio::bookSession() {
    if (customers.size() == 0 || photographers.size() == 0 || packages.size() == 0)
        throw BookingException("Add at least one customer, one photographer and one package before booking.");

    cout << "\n-- Customers --\n";     printAll(customers);
    int cIdx = indexOrThrow(customers, Input::askInt("Customer ID: ", 1, 999999999), "Customer");
    cout << "\n-- Photographers --\n"; printAll(photographers);
    int phIdx = indexOrThrow(photographers, Input::askInt("Photographer ID: ", 1, 999999999), "Photographer");
    cout << "\n-- Packages --\n";      printAll(packages);
    int pkIdx = indexOrThrow(packages, Input::askInt("Package ID: ", 1, 999999999), "Package");

    const Customer& cust = customers.at(cIdx);
    const Photographer& phot = photographers.at(phIdx);
    const Package& pkg = packages.at(pkIdx);

    string date = askFutureDate("Session date (YYYY-MM-DD): ");
    int start = Input::askInt("Start hour, 24h (" + to_string(openHour) + "-" + to_string(closeHour - 1) + "): ", 0, 23);
    int end = start + pkg.getDurationHours();

    checkAvailability(date, start, end, phot.getId(), 0);       // throws BookingException on conflict

    double discount = cust.discountRate();
    double total = round2(pkg.getPrice() * (1.0 - discount));

    cout << "\n--- Booking summary ---\n"
         << "  Customer     : " << cust.getName() << " (" << cust.getMembership() << ")\n"
         << "  Photographer : " << phot.getName() << " - " << phot.roleDescription() << "\n"
         << "  Package      : " << pkg.getName() << " (" << pkg.getDurationHours() << " h, " << pkg.getNumPhotos() << " photos)\n"
         << "  Date / time  : " << date << ", " << hh(start) << " to " << hh(end) << "\n"
         << "  Price        : Rs. " << money(pkg.getPrice()) << "\n"
         << "  Discount     : " << (int)(discount * 100 + 0.5) << "%\n"
         << "  TOTAL        : Rs. " << money(total) << "\n";
    if (!Input::askYesNo("Confirm this booking? (y/n): ")) { cout << "Booking abandoned.\n"; return; }

    Booking b(nextId(bookings), cust.getId(), phot.getId(), pkg.getId(), date, start, pkg.getDurationHours(), total, "Confirmed");
    bookings.add(b);
    unsaved = true;
    cout << "Booking confirmed! Booking ID = " << b.getId() << "\n";
}

void Studio::cancelBooking() {
    int id = Input::askInt("Booking ID to cancel: ", 1, 999999999);
    int idx = indexOrThrow(bookings, id, "Booking");
    Booking& b = bookings.at(idx);
    if (!b.isActive()) throw BookingException("Booking " + to_string(id) + " is already " + b.getStatus() + ".");
    printBookings(vector<int>(1, idx));
    if (!Input::askYesNo("Cancel this booking? (y/n): ")) { cout << "Nothing changed.\n"; return; }
    b.setStatus("Cancelled");
    unsaved = true;
    cout << "Booking cancelled. The time slot is free again.\n";
}

void Studio::completeBooking() {
    int id = Input::askInt("Booking ID to complete: ", 1, 999999999);
    int idx = indexOrThrow(bookings, id, "Booking");
    Booking& b = bookings.at(idx);
    if (!b.isActive()) throw BookingException("Booking " + to_string(id) + " is " + b.getStatus() + " and cannot be completed.");
    b.setStatus("Completed");
    unsaved = true;
    cout << "Booking " << id << " marked as Completed (Rs. " << money(b.getTotalPrice()) << " earned).\n";
}

void Studio::viewSchedule() {
    string d = Input::askText("Date (YYYY-MM-DD): ", Validator::checkDate);
    vector<int> idx;
    for (int i = 0; i < bookings.size(); ++i)
        if (bookings.at(i).isActive() && bookings.at(i).getDate() == d) idx.push_back(i);
    sort(idx.begin(), idx.end(), [this](int a, int b) { return bookings.at(a).getStartHour() < bookings.at(b).getStartHour(); });
    cout << "\nConfirmed sessions on " << d << ":\n";
    printBookings(idx);
}

// ============================================================ 7. REPORT
void Studio::report() const {
    int nPh = photographers.size(), nPk = packages.size(), nCu = customers.size();

    // temporary working arrays on the heap, freed with delete[] at the end of the function
    int*    phCount = new int[nPh > 0 ? nPh : 1]();
    double* phMoney = new double[nPh > 0 ? nPh : 1]();
    int*    pkCount = new int[nPk > 0 ? nPk : 1]();
    double* cuSpend = new double[nCu > 0 ? nCu : 1]();

    int confirmed = 0, completed = 0, cancelled = 0;
    double earned = 0, expected = 0;
    for (int i = 0; i < bookings.size(); ++i) {
        const Booking& b = bookings.at(i);
        if (b.getStatus() == "Cancelled") { ++cancelled; continue; }
        if (b.getStatus() == "Completed") { ++completed; earned += b.getTotalPrice(); }
        else { ++confirmed; expected += b.getTotalPrice(); }
        int p = photographers.indexOfId(b.getPhotographerId());
        if (p >= 0) { ++phCount[p]; phMoney[p] += b.getTotalPrice(); }
        int k = packages.indexOfId(b.getPackageId());
        if (k >= 0) ++pkCount[k];
        int c = customers.indexOfId(b.getCustomerId());
        if (c >= 0) cuSpend[c] += b.getTotalPrice();
    }

    cout << "\n"; line();
    cout << "  " << name << " - SUMMARY REPORT   (" << Validator::today() << ")\n"; line();
    cout << "  Customers: " << nCu << "   Photographers: " << nPh << "   Packages: " << nPk
         << "   Bookings: " << bookings.size() << "\n";
    cout << "  Confirmed: " << confirmed << "   Completed: " << completed << "   Cancelled: " << cancelled << "\n";
    cout << "  Revenue earned (completed) : Rs. " << money(earned) << "\n";
    cout << "  Revenue expected (upcoming): Rs. " << money(expected) << "\n";

    cout << "\n  Bookings per photographer (excluding cancelled)\n  " << string(60, '-') << "\n";
    for (int i = 0; i < nPh; ++i)
        cout << "  " << Validator::fit(photographers.at(i).getName(), 24) << Validator::fit(photographers.at(i).getSpecialty(), 16)
             << Validator::fit(to_string(phCount[i]) + " booking(s)", 14) << "Rs. " << money(phMoney[i]) << "\n";

    cout << "\n  Package popularity (excluding cancelled)\n  " << string(60, '-') << "\n";
    int bestPk = -1;
    for (int i = 0; i < nPk; ++i) {
        cout << "  " << Validator::fit(packages.at(i).getName(), 28) << pkCount[i] << " booking(s)\n";
        if (pkCount[i] > 0 && (bestPk < 0 || pkCount[i] > pkCount[bestPk])) bestPk = i;
    }
    if (bestPk >= 0) cout << "  Most popular package: " << packages.at(bestPk).getName() << "\n";

    int bestCu = -1;
    for (int i = 0; i < nCu; ++i) if (cuSpend[i] > 0 && (bestCu < 0 || cuSpend[i] > cuSpend[bestCu])) bestCu = i;
    if (bestCu >= 0) cout << "  Top customer by spend: " << customers.at(bestCu).getName() << " (Rs. " << money(cuSpend[bestCu]) << ")\n";

    // next upcoming sessions
    vector<int> up;
    string today = Validator::today();
    for (int i = 0; i < bookings.size(); ++i)
        if (bookings.at(i).isActive() && bookings.at(i).getDate() >= today) up.push_back(i);
    sort(up.begin(), up.end(), [this](int a, int b) {
        const Booking& x = bookings.at(a); const Booking& y = bookings.at(b);
        return x.getDate() != y.getDate() ? x.getDate() < y.getDate() : x.getStartHour() < y.getStartHour();
    });
    if (up.size() > 5) up.resize(5);
    cout << "\n  Next upcoming sessions\n";
    printBookings(up);
    line();

    delete[] phCount;
    delete[] phMoney;
    delete[] pkCount;
    delete[] cuSpend;
}
