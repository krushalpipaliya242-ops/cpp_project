#include "Record.h"
#include "Exceptions.h"

Record::Record() : id(0) {}
Record::Record(int newId) : id(0) { setId(newId); }
Record::~Record() {}

int Record::getId() const { return id; }

void Record::setId(int newId) {
    if (newId <= 0) throw ValidationException("ID must be a positive number.");
    id = newId;
}
