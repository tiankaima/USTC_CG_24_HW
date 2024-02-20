#pragma once

#include <cassert>
#include <iostream>

template <class T = double>
class TEDArray {
public:
    TEDArray()
        : data(nullptr)
        , size(0)
        , allocated_size(0)
    {
    }

    explicit TEDArray(int size, const T& value = static_cast<T>(0))
        : size(size)
        , allocated_size(size)
    {
        this->data = new T[size];
        for (int i = 0; i < size; i++) {
            this->data[i] = value;
        }
    }

    TEDArray(const TEDArray& arr)
    {
        this->size = arr.size;
        this->allocated_size = arr.size;
        this->data = new T[arr.size];
        for (int i = 0; i < this->size; i++) {
            this->data[i] = arr.data[i];
        }
    }

    ~TEDArray()
    {
        delete[] this->data;
        this->size = 0;
        this->allocated_size = 0;
    }

    TEDArray& operator=(const TEDArray& arr)
    {
        if (this == &arr) {
            return *this;
        }
        delete[] this->data;
        this->size = arr.size;
        this->allocated_size = arr.size;
        this->data = new T[arr.size];
        for (int i = 0; i < this->size; i++) {
            this->data[i] = arr.data[i];
        }
        return *this;
    }

    friend auto operator<<(std::ostream& os, const TEDArray& arr) -> std::ostream&
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

    double& operator[](int index)
    {
        assert(index >= 0 && index < this->size);

        return this->data[index];
    }

    const double& operator[](int index) const
    {
        assert(index >= 0 && index < this->size);

        return this->data[index];
    }

    //! non-standard interfaces

    [[nodiscard]] int GetSize() const
    {
        return this->size;
    }

    void SetSize(int size)
    {
        assert(size >= 0);

        if (this->allocated_size >= size) {
            this->size = size;
        } else {
            T* new_data = new T[size];
            for (int i = 0; i < this->size; i++) {
                new_data[i] = this->data[i];
            }
            delete[] this->data;
            this->data = new_data;
            this->size = size;
            this->allocated_size = size;
        }
    }

    void PushBack(double value)
    {
        if (this->size == this->allocated_size) {
            this->allocated_size = this->allocated_size == 0 ? 1 : this->allocated_size * 2;
            T* new_data = new T[this->allocated_size];
            for (int i = 0; i < this->size; i++) {
                new_data[i] = this->data[i];
            }
            delete[] this->data;
            this->data = new_data;
        }
        this->data[this->size++] = value;
    }

    void DeleteAt(int index)
    {
        assert(index >= 0 && index < this->size);

        for (int i = index; i < this->size - 1; i++) {
            this->data[i] = this->data[i + 1];
        }
        this->size--;
    }

    void InsertAt(int index, double value)
    {
        assert(index >= 0 && index <= this->size);

        if (this->size == this->allocated_size) {
            this->allocated_size = this->allocated_size == 0 ? 1 : this->allocated_size * 2;
            T* new_data = new T[this->allocated_size];
            for (int i = 0; i < index; i++) {
                new_data[i] = this->data[i];
            }
            new_data[index] = value;
            for (int i = index; i < this->size; i++) {
                new_data[i + 1] = this->data[i];
            }
            delete[] this->data;
            this->data = new_data;
            this->size++;
        } else {
            for (int i = this->size; i > index; i--) {
                this->data[i] = this->data[i - 1];
            }
            this->data[index] = value;
            this->size++;
        }
    }

    //! alternative available for the following methods

    void Print() const
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

    [[nodiscard]] const double& GetAt(int index) const
    {
        assert(index >= 0 && index < this->size);

        return this->data[index];
    }

    const void SetAt(int index, double value) const
    {
        assert(index >= 0 && index < this->size);

        this->data[index] = value;
    }

private:
    T* data;
    int size; // used size.
    int allocated_size;
};
