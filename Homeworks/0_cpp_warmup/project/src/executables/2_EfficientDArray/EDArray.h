#pragma once

#include "iostream"

class EDArray {
public:
	EDArray();

	explicit EDArray(int nSize, double dValue = 0);

	EDArray(const EDArray &arr);

	~EDArray();

	void Print() const;

	[[nodiscard]] int GetSize() const;

	void SetSize(int nSize);

	[[nodiscard]] const double &GetAt(int nIndex) const;

	void SetAt(int nIndex, double dValue) const;

	double &operator[](int nIndex);

	const double &operator[](int nIndex) const;

	void PushBack(double dValue);

	void DeleteAt(int nIndex);

	void InsertAt(int nIndex, double dValue);

	EDArray &operator =(const EDArray &arr);

private:
	double *m_pData;
	int m_nSize; // number of elements
	int m_nMax; // size of allocated memory
};
