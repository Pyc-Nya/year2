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
//#include "lib_array.hpp"
//#include "lib_struct.hpp"
#include "lib_bitset.hpp"

#define maxsize 13

int Set::cnt = 0;
const long q0 = 100000;

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

    return 0;
}

// int main()
// {
//     srand(time(nullptr));
//     printf("Sets:");
//     Set<char> A('A'), B('B'), C('C'), D('D'), E;

//     clock_t begin = clock();

//     for (int i = 0; i < q0; ++i)
//     {
//         E = (A & B & C) | D;
//     }

//     clock_t end = clock();

//     printf("\n\nResult:");

//     E.print();

//     std::cout << "Time: " << (double)(end - begin) / CLOCKS_PER_SEC << "s\n";
//     return 0;
// }