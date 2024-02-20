#include "PolynomialMap.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

#define EPSILON 1.0e-10

PolynomialMap::PolynomialMap(const PolynomialMap& other)
{
    this->polynomial = other.polynomial;
}

PolynomialMap::PolynomialMap(const std::string& file)
{
    std::ifstream in(file);
    if (!in.is_open()) {
        throw std::runtime_error("File not found");
    }

    char P;
    int n;
    in >> P >> n;

    for (int i = 0; i < n; i++) {
        int degree;
        double coefficent;
        in >> degree >> coefficent;
        this->polynomial[degree] = coefficent;
    }

    in.close();
}

PolynomialMap::PolynomialMap(const double* coefficent, const int* degree, int n)
{
    for (int i = 0; i < n; i++) {
        this->polynomial[degree[i]] = coefficent[i];
    }
}

PolynomialMap::PolynomialMap(const std::vector<double>& coefficent, const std::vector<int>& degree)
{
    if (coefficent.size() != degree.size()) {
        throw std::runtime_error("Size of coefficent and degree are not equal");
    }

    for (size_t i = 0; i < coefficent.size(); i++) {
        this->polynomial[degree[i]] = coefficent[i];
    }
}

double PolynomialMap::cofficent(int i) const
{
    auto it = this->polynomial.find(i);
    if (it != this->polynomial.end()) {
        return it->second;
    }
    return 0;
}

double& PolynomialMap::coefficent(int i)
{
    return this->polynomial[i];
}

void PolynomialMap::compress()
{
    std::map<int, double> new_polynomial;
    for (const auto& term : this->polynomial) {
        if (std::abs(term.second) > EPSILON) {
            new_polynomial[term.first] = term.second;
        }
    }
    this->polynomial = new_polynomial;
}

PolynomialMap PolynomialMap::operator+(const PolynomialMap& right) const
{
    PolynomialMap result(*this);
    for (const auto& term : right.polynomial) {
        result.polynomial[term.first] += term.second;
    }
    result.compress();
    return result;
}

PolynomialMap PolynomialMap::operator-(const PolynomialMap& right) const
{
    PolynomialMap result(*this);
    for (const auto& term : right.polynomial) {
        result.polynomial[term.first] -= term.second;
    }
    result.compress();
    return result;
}

PolynomialMap PolynomialMap::operator*(const PolynomialMap& right) const
{
    PolynomialMap result;
    for (const auto& term1 : this->polynomial) {
        for (const auto& term2 : right.polynomial) {
            int degree = term1.first + term2.first;
            double coefficent = term1.second * term2.second;
            result.polynomial[degree] += coefficent;
        }
    }
    result.compress();
    return result;
}

PolynomialMap& PolynomialMap::operator=(const PolynomialMap& right)
{
    this->polynomial = right.polynomial;
    return *this;
}

auto operator<<(std::ostream& os, const PolynomialMap& poly) -> std::ostream&
{
    if (poly.polynomial.empty()) {
        os << "0";
        return os;
    }

    for (const auto& term : poly.polynomial) {
        if (&term != &*poly.polynomial.begin()) {
            os << " ";
            if (term.second > 0) {
                os << "+";
            }
        }
        os << term.second;
        if (term.first != 0) {
            os << "x^" << term.first;
        }
    }
    return os;
}

void PolynomialMap::Print() const
{
    std::cout << *this << std::endl;
}
