// SetWord.h

#include <iostream>

typedef unsigned short Word;

class Set {
private:
    Word bits; // Используем биты 0-9 для цифр 0-9

public:
    Set() : bits(0) {}

    Set(const char elems[], int n) : bits(0) {
        for (int i = 0; i < n; ++i) {
            if ('0' <= elems[i] && elems[i] <= '9') {
                bits |= (1 << (elems[i] - '0'));
            }
        }
    }

    void input(char name) {
        char A[80] = {0};
        std::cout << "Enter elements of set " << name << " (decimal numbers): ";
        std::cin >> A;
        bits = 0;
        for (int i = 0; A[i] != '\0'; ++i) {
            if ('0' <= A[i] && A[i] <= '9') {
                bits |= (1 << (A[i] - '0'));
            }
        }
    }

    void output(char name) const {
        std::cout << "Set " << name << ": [";
        bool first = true;
        for (int i = 0; i <= 9; ++i) {
            if (bits & (1 << i)) {
                if (!first) std::cout << ", ";
                std::cout << i;
                first = false;
            }
        }
        std::cout << "]" << std::endl;
    }

    bool contains(char value) const {
        if ('0' <= value && value <= '9') {
            return bits & (1 << (value - '0'));
        }
        return false;
    }

    // Перегрузка операторов
    Set operator&(const Set& other) const {
        Set result;
        result.bits = bits & other.bits;
        return result;
    }

    Set operator|(const Set& other) const {
        Set result;
        result.bits = bits | other.bits;
        return result;
    }

    Set operator!() const {
        Set result;
        result.bits = ~bits & 0x3FF; // Маска для 10 бит
        return result;
    }
};

