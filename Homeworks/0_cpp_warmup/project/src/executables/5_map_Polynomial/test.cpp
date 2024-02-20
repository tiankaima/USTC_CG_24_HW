#include "TestPolynomial.h"
#include <PolynomialList.h>
#include <PolynomialMap.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

void generateTestCase(int size, std::vector<int>& degree, std::vector<double>& coefficent)
{
    degree.clear();
    coefficent.clear();
    for (int i = 0; i < size; i++) {
        int c = rand() % 10000;
        double val = (double)(rand() % 100);
        degree.push_back(c);
        coefficent.push_back(val);
    }
}

int main(int argc, char** argv)
{
    srand(unsigned(time(0)));
    TestPolynomial<PolynomialList> testList;
    TestPolynomial<PolynomialMap> testMap;

    for (const auto& size : { 5, 100, 150 }) {
        std::vector<int> deg0, deg1;
        std::vector<double> cof0, cof1;
        generateTestCase(size, deg0, cof0);
        generateTestCase(size, deg1, cof1);
        std::cout << "Test List:" << std::endl;
        testList.testOperationFromGivenData(deg0, cof0, deg1, cof1, size == 5);
        std::cout << "Test Map: " << std::endl;
        testMap.testOperationFromGivenData(deg0, cof0, deg1, cof1, size == 5);
    }
    return 0;
}
