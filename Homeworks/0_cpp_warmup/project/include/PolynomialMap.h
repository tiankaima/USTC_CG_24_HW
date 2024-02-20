#pragma once

#include <map>
#include <string>
#include <vector>

class PolynomialMap {
public:
    PolynomialMap() { }
    PolynomialMap(const PolynomialMap& other);
    PolynomialMap(const std::string& file);
    PolynomialMap(const double* coefficent, const int* degree, int n);
    PolynomialMap(const std::vector<double>& coefficent, const std::vector<int>& degree);

    double& coefficent(int i);
    double cofficent(int i) const;

    void compress();

    PolynomialMap operator+(const PolynomialMap& right) const;
    PolynomialMap operator-(const PolynomialMap& right) const;
    PolynomialMap operator*(const PolynomialMap& right) const;
    PolynomialMap& operator=(const PolynomialMap& right);

    friend auto operator<<(std::ostream& os, const PolynomialMap& poly) -> std::ostream&;

    void Print() const;

private:
    std::map<int, double> polynomial;
};
