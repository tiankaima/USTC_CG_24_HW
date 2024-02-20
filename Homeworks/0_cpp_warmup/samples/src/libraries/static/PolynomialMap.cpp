#include "PolynomialMap.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>

#define EPSILON 1.0e-10	// zero double

using namespace std;

PolynomialMap::PolynomialMap(const PolynomialMap& other) {
    polynomial = other.polynomial;
}

PolynomialMap::PolynomialMap(const string& file) {
    ReadFromFile(file);
}

PolynomialMap::PolynomialMap(const double* cof, const int* deg, int n) {
    for (int i = 0; i < n; i++)
        coefficent(deg[i]) = cof[i];
}

PolynomialMap::PolynomialMap(const vector<int>& deg, const vector<double>& cof) {
    assert(deg.size() == cof.size());

    for (size_t i = 0; i < deg.size(); i++)
        coefficent(deg[i]) = cof[i];
}

double PolynomialMap::cofficent(int i) const
{
    auto target = polynomial.find(i);
    if (target == polynomial.end())
        return 0.;

    return target->second;
}

double& PolynomialMap::coefficent(int i) {
    return polynomial[i];
}

void PolynomialMap::compress() {
    /// Safe but not effective method--- to study stl for getting more effective method
    map<int, double> tmpPoly = polynomial;
    polynomial.clear();
    for (const auto& term : tmpPoly) {
        if (fabs(term.second) > EPSILON)
            coefficent(term.first) = term.second;
    }
}

PolynomialMap PolynomialMap::operator+(const PolynomialMap& right) const {
    PolynomialMap poly(right);
    for (const auto& term : polynomial)
        poly.coefficent(term.first) += term.second;

    poly.compress();
    return poly;
}

PolynomialMap PolynomialMap::operator-(const PolynomialMap& right) const {
    PolynomialMap poly(right);
    for (const auto& term : polynomial)
        poly.coefficent(term.first) -= term.second;

    poly.compress();
    return poly;
}

PolynomialMap PolynomialMap::operator*(const PolynomialMap& right) const {
    PolynomialMap poly;
    for (const auto& term1 : polynomial) {
        for (const auto& term2 : right.polynomial) {
            int deg = term1.first + term2.first;
            double cof = term1.second * term2.second;
            poly.coefficent(deg) += cof;
        }
    }
    return poly;
}

PolynomialMap& PolynomialMap::operator=(const PolynomialMap& right) {
    polynomial = right.polynomial;
    return *this;
}

void PolynomialMap::Print() const {
    auto itr = polynomial.begin();
    if (itr == polynomial.end()) {
        cout << "0" << endl;
        return;
    }

    for (; itr != polynomial.end(); itr++) {
        if (itr != polynomial.begin()) {
            cout << " ";
            if (itr->second > 0)
                cout << "+";
        }

        cout << itr->second;

        if (itr->first > 0)
            cout << "x^" << itr->first;
    }
    cout << endl;
}

bool PolynomialMap::ReadFromFile(const string& file)
{
    polynomial.clear();

    ifstream inp;
    inp.open(file.c_str());
    if (!inp.is_open()) {
        cout << "ERROR::PolynomialList::ReadFromFile:" << endl
            << "\t" << "file [" << file << "] opens failed" << endl;
        return false;
    }

    char ch;
    int n;
    inp >> ch;
    inp >> n;
    for (int i = 0; i < n; i++) {
        int deg;
        double cof;
        inp >> deg;
        inp >> cof;
        coefficent(deg) = cof;
    }

    inp.close();

    return true;
}
