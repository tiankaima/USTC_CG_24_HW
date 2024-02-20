#pragma once

#include "cassert"
#include "iostream"

class DArray {
public:
    DArray();

    explicit DArray(int size, double data = 0);

    DArray(const DArray& arr);

    ~DArray();

    DArray& operator=(const DArray& arr);

    friend auto operator<<(std::ostream& os, const DArray& arr) -> std::ostream&;

    double& operator[](int index);

    const double& operator[](int index) const;

    //! non-standard interfaces

    [[nodiscard]] int GetSize() const;

    void SetSize(int size);

    void PushBack(double value);

    void DeleteAt(int index);

    void InsertAt(int index, double value);

    //! alternative available for the following methods

    void Print() const; // use cout<< instead.

    [[nodiscard]] double& GetAt(int index) const; // use operator[] instead.

    const void SetAt(int index, double value) const; // use operator[] instead.

private:
    int size;
    double* data;
};
