/******************************************************************************
 * @file lib_array.hpp
 * @brief [Set and Classes lab 2. Class for arrays]
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

// Class for array
class Set
{
private:
    static int cnt;
    char *data;
    int size;
    char tag;

public:
    Set() : size(0), tag('A' + cnt++), data(new char[maxsize + 1]) { data[0] = 0; }

    Set(char) : tag('A' + cnt++), size(0), data(new char[maxsize + 1])
    {
        for (int i = 0; i < maxsize; ++i)
        {
            if (rand() % 2)
            {
                data[size++] = universe[i];
            }
        }
        data[size] = 0;
    }

    ~Set()
    {
        delete[] data;
    }

    int len() { return size; }

    void printSet()
    {
        std::cout << tag << ": [";

        for (int i = 0; i < size; i++)
        {
            if (i == size - 1)
            {
                std::cout << data[i] << "]" << std::endl;
            }
            else
            {
                std::cout << data[i] << ", ";
            }
        }
    }

    Set &operator&=(const Set &other)
    {

        Set res(*this);
        size = 0;
        for (int i = 0; i < res.size; ++i)
        {
            for (int j = 0; j < other.size; ++j)
            {
                if (res.data[i] == other.data[j])
                {
                    data[size++] = res.data[i];
                    break;
                }
            }
        }
        data[size] = 0;
        return *this;
    }

    Set &operator|=(const Set &other)
    {
        bool flag;

        for (int i = 0; i < other.size; ++i)
        {
            flag = true;
            for (int j = 0; j < size; ++j)
            {
                if (other.data[i] == data[j])
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                data[size++] = other.data[i];
            }
        }
        data[size] = 0;
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

    Set(const Set &other) : tag('A' + cnt++), size(other.size), data(new char[maxsize + 1])
    {
        char *dst(data), *src(other.data);
        while ((*dst++ = *src++) != '\0')
            ;
    }

    Set &operator=(Set &&other)
    {
        if (this != &other)
        {
            size = other.size;
            data = other.data;
            other.data = nullptr;
        }

        return *this;
    }
};