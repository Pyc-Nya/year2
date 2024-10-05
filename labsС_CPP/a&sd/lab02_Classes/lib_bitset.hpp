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

// Class for bit array type
class Set
{
private:
    static int cnt;
    bool *data;
    int size;
    char tag;

public:
    Set() : size(0), tag('A' + cnt++), data(new bool[maxsize + 1]) { data[0] = false; }

    Set(char) : tag('A' + cnt++), size(maxsize), data(new bool[maxsize + 1])
    {
        for (int i = 0; i < maxsize; ++i)
        {
            if (rand() % 2)
            {
                data[i] = true;
            }
        }
        data[size] = false;
    }

    ~Set()
    {
        delete[] data;
    }

    int len() { return size; }

    void printSet()
    {
        std::cout << tag << ": [";
        for (int i = 0; i < maxsize; ++i)
        {
            if (data[i] && i < size)
            {
                std::cout << universe[i] << ", ";
            }
            else
            {
                std::cout << universe[i] << "]";
            }
        }
    }

    Set &operator&=(const Set &other)
    {
        Set res(*this);
        
    }
};

// // Класс для структуры
// class Structure
// {
// private:
//     struct Node
//     {
//         char data;
//         Node *next;
//     };

//     Node *head;

// public:
//     Structure()
//     {
//         head = nullptr;
//     }

//     ~Structure()
//     {
//         while (head != nullptr)
//         {
//             Node *temp = head;
//             head = head->next;
//             delete temp;
//         }
//     }

//     void addElement(char data)
//     {
//         Node *newNode = new Node();
//         newNode->data = data;
//         newNode->next = head;
//         head = newNode;
//     }

//     void printStructure()
//     {
//         Node *temp = head;
//         while (temp != nullptr)
//         {
//             std::cout << temp->data << " ";
//             temp = temp->next;
//         }
//         std::cout << std::endl;
//     }
// };

// // Класс для битового массива
// class BitArray
// {
// private:
//     bool *data;
//     int size;

// public:
//     BitArray(int size)
//     {
//         this->size = size;
//         data = new bool[size];
//     }

//     ~BitArray()
//     {
//         delete[] data;
//     }

//     void fillBitArray(bool *arr)
//     {
//         for (int i = 0; i < size; i++)
//         {
//             data[i] = arr[i];
//         }
//     }

//     void printBitArray()
//     {
//         for (int i = 0; i < size; i++)
//         {
//             std::cout << data[i] << " ";
//         }
//         std::cout << std::endl;
//     }
// };

// // Класс для машинного слова
// class MachineWord
// {
// private:
//     unsigned short int data;

// public:
//     MachineWord()
//     {
//         data = 0;
//     }

//     void fillMachineWord(unsigned short int value)
//     {
//         data = value;
//     }

//     void printMachineWord()
//     {
//         std::cout << data << std::endl;
//     }
// };