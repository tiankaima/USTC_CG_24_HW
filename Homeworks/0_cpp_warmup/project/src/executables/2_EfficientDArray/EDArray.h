#pragma once

#include "cassert"
#include "iostream"

class EDArray {
public:
    EDArray();

    explicit EDArray(int size, double value = 0);

    EDArray(const EDArray& arr);

    ~EDArray();

    EDArray& operator=(const EDArray& arr);

    friend auto operator<<(std::ostream& os, const EDArray& arr)
        -> std::ostream&;

    double& operator[](int index);

    const double& operator[](int index) const;

    //! non-standard interfaces

    [[nodiscard]] int GetSize() const;

    void SetSize(int size);

    void PushBack(double value);

    void DeleteAt(int index);

    void InsertAt(int index, double value);

    //! alternative available for the following methods

    void Print() const;

    [[nodiscard]] const double& GetAt(int index) const;

    const void SetAt(int index, double value) const;

private:
    double* data;
    int size; // used size.
    int allocated_size;
};
