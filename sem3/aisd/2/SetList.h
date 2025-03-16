// SetList.h

#include <iostream>

class Set {
private:
    struct Node {
        char data;
        Node* next;
        Node(char d) : data(d), next(nullptr) {}
    };
    Node* head;

    void append(char value) {
        if (!head) {
            head = new Node(value);
        } else {
            Node* temp = head;
            while (temp->next && temp->data != value) {
                temp = temp->next;
            }
            if (temp->data != value && !temp->next) {
                temp->next = new Node(value);
            }
        }
    }

    void clear() {
        Node* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }

public:
    Set() : head(nullptr) {}

    Set(const char elems[], int n) : head(nullptr) {
        for (int i = 0; i < n; ++i) {
            if ('0' <= elems[i] && elems[i] <= '9') {
                append(elems[i]);
            }
        }
    }

    ~Set() {
        clear();
    }

    void input(char name) {
        char A[80] = {0};
        std::cout << "Enter elements of set " << name << " (decimal numbers): ";
        std::cin >> A;
        clear();
        for (int i = 0; A[i] != '\0'; ++i) {
            if ('0' <= A[i] && A[i] <= '9') {
                append(A[i]);
            }
        }
    }

    void output(char name) const {
        std::cout << "Set " << name << ": [";
        Node* temp = head;
        while (temp) {
            std::cout << temp->data;
            if (temp->next) std::cout << ", ";
            temp = temp->next;
        }
        std::cout << "]" << std::endl;
    }

    bool contains(char value) const {
        Node* temp = head;
        while (temp) {
            if (temp->data == value) return true;
            temp = temp->next;
        }
        return false;
    }

    // Перегрузка операторов
    Set operator&(const Set& other) const {
        Set result;
        Node* temp = head;
        while (temp) {
            if (other.contains(temp->data)) {
                result.append(temp->data);
            }
            temp = temp->next;
        }
        return result;
    }

    Set operator|(const Set& other) const {
        Set result(*this);
        Node* temp = other.head;
        while (temp) {
            if (!contains(temp->data)) {
                result.append(temp->data);
            }
            temp = temp->next;
        }
        return result;
    }

    Set operator!() const {
        Set result;
        for (char c = '0'; c <= '9'; ++c) {
            if (!contains(c)) {
                result.append(c);
            }
        }
        return result;
    }
};
