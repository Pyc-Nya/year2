// SetArray.h

#include <iostream>
#include <algorithm>

class Set {
private:
    char elements[10]; // Массив для десятичных цифр 0-9
    int size;

    void sortAndRemoveDuplicates() {
        std::sort(elements, elements + size);
        size = std::unique(elements, elements + size) - elements;
    }

public:
    Set() : size(0) {}

    Set(const char elems[], int n) : size(n) {
        for (int i = 0; i < n; ++i) {
            elements[i] = elems[i];
        }
        sortAndRemoveDuplicates();
    }

    void input(char name) {
        char A[80] = {0};
        std::cout << "Enter elements of set " << name << " (decimal numbers): ";
        std::cin >> A;
        size = 0;
        for (int i = 0; A[i] != '\0'; ++i) {
            if ('0' <= A[i] && A[i] <= '9') {
                elements[size++] = A[i];
            }
        }
        sortAndRemoveDuplicates();
    }

    void output(char name) const {
        std::cout << "Set " << name << ": [";
        for (int i = 0; i < size; ++i) {
            std::cout << elements[i];
            if (i < size - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }

    bool contains(char value) const {
        return std::find(elements, elements + size, value) != elements + size;
    }

    // Перегрузка операторов
    Set operator&(const Set& other) const {
        Set result;
        for (int i = 0; i < size; ++i) {
            if (other.contains(elements[i])) {
                result.elements[result.size++] = elements[i];
            }
        }
        result.sortAndRemoveDuplicates();
        return result;
    }

    Set operator|(const Set& other) const {
        Set result(*this);
        for (int i = 0; i < other.size; ++i) {
            result.elements[result.size++] = other.elements[i];
        }
        result.sortAndRemoveDuplicates();
        return result;
    }

    Set operator!() const {
        Set result;
        for (char c = '0'; c <= '9'; ++c) {
            if (!contains(c)) {
                result.elements[result.size++] = c;
            }
        }
        return result;
    }
};

