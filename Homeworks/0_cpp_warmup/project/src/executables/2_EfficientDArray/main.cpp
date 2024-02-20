#include "EDArray.h"
#include <iostream>

int main(int argc, char** argv)
{
    EDArray a;
    std::cout << a << std::endl;

    a.PushBack(3.0);
    a.PushBack(3.1);
    a.PushBack(3.2);
    std::cout << a << std::endl;

    a.DeleteAt(0);
    std::cout << a << std::endl;
    a.InsertAt(0, 4.1);
    std::cout << a << std::endl;

    EDArray acopy = a;
    std::cout << acopy << std::endl;

    EDArray acopy2(a);
    std::cout << acopy2 << std::endl;

    EDArray acopy3, acopy4;
    acopy4 = acopy3 = a;
    std::cout << acopy3 << std::endl;
    std::cout << acopy4 << std::endl;

    EDArray b;
    b.PushBack(21);
    std::cout << b << std::endl;
    b.DeleteAt(0);
    std::cout << b << std::endl;
    b.PushBack(22);
    b.SetSize(5);
    std::cout << b << std::endl;

    EDArray c;
    c.PushBack('a');
    c.PushBack('b');
    c.PushBack('c');
    c.InsertAt(0, 'd');
    std::cout << c << std::endl;

    return 0;
}
