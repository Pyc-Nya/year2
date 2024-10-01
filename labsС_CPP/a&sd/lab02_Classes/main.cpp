#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "lib.hpp"

#define maxsize 13
template <typename T>
int Set<T>::size_un = maxsize;

template <typename T>
int Set<T>::cnt = 0;
const long q0 = 100000;

int main()
{
    srand(time(nullptr));
    Set<char> A('A'), B('B'), C('C'), D('D'), E;

    clock_t begin = clock();

    for (int i = 0; i < q0; ++i)
    {
        E = (A & B & C) | D;
    }

    clock_t end = clock();

    E.print();

    std::cout << "Time: " << (double)(end - begin) / CLOCKS_PER_SEC << "s\n";
    return 0;
}