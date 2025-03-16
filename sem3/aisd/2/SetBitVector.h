// SetBitVector.h

#include <iostream>

class Set {
private:
    bool bits[10]; // Битовый вектор для цифр 0-9

public:
    Set() {
        for (int i = 0; i < 10; ++i) bits[i] = false;
    }

    Set(const char elems[], int n) {
        for (int i = 0; i < 10; ++i) bits[i] = false;
        for (int i = 0; i < n; ++i) {
            if ('0' <= elems[i] && elems[i] <= '9') {
                bits[elems[i] - '0'] = true;
            }
        }
    }

    void input(char name) {
        char A[80] = {0};
        std::cout << "Enter elements of set " << name << " (decimal numbers): ";
        std::cin >> A;
        for (int i = 0; i < 10; ++i) bits[i] = false;
        for (int i = 0; A[i] != '\0'; ++i) {
            if ('0' <= A[i] && A[i] <= '9') {
                bits[A[i] - '0'] = true;
            }
        }
    }

    void output(char name) const {
        std::cout << "Set " << name << ": [";
        bool first = true;
        for (int i = 0; i < 10; ++i) {
            if (bits[i]) {
                if (!first) std::cout << ", ";
                std::cout << i;
                first = false;
            }
        }
        std::cout << "]" << std::endl;
    }

    bool contains(char value) const {
        if ('0' <= value && value <= '9') {
            return bits[value - '0'];
        }
        return false;
    }

    // Перегрузка операторов
    Set operator&(const Set& other) const {
        Set result;
        for (int i = 0; i < 10; ++i) {
            result.bits[i] = bits[i] && other.bits[i];
        }
        return result;
    }

    Set operator|(const Set& other) const {
        Set result;
        for (int i = 0; i < 10; ++i) {
            result.bits[i] = bits[i] || other.bits[i];
        }
        return result;
    }

    Set operator!() const {
        Set result;
        for (int i = 0; i < 10; ++i) {
            result.bits[i] = !bits[i];
        }
        return result;
    }
};

