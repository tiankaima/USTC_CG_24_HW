#include "EDArray.h"
#include <cassert>

EDArray::EDArray()
    : data(nullptr)
    , size(0)
    , allocated_size(0)
{
}

EDArray::EDArray(int size, double value)
    : data(new double[size])
    , size(size)
    , allocated_size(size)
{
    for (int i = 0; i < size; i++) {
        this->data[i] = value;
    }
}

EDArray::EDArray(const EDArray& arr)
    : data(new double[arr.size])
    , size(arr.size)
    , allocated_size(arr.size)
{
    for (int i = 0; i < arr.size; i++) {
        this->data[i] = arr.data[i];
    }
}

EDArray::~EDArray()
{
    delete[] this->data;
    this->data = nullptr;
    this->size = 0;
    this->allocated_size = 0;
}

EDArray& EDArray::operator=(const EDArray& arr)
{
    if (this == &arr) {
        return *this;
    }
    delete[] this->data;
    this->size = arr.size;
    this->data = new double[arr.size];
    for (int i = 0; i < arr.size; i++) {
        this->data[i] = arr.data[i];
    }
    return *this;
}

auto operator<<(std::ostream& os, const EDArray& arr) -> std::ostream&
{
    os << "[";
    for (int i = 0; i < arr.size; i++) {
        os << arr.data[i];
        if (i < arr.size - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

double& EDArray::operator[](int index)
{
    assert(index >= 0 && index < this->size);

    return this->data[index];
}

const double& EDArray::operator[](int index) const
{
    assert(index >= 0 && index < this->size);

    return this->data[index];
}

void EDArray::Print() const
{
    std::cout << "[";
    for (int i = 0; i < this->size; i++) {
        std::cout << this->data[i];
        if (i < this->size - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

int EDArray::GetSize() const
{
    return this->size;
}

void EDArray::SetSize(int size)
{
    assert(size >= 0);

    if (this->allocated_size >= size) {
        for (int i = this->size; i < size; i++) {
            this->data[i] = 0;
        }
        this->size = size;
    } else {
        auto* newData = new double[size];
        for (int i = 0; i < this->size; i++) {
            newData[i] = this->data[i];
        }
        for (int i = this->size; i < size; i++) {
            newData[i] = 0;
        }
        delete[] this->data;
        this->data = newData;
        this->size = size;
        this->allocated_size = size;
    }
}

const double& EDArray::GetAt(int index) const
{
    assert(index >= 0 && index < this->size);

    return this->data[index];
}

const void EDArray::SetAt(int index, double value) const
{
    assert(index >= 0 && index < this->size);

    this->data[index] = value;
}

void EDArray::PushBack(double value)
{
    if (this->allocated_size > this->size) {
        // no new allocation is required.
        this->data[this->size] = value;
        this->size++;
    } else {
        // create a new array with this.size + 1:
        auto* newData = new double[this->size + 1];
        for (int i = 0; i < this->size; i++) {
            newData[i] = this->data[i];
        }
        newData[this->size] = value;
        delete[] this->data;
        this->data = newData;
        this->size++;
        this->allocated_size = this->size;
    }
}

void EDArray::DeleteAt(int index)
{
    // move foward all the elements after index:
    for (int i = index; i < this->size - 1; i++) {
        this->data[i] = this->data[i + 1];
    }
    this->size--;
}

void EDArray::InsertAt(int index, double value)
{
    if (this->allocated_size > this->size) {
        // no new allocation is required.
        for (int i = this->size; i > index; i--) {
            this->data[i] = this->data[i - 1];
        }
        this->data[index] = value;
        this->size++;
    } else {
        // create a new array with this.size + 1:
        auto* newData = new double[this->size + 1];
        for (int i = 0; i < index; i++) {
            newData[i] = this->data[i];
        }
        newData[index] = value;
        for (int i = index; i < this->size; i++) {
            newData[i + 1] = this->data[i];
        }
        delete[] this->data;
        this->data = newData;
        this->size++;
        this->allocated_size = this->size;
    }
}
