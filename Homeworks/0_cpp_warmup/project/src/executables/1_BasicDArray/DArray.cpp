#include "DArray.h"

DArray::DArray(): size(0), data(nullptr) {
}

DArray::DArray(int size, double data): size(size), data(new double[size]) {
	for (int i = 0; i < size; i++) {
		this->data[i] = data;
	}
}

DArray::DArray(const DArray &arr): size(arr.size), data(new double[arr.size]) {
	for (int i = 0; i < arr.size; i++) {
		this->data[i] = arr.data[i];
	}
}

DArray::~DArray() {
	delete[] this->data;
	this->data = nullptr;
	this->size = 0;
}

DArray &DArray::operator =(const DArray &arr) {
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

auto operator<<(std::ostream &os, const DArray &arr) -> std::ostream & {
	os << "[";
	for (int i = 0; i < arr.size; i++) {
		os << arr.data[i];
		if (i < arr.size - 1) {
			os << ", ";
		}
	}
	os << "]" << std::endl;
	return os;
}


double &DArray::operator[](int index) {
	double &ref = this->data[index];
	return ref;
}

const double &DArray::operator[](int index) const {
	return this->data[index];
}


void DArray::Print() const {
	std::cout << "[";
	for (int i = 0; i < this->size; i++) {
		std::cout << this->data[i];
		if (i < this->size - 1) {
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;
}


int DArray::GetSize() const {
	return this->size;
}

void DArray::SetSize(int size) {
	delete[] this->data;
	this->data = new double[size];
	this->size = size;
}

const double &DArray::GetAt(int index) const {
	return this->data[index];
}

void DArray::SetAt(int index, double value) const {
	this->data[index] = value;
}


void DArray::PushBack(double value) {
	this->size++;
	auto *pNewData = new double[this->size];
	for (int i = 0; i < this->size - 1; i++) {
		pNewData[i] = this->data[i];
	}
	pNewData[this->size - 1] = value;
	delete[] this->data;
	this->data = pNewData;
}

void DArray::DeleteAt(int index) {
	if (index < 0 || index >= this->size) {
		throw std::out_of_range("Index out of range");
	}
	this->size--;
	auto *pNewData = new double[this->size];
	for (int i = 0; i < index; i++) {
		pNewData[i] = this->data[i];
	}
	for (int i = index; i < this->size; i++) {
		pNewData[i] = this->data[i + 1];
	}
	delete[] this->data;
	this->data = pNewData;
}

void DArray::InsertAt(int index, double value) {
	if (index < 0 || index > this->size) {
		throw std::out_of_range("Index out of range");
	}
	this->size++;
	auto *pNewData = new double[this->size];
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
