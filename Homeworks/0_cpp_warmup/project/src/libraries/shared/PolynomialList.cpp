#include "PolynomialList.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#define EPSILON 1.0e-10

PolynomialList::PolynomialList(const PolynomialList& other)
{
    this->polynomial = other.polynomial;
}

PolynomialList::PolynomialList(const std::string& file)
{
    std::ifstream in(file);
    if (!in.is_open()) {
        throw std::runtime_error("File not found");
    }

    char P;
    int n;
    in >> P >> n;

    for (int i = 0; i < n; i++) {
        PolynomialList::Term tmp;
        in >> tmp.degree >> tmp.coefficent;
        this->AddOneTerm(tmp);
    }

    in.close();
}

PolynomialList::PolynomialList(const double* coefficent, const int* degree, int n)
{
    for (int i = 0; i < n; i++) {
        this->AddOneTerm(PolynomialList::Term(degree[i], coefficent[i]));
    }
}

PolynomialList::PolynomialList(const std::vector<double>& coefficent, const std::vector<int>& degree)
{
    if (coefficent.size() != degree.size()) {
        throw std::runtime_error("Size of coefficent and degree are not equal");
    }

    for (size_t i = 0; i < coefficent.size(); i++) {
        this->AddOneTerm(PolynomialList::Term(degree[i], coefficent[i]));
    }
}

double PolynomialList::coefficent(int i) const
{
    for (const Term& term : this->polynomial) {
        if (term.degree < i) {
            break;
        }
        if (term.degree == i) {
            return term.coefficent;
        }
    }

    return 0;
}

double& PolynomialList::coefficent(int i)
{
    return this->AddOneTerm(PolynomialList::Term(i, 0)).coefficent;
}

void PolynomialList::compress()
{
    // for (const auto& term : this->polynomial) {
    //     if (std::fabs(term.coefficent) < EPSILON) {
    //         this->polynomial.remove(term);
    //     }
    // }

    // remove is called on that function and would result in seg fault,

    // try another way:
    auto itr = this->polynomial.begin();
    while (itr != this->polynomial.end()) {
        if (std::fabs((*itr).coefficent) < EPSILON) {
            itr = this->polynomial.erase(itr);
        } else {
            itr++;
        }
    }
}

PolynomialList PolynomialList::operator+(const PolynomialList& right) const
{
    PolynomialList result(*this);
    for (const auto& term : right.polynomial) {
        result.AddOneTerm(term);
    }

    result.compress();
    return result;
}

PolynomialList PolynomialList::operator-(const PolynomialList& right) const
{
    PolynomialList result(*this);
    for (const auto& term : right.polynomial) {
        result.AddOneTerm(PolynomialList::Term(term.degree, -term.coefficent));
    }

    result.compress();
    return result;
}

PolynomialList PolynomialList::operator*(const PolynomialList& right) const
{
    PolynomialList result;

    for (const auto& term1 : this->polynomial) {
        for (const auto& term2 : right.polynomial) {
            result.AddOneTerm(PolynomialList::Term(term1.degree + term2.degree, term1.coefficent * term2.coefficent));
        }
    }

    result.compress();
    return result;
}

PolynomialList& PolynomialList::operator=(const PolynomialList& other)
{
    this->polynomial = other.polynomial;
    return *this;
}

auto operator<<(std::ostream& os, const PolynomialList& poly) -> std::ostream&
{
    if (poly.polynomial.empty()) {
        os << "0";
        return os;
    }

    for (const auto& term : poly.polynomial) {
        if (&term != &poly.polynomial.front()) {
            os << " ";
            if (term.coefficent > 0) {
                os << "+";
            }
        }

        os << term.coefficent;

        if (term.degree > 0) {
            os << "x^" << term.degree;
        }
    }
    return os;
}

void PolynomialList::Print() const
{
    std::cout << *this << std::endl;
}

PolynomialList::Term& PolynomialList::AddOneTerm(const Term& term)
{
    for (auto& t : this->polynomial) {
        if (t.degree == term.degree) {
            t.coefficent += term.coefficent;
            return t;
        }
    }

    this->polynomial.push_back(term);
    this->polynomial.sort([](const Term& a, const Term& b) { return a.degree > b.degree; });
    return this->polynomial.front();
}

PolynomialList::Term::Term()
    : degree(0)
    , coefficent(0)
{
}

PolynomialList::Term::Term(int degree, double coefficent)
    : degree(degree)
    , coefficent(coefficent)
{
}

bool PolynomialList::Term::operator==(const Term& other) const
{
    return this->degree == other.degree && this->coefficent == other.coefficent;
}