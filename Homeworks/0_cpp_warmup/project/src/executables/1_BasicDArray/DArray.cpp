
#include "DArray.h"


DArray::DArray() {
	this->m_nSize = 0;
	this->m_pData = nullptr;
}


DArray::DArray(int nSize, double dValue) {
	this->m_nSize = nSize;
	this->m_pData = new double[nSize];
	for (int i = 0; i < nSize; i++) {
		this->m_pData[i] = dValue;
	}
}

DArray::DArray(const DArray &arr) {
	this->m_nSize = arr.m_nSize;
	this->m_pData = new double[arr.m_nSize];
	for (int i = 0; i < arr.m_nSize; i++) {
		this->m_pData[i] = arr.m_pData[i];
	}
}


DArray::~DArray() {
	delete[] this->m_pData;
	this->m_pData = nullptr;
	this->m_nSize = 0;
}


void DArray::Print() const {
	std::cout << "[";
	for (int i = 0; i < this->m_nSize; i++) {
		std::cout << this->m_pData[i];
		if (i < this->m_nSize - 1) {
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;
}


int DArray::GetSize() const {
	return this->m_nSize;
}


void DArray::SetSize(int nSize) {
	delete[] this->m_pData;
	this->m_pData = new double[nSize];
	this->m_nSize = nSize;
}


const double &DArray::GetAt(int nIndex) const {
	return this->m_pData[nIndex];
}


void DArray::SetAt(int nIndex, double dValue) {
	this->m_pData[nIndex] = dValue;
}


const double &DArray::operator[](int nIndex) const {
	return this->m_pData[nIndex];
}


void DArray::PushBack(double dValue) {
	this->m_nSize++;
	auto *pNewData = new double[this->m_nSize];
	for (int i = 0; i < this->m_nSize - 1; i++) {
		pNewData[i] = this->m_pData[i];
	}
	pNewData[this->m_nSize - 1] = dValue;
	delete[] this->m_pData;
	this->m_pData = pNewData;
}


void DArray::DeleteAt(int nIndex) {
	if (nIndex < 0 || nIndex >= this->m_nSize) {
		throw std::out_of_range("Index out of range");
	}
	this->m_nSize--;
	auto *pNewData = new double[this->m_nSize];
	for (int i = 0; i < nIndex; i++) {
		pNewData[i] = this->m_pData[i];
	}
	for (int i = nIndex; i < this->m_nSize; i++) {
		pNewData[i] = this->m_pData[i + 1];
	}
	delete[] this->m_pData;
	this->m_pData = pNewData;
}


void DArray::InsertAt(int nIndex, double dValue) {
	if (nIndex < 0 || nIndex > this->m_nSize) {
		throw std::out_of_range("Index out of range");
	}
	this->m_nSize++;
	auto *pNewData = new double[this->m_nSize];
	for (int i = 0; i < nIndex; i++) {
		pNewData[i] = this->m_pData[i];
	}
	pNewData[nIndex] = dValue;
	for (int i = nIndex + 1; i < this->m_nSize; i++) {
		pNewData[i] = this->m_pData[i - 1];
	}
	delete[] this->m_pData;
	this->m_pData = pNewData;
}


DArray &DArray::operator =(const DArray &arr) {
	if (this == &arr) {
		return *this;
	}
	delete[] this->m_pData;
	this->m_nSize = arr.m_nSize;
	this->m_pData = new double[arr.m_nSize];
	for (int i = 0; i < arr.m_nSize; i++) {
		this->m_pData[i] = arr.m_pData[i];
	}
	return *this;
}
