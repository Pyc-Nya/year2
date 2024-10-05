/******************************************************************************
 * @file lib_struct.hpp
 * @brief [Set and Classes lab 2. Class for structures]
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

class Set
{
private:
    static int cnt;
    char tag;

    struct Node
    {
        char data;
        Node *next;
    };

    Node *head;

public:
    Set() : tag('A' + cnt++), head(nullptr) {}

    Set(char) : head(nullptr), tag('A' + cnt++)
    {
        for (int i = 0; i < maxsize; ++i)
        {
            if (rand() % 2)
            {
                Node *newNode = new Node();
                newNode->data = universe[i];
                newNode->next = head;
                head = newNode;
            }
        }
    }

    ~Set()
    {
        while (head != nullptr)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
    }

    int len()
    {
        int len = 0;
        Node *temp = head;
        while (temp != nullptr)
        {
            len++;
            temp = temp->next;
        }
        return len;
    }

    void printSet()
    {
        Node *temp = head;

        std::cout << tag << ": [";
        while (temp != nullptr)
        {
            if (temp->next == nullptr)
            {
                std::cout << temp->data << "]" << std::endl;
            }
            else
            {
                std::cout << temp->data << ", ";
            }
            temp = temp->next;
        }
    }

    bool contains(char el) const
    {
        Node *temp = head;
        while (temp != nullptr)
        {
            if (temp->data == el)
            {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    void addElement(char el)
    {
        if (!contains(el))
        {
            Node *newNode = new Node();
            newNode->data = el;
            newNode->next = head;
            head = newNode;
        }
    }

    Set &operator&=(const Set &other)
    {
        Node *temp = head;
        while (temp != nullptr)
        {
            Node *nextTemp = temp->next;
            if (!other.contains(temp->data))
            {
                if (temp == head)
                {
                    head = temp->next;
                }
                else
                {
                    Node *prev = head;
                    while (prev->next != temp)
                    {
                        prev = prev->next;
                    }
                    prev->next = temp->next;
                }
                delete temp;
            }
            temp = nextTemp;
        }
        return *this;
    }

    Set &operator|=(const Set &other)
    {
        Node *temp = other.head;
        while (temp != nullptr)
        {
            addElement(temp->data);
            temp = temp->next;
        }
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

    Set(const Set &other) : tag('A' + cnt++), head(nullptr)
    {
        Node *temp = other.head;
        while (temp != nullptr)
        {
            addElement(temp->data);
            temp = temp->next;
        }
    }

    Set &operator=(const Set &other)
    {
        if (this != &other)
        {
            head = nullptr;
            Node *temp = other.head;
            while (temp != nullptr)
            {
                addElement(temp->data);
                temp = temp->next;
            }
        }
        return *this;
    }

    Set(Set &&other) : tag('A' + cnt++), head(other.head)
    {
        other.head = nullptr;
    }

    Set &operator=(Set &&other)
    {
        if (this != &other)
        {
            head = other.head;
            other.head = nullptr;
        }
        return *this;
    }
};
