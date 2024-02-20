#include "DArray.h"

DArray::DArray()
    : size(0)
    , data(nullptr)
{
}

DArray::DArray(int size, double data)
    : size(size)
    , data(new double[size])
{
    for (int i = 0; i < size; i++) {
        this->data[i] = data;
    }
}

DArray::DArray(const DArray& arr)
    : size(arr.size)
    , data(new double[arr.size])
{
    for (int i = 0; i < arr.size; i++) {
        this->data[i] = arr.data[i];
    }
}

DArray::~DArray()
{
    delete[] this->data;
    this->data = nullptr;
    this->size = 0;
}

DArray& DArray::operator=(const DArray& arr)
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

auto operator<<(std::ostream& os, const DArray& arr) -> std::ostream&
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

double& DArray::operator[](int index)
{
    assert(index >= 0 && index < this->size);

    return this->data[index];
}

const double& DArray::operator[](int index) const
{
    assert(index >= 0 && index < this->size);

    return this->data[index];
}

void DArray::Print() const
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

int DArray::GetSize() const
{
    return this->size;
}

void DArray::SetSize(int size)
{
    assert(size >= 0);

    auto* pNewData = new double[size];
    for (int i = 0; i < size; i++) {
        pNewData[i] = i < this->size ? this->data[i] : 0;
    }
    delete[] this->data;
    this->data = pNewData;
    this->size = size;
}

double& DArray::GetAt(int index) const
{
    assert(index >= 0 && index < this->size);

    return this->data[index];
}

const void DArray::SetAt(int index, double value) const
{
    assert(index >= 0 && index < this->size);

    this->data[index] = value;
}

void DArray::PushBack(double value)
{
    this->size++;
    auto* pNewData = new double[this->size];
    for (int i = 0; i < this->size - 1; i++) {
        pNewData[i] = this->data[i];
    }
    pNewData[this->size - 1] = value;
    delete[] this->data;
    this->data = pNewData;
}

void DArray::DeleteAt(int index)
{
    assert(index >= 0 && index < this->size);

    this->size--;
    auto* pNewData = new double[this->size];
    for (int i = 0; i < index; i++) {
        pNewData[i] = this->data[i];
    }
    for (int i = index; i < this->size; i++) {
        pNewData[i] = this->data[i + 1];
    }
    delete[] this->data;
    this->data = pNewData;
}

void DArray::InsertAt(int index, double value)
{
    assert(index >= 0 && index <= this->size);

    this->size++;
    auto* pNewData = new double[this->size];
    for (int i = 0; i < index; i++) {
        pNewData[i] = this->data[i];
    }
    pNewData[index] = value;
    for (int i = index + 1; i < this->size; i++) {
        pNewData[i] = this->data[i - 1];
    }
    delete[] this->data;
    this->data = pNewData;
}
