// Record.h - abstract base class of every stored entity (inheritance root).
// The pure virtual functions give the whole system RUNTIME POLYMORPHISM:
// generic code can call display(), toFileLine(), matches() on any Record.
#ifndef RECORD_H
#define RECORD_H

#include <string>

class Record {
private:
    int id;

public:
    Record();
    explicit Record(int id);
    Record(const Record&) = default;
    Record& operator=(const Record&) = default;
    virtual ~Record();                                   // virtual destructor

    int  getId() const;
    void setId(int newId);                               // validates id > 0

    virtual std::string typeName() const = 0;            // "Customer", "Package" ...
    virtual std::string header() const = 0;              // table heading
    virtual void        display() const = 0;             // one table row
    virtual std::string toFileLine() const = 0;          // '|' separated line for the data file
    virtual bool        matches(const std::string& keyword) const = 0;   // search
};

#endif
