/******************************************************************************
 * @file lib_word.hpp
 * @brief [Set and Classes lab 2. Class for word set]
 * @details
 * Universum - C++ Operation Signs
 * A set containing operations present in A, also simultaneously in B and C, and all operations from D
 *
 * plurality:  (A ∧ B ∧ C) ∨ D
 *
 * @author [Kostenko Aleksey:Voronin Mark]
 *******************************************************************************/
#include <iostream>
#include <cstdlib>
#include <string.h>

typedef short int word;
#define maxsize 13
const char universe[] = "!%&*+-=/<>^|~"; // C++ operator signs for universe set

// Class for word
class Set
{
private:
    static int cnt;
    word data = {0};
    char tag;

public:
    Set() : tag('A' + cnt++), data(0) {}

    Set(word) : tag('A' + cnt++), data(0)
    {

        for (int i = 0; i < maxsize; ++i)
        {
            if (rand() % 2)
            {
                data |= (1 << i);
            }
        }
    }

    ~Set() {}

    void printSet()
    {
        std::cout << tag << ": [";
        bool isFirst = true;
        for (int i = 0; i < maxsize; ++i)
        {
            if ((data >> i) & 1)
            {
                if (!isFirst)
                {
                    std::cout << ", ";
                }
                std::cout << universe[i];
                isFirst = false;
            }
        }
        std::cout << "]" << std::endl;
    }

    Set &operator&=(const Set &other)
    {
        data = data & other.data;
        return *this;
    }

    Set &operator|=(const Set &other)
    {
        data = data | other.data;
        return *this;
    }

    Set operator&(const Set &other) const
    {
        Set res(*this);
        return (res &= other);
    }

    Set operator|(const Set &other) const
    {
        Set res(*this);
        return (res |= other);
    }

    Set(const Set &other) : tag('A' + cnt++), data(other.data) {}

    Set &operator=(Set &&other)
    {
        std::cout << "move#2" << std::endl;
        if (this != &other)
        {
            data = other.data;
            other.data = 0;
        }
        return *this;
    }
};