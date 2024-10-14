/******************************************************************************
 * @file main.cpp
 * @brief [Set and Classes lab 2]
 * @details
 * Universum - C++ Operation Signs
 * A set containing operations present in A, also simultaneously in B and C, and all operations from D
 *
 * plurality:  (A ∧ B ∧ C) ∨ D
 *
 * @author [Kostenko Aleksey:Voronin Mark]
 *******************************************************************************/
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "lib_array.hpp"
//#include "lib_struct.hpp"
//#include "lib_bitset.hpp"
//#include "lib_word.hpp"

#define maxsize 13

int Set::cnt = 0;
const long q0 = 100000;
const int cycleRepeat = 10000000;

int main()
{
    srand(time(0));
    char choice;
    Set A('A'), B('B'), C('C'), D('D'), E;

    std::cout << "Sets:\n";
    A.printSet();
    B.printSet();
    C.printSet();
    D.printSet();

    std::cout << "\nResult:\n";
    E = (A & B & C) | D;
    E.printSet();

    clock_t startTime = clock();

    for (int i = 0; i < cycleRepeat; ++i)
    {
        E = (A & B & C) | D;
    }

    clock_t endTime = clock();

    double duration = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;
    std::cout << "Time for 10000000" << " operations: " << duration << " seconds." << std::endl;

    return 0;
}