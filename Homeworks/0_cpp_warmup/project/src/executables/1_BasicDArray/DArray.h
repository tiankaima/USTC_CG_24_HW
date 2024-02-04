#pragma once
#include "iostream"


class DArray {
public:
	DArray();

	DArray(int nSize, double dValue = 0);

	DArray(const DArray &arr);

	~DArray();

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

	DArray &operator =(const DArray &arr);

private:
	double *m_pData;
	int m_nSize;
};
