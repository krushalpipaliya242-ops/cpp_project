// DynArray.h - growable array built on dynamic memory (new[] / delete[]).
//
// Purpose: the studio does not know in advance how many customers, photographers,
// packages or bookings it will hold. DynArray allocates a block on the heap with
// `new T[capacity]`, doubles it when full, and releases it with `delete[]` in the
// destructor, so memory use always matches the amount of data.
#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stdexcept>

template <class T>
class DynArray {
private:
    T*  data;        // heap block
    int count;       // elements in use
    int capacity;    // elements allocated

    void grow() {
        int newCap = capacity * 2;
        T* bigger = new T[newCap];                      // allocate larger block
        for (int i = 0; i < count; ++i) bigger[i] = data[i];
        delete[] data;                                  // release old block
        data = bigger;
        capacity = newCap;
    }

public:
    explicit DynArray(int initialCapacity = 4)
        : data(nullptr), count(0), capacity(initialCapacity > 0 ? initialCapacity : 4) {
        data = new T[capacity];
    }

    ~DynArray() { delete[] data; }

    // Owning raw pointer -> copying is disabled to prevent double delete[].
    DynArray(const DynArray&) = delete;
    DynArray& operator=(const DynArray&) = delete;

    void add(const T& item) {
        if (count == capacity) grow();
        data[count++] = item;
    }

    void removeAt(int index) {
        if (index < 0 || index >= count) throw std::out_of_range("DynArray::removeAt");
        for (int i = index; i < count - 1; ++i) data[i] = data[i + 1];
        --count;
    }

    int indexOfId(int id) const {
        for (int i = 0; i < count; ++i)
            if (data[i].getId() == id) return i;
        return -1;
    }

    T& at(int index) {
        if (index < 0 || index >= count) throw std::out_of_range("DynArray::at");
        return data[index];
    }
    const T& at(int index) const {
        if (index < 0 || index >= count) throw std::out_of_range("DynArray::at");
        return data[index];
    }

    void clear() { count = 0; }
    int size() const { return count; }
    int getCapacity() const { return capacity; }
};

#endif
