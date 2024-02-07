#pragma once

#include "iostream"

class EDArray {
public:
	EDArray();

	EDArray(int nSize, double dValue = 0);

	EDArray(const EDArray &arr);

	~EDArray();

	void Print() const;

	int GetSize() const;

	void SetSize(int nSize);

	const double &GetAt(int nIndex) const;

	void SetAt(int nIndex, double dValue);

	double &operator[](int nIndex);

	const double &operator[](int nIndex) const;

	void PushBack(double dValue);

	void DeleteAt(int nIndex);

	void InsertAt(int nIndex, double dValue);

	EDArray &operator =(const EDArray &arr);

private:
	double *m_pData;
	int m_nSize;
	int m_nMax;
};
