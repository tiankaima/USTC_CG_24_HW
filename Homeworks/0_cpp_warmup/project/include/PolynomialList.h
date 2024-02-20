#pragma once

#if defined(_WIN32) && defined(AsShared_PolynomialList)
#ifdef Export_PolynomialList
#define DECLSPEC_PolynomialList __declspec(dllexport)
#else
#define DECLSPEC_PolynomialList __declspec(dllimport)
#endif
#else
#define DECLSPEC_PolynomialList
#endif

#include <list>
#include <string>
#include <vector>

class DECLSPEC_PolynomialList PolynomialList {
public:
    PolynomialList() {};
    PolynomialList(const PolynomialList& other);
    PolynomialList(const std::string& file);
    PolynomialList(const double* coefficent, const int* degree, int n);
    PolynomialList(const std::vector<double>& coefficent, const std::vector<int>& degree);

    double& coefficent(int i);
    double coefficent(int i) const;

    void compress();

    PolynomialList operator+(const PolynomialList& other) const;
    PolynomialList operator-(const PolynomialList& other) const;
    PolynomialList operator*(const PolynomialList& other) const;
    PolynomialList& operator=(const PolynomialList& other);

    friend auto operator<<(std::ostream& os, const PolynomialList& poly) -> std::ostream&;

    void Print() const;

private:
    struct Term {
        int degree;
        double coefficent;

        Term();
        Term(int degree, double coefficent);

        bool operator==(const Term& other) const;
    };
    Term& AddOneTerm(const Term& term);

private:
    std::list<Term> polynomial;
};
