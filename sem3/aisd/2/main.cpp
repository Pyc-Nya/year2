#include <iostream>
#include <ctime>
#include "SetArray.h"
// #include "SetList.h"
// #include "SetBitVector.h"
// #include "SetWord.h"

const int cycleRepeat = 10000000; 

int main() {
    Set A, B, C, D, E;

    A.input('A');
    B.input('B');
    C.input('C');
    D.input('D');

    E = (A & B) & !(C | D);
    E.output('E');

    clock_t startTime = clock();

    for (int i = 0; i < cycleRepeat; ++i) {
        E = (A & B) & !(C | D);
    }

    clock_t endTime = clock();
    double duration = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;

    std::cout << "Time for " << cycleRepeat << " operations: " << duration << " seconds." << std::endl;

    return 0;
}
