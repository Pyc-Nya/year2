#include <iostream>
#include <cstdlib>

const char universe[] = "!%&*+-=/<>^|~"; // C++ operator signs for universe set

template <typename T>
class Set
{
private:
    static int size_un, cnt;
    int n;
    char S, *A;

public:
    Set operator|(const Set &) const;
    Set operator&(const Set &) const;
    Set operator~() const;
    void print();
    int size() { return n; }
    Set();
    Set(char);
    Set(Set &&B);
    Set &operator=(Set &&B);
    Set(const Set &);
    Set &operator=(const Set &);
    ~Set() { delete[] A; };

    Set &operator&=(const Set &B)
    {
        Set C(*this);
        n = 0;
        for (int i = 0; i < C.n; ++i)
        {
            for (int j = 0; j < B.n; j++)
            {
                if (C.A[i] == B.A[j])
                {
                    A[n++] = C.A[i];
                    break;
                }
            }
        }
        A[n] = 0;
        return *this;
    }

    Set &operator|=(const Set &B)
    {
        bool flag;

        for (int i = 0; i < B.n; ++i)
        {
            flag = true;
            for (int j = 0; j < n; j++)
            {
                if (B.A[i] == A[j])
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                A[n++] = B.A[i];
            }
        }
        A[n] = 0;
        return *this;
    }
};

template <typename T>
Set<T> Set<T>::operator&(const Set &B) const
{
    Set C(*this);
    return (C &= B);
}

template <typename T>
Set<T> Set<T>::operator|(const Set &B) const
{
    Set C(*this);
    return (C |= B);
}

template <typename T>
Set<T> Set<T>::operator~() const
{
    Set C;
    bool flag = true;

    for (int i = 0; i < size_un; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (universe[i] == A[j])
            {
                flag = false;
                break;
            }
        }
        if (flag)
        {
            C.A[C.n++] = universe[i];
        }
    }
    C.A[C.n] = 0;
    return C;
}

template <typename T>
void Set<T>::print()
{
    std::cout << "\n"
              << S << " = [" << A << "]\n";
}

template <typename T>
Set<T>::Set() : n(0), S('A' + cnt++), A(new char[size_un + 1]) { A[0] = 0; }

template <typename T>
Set<T>::Set(char) : S('A' + cnt++), n(0), A(new char[size_un + 1])
{
    for (int i = 0; i < size_un; ++i)
    {
        if (rand() % 2)
        {
            A[n++] = universe[i];
        }
    }
    A[n] = 0;
    std::cout << '\n'
              << S << " = [" << A << "]";
}

template <typename T>
Set<T>::Set(const Set &B) : S('A' + cnt++), n(B.n), A(new char[size_un + 1])
{
    char *dst(A), *src(B.A);
    while ((*dst++ = *src++) != '\0')
        ;
}

template <typename T>
Set<T> &Set<T>::operator=(const Set &B)
{
    if (this != &B)
    {
        n = B.n;
        char *dst(A), *src(B.A);
        while ((*dst++ = *src++) != '\0')
            ;
    }

    return *this;
}

template <typename T>
Set<T>::Set(Set &&B) : S('A' + cnt++), n(B.n), A(B.A)
{
    B.A = nullptr;
}

template <typename T>
Set<T> &Set<T>::operator=(Set &&B)
{
    if (this != &B)
    {
        n = B.n;
        A = B.A;
        S = 'A' + cnt++;
        B.A = nullptr;
    }

    return *this;
}