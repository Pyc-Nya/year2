/******************************************************************************
 * @file lib_bitset.hpp
 * @brief [Set and Classes lab 2. Class for bit arrays]
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

#define maxsize 13
const char universe[] = "!%&*+-=/<>^|~"; // C++ operator signs for universe set

// Class for bit array
class Set
{
private:
    static int cnt;
    bool *data;
    char tag;

public:
    Set() : tag('A' + cnt++), data(new bool[maxsize + 1])
    {
        data[0] = false;
    }

    Set(bool) : tag('A' + cnt++), data(new bool[maxsize + 1])
    {
        for (int i = 0; i < maxsize; ++i)
        {
            if (rand() % 2)
            {
                data[i] = true;
            }
            else
            {
                data[i] = false;
            }
        }
        data[maxsize] = false;
    }

    ~Set()
    {
        delete[] data;
    }

    void printSet()
    {
        std::cout << tag << ": [";
        bool isFirst = true;
        for (int i = 0; i < maxsize; ++i)
        {
            if (data[i])
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
        Set res(*this);

        for (int i = 0; i < maxsize; ++i)
        {
            res.data[i] = res.data[i] && other.data[i];
        }

        data[maxsize] = false;
        return *this;
    }

    Set &operator|=(const Set &other)
    {
        Set res(*this);

        for (int i = 0; i < maxsize; ++i)
        {
            res.data[i] = res.data[i] || other.data[i];
        }

        data[maxsize] = false;
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

    Set(const Set &other) : tag('A' + cnt++), data(new bool[maxsize + 1])
    {
        std::copy(other.data, other.data + maxsize + 1, data);
    }

    Set &operator=(Set &&other)
    {
        if (this != &other)
        {
            delete[] data;
            data = other.data;
            other.data = nullptr;
        }

        return *this;
    }
};
