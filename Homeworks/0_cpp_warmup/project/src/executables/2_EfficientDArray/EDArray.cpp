
#include "EDArray.h"


EDArray::EDArray() {
	this->m_pData = nullptr;
	this->m_nSize = 0;
	this->m_nMax = 0;
}


EDArray::EDArray(int nSize, double dValue) {
	this->m_pData = new double[nSize];
	this->m_nSize = nSize;
	this->m_nMax = nSize;
	for (int i = 0; i < nSize; i++) {
		this->m_pData[i] = dValue;
	}
}

EDArray::EDArray(const EDArray &arr) {
	this->m_pData = new double[arr.m_nSize];
	this->m_nSize = arr.m_nSize;
	this->m_nMax = arr.m_nSize;
	for (int i = 0; i < arr.m_nSize; i++) {
		this->m_pData[i] = arr.m_pData[i];
	}
}


EDArray::~EDArray() {
	delete[] this->m_pData;
	this->m_pData = nullptr;
	this->m_nSize = 0;
	this->m_nMax = 0;
}


void EDArray::Print() const {
	std::cout << "[";
	for (int i = 0; i < this->m_nSize; i++) {
		std::cout << this->m_pData[i];
		if (i < this->m_nSize - 1) {
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;
}

int EDArray::GetSize() const {
	return this->m_nSize;
}


void EDArray::SetSize(int nSize) {
	if (this->m_nSize >= nSize) {
		// no new allocation needed, just update size and set 0 for new elements
		for (int i = nSize; i < this->m_nSize; i++) {
			this->m_pData[i] = 0;
		}
		this->m_nSize = nSize;
	} else {
		// allocate new double[] and copy values, then release old memory
		auto *newData = new double[nSize];
		for (int i = 0; i < this->m_nSize; i++) {
			newData[i] = this->m_pData[i];
		}
		for (int i = this->m_nSize; i < nSize; i++) {
			newData[i] = 0;
		}
		delete[] this->m_pData;
		this->m_pData = newData;
		this->m_nSize = nSize;
		this->m_nMax = nSize;
	}
}


const double &EDArray::GetAt(int nIndex) const {
	return this->m_pData[nIndex];
}


void EDArray::SetAt(int nIndex, double dValue) {
	this->m_pData[nIndex] = dValue;
}


double &EDArray::operator[](int nIndex) {

}


const double &EDArray::operator[](int nIndex) const {
}


void EDArray::PushBack(double dValue) {
}


void EDArray::DeleteAt(int nIndex) {
}


void EDArray::InsertAt(int nIndex, double dValue) {
}


EDArray &EDArray::operator =(const EDArray &arr) {
	return *this;
}
