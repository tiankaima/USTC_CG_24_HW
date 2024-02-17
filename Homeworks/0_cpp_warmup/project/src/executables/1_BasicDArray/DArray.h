#pragma once

#include "iostream"

class DArray {
public:
	DArray();

	explicit DArray(int size, double data = 0);

	DArray(const DArray &arr);

	~DArray();

	DArray &operator =(const DArray &arr);

	friend auto operator<<(std::ostream &os, const DArray &arr) -> std::ostream &;

	double &operator[](int index);

	const double &operator[](int index) const;

public: // non-standard interfaces
	void Print() const;

	[[nodiscard]] int GetSize() const;

	void SetSize(int size);

	[[nodiscard]] const double &GetAt(int index) const;

	void SetAt(int index, double value) const;

	void PushBack(double value);

	void DeleteAt(int index);

	void InsertAt(int index, double value);

private:
	int size;
	double *data;
};
