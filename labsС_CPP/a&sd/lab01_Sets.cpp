/******************************************************************************
 * @brief [Set and Classes]
 * @details
 * Create and debug a program that processes sets according to instructions:
 * Universum - C++ Operation Signs
 * A set containing operations present in A, also simultaneously in B and C, and all operations from D
 *
 * plurality :  (A ∧ B ∧ C) ∨ D
 *
 *
 * @author [Kostenko Aleksey:Voronin Mark]
 *******************************************************************************/

#include <iostream> // for IO operations
#include <chrono>   // for timing
#include <iomanip>  // для std::setprecision
#include <algorithm>

#define maxsize 13
const char operators[] = {'!', '%', '&', '*', '+', '-', '=', '/', '<', '>', '^', '|', '~'}; // C++ operator signs for bit array
typedef short int word;

struct Set
{
    char el;
    Set *next;
    Set(char e, Set *n = nullptr) : el(e), next(n) {}
    ~Set() { delete next; }
};

namespace options
{
    /******************************************************************************
     * @brief The function which creates a new set
     * @param arr massive with elements
     * @returns @param set new set
     ******************************************************************************/
    Set *createSet(char *arr, int len)
    {
        Set *head = nullptr;
        for (uint64_t i = 0; i < len; i++)
        {
            head = new Set(arr[i], head);
        }
        return head;
    }

    /******************************************************************************
     * @brief The function which fills the array
     * @param arr massive
     * @returns void
     ******************************************************************************/
    uint64_t findIndex(char el)
    {
        uint64_t index = 0;
        for (uint64_t i = 0; i < maxsize; i++)
        {
            if (operators[i] == el)
            {
                index = i;
                break;
            }
        }
        return index;
    }

    /******************************************************************************
     * @brief The function which returns the length of the array
     * @param arr massive
     * @returns @param uint64_t length
     ******************************************************************************/
    uint64_t lenArr(char *arr)
    {
        uint64_t i = 0;

        for (i = 0; i < maxsize; i++)
        {
            if (arr[i] == '0')
                break;
        }
        return i;
    }

    /******************************************************************************
     * @brief The function which sorts the array
     * @param arr massive
     * @returns void
     ******************************************************************************/
    void bubbleSort(char *arr)
    {
        for (int i = 0; i < lenArr(arr) - 1; ++i)
        {
            for (int j = 0; j < lenArr(arr) - i - 1; ++j)
            {
                if (arr[j] > arr[j + 1])
                {
                    char temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }

    /******************************************************************************
     * @brief The function which fills the array
     * @param arr massive
     * @returns void
     ******************************************************************************/
    void fillarray(char *arr)
    {
        char temp;

        for (uint64_t i = 0; i < maxsize; ++i)
        {
            std::cin >> temp;
            if (temp == '0')
                break;
            arr[i] = temp;
        }

        bubbleSort(arr);
    }

    /******************************************************************************
     * @brief The function which fills the array with nulls ('0')
     * @param arr massive
     * @returns void
     ******************************************************************************/
    void fillArrNull(char *arr)
    {
        for (uint64_t i = 0; i < maxsize; i++)
            arr[i] = '0';
    }

    /******************************************************************************
     * @brief The function which checks the presence of an element in the array
     * @param arr massive
     * @param el element
     * @returns @param bool true/false
     ******************************************************************************/
    bool ArrContains(char *arr, char el)
    {
        static uint64_t len = lenArr(arr);
        for (uint64_t i = 0; i < len; i++)
        {
            if (arr[i] == el)
            {
                return true;
            }
        }
        return false;
    }

    /******************************************************************************
     * @brief The function which prints the array
     * @param arr massive
     * @returns void
     ******************************************************************************/
    void printArr(char *arr)
    {
        for (uint64_t i = 0; i < lenArr(arr); i++)
        {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;
    }

    /******************************************************************************
     * @brief The function for finding the intersection of two sets
     * @param A set
     * @param B set
     * @param C set
     * @param result result set
     * @returns void
     ******************************************************************************/
    void intersectArr(char *A, char *B, char *C, char *result)
    {
        static uint64_t lenA = lenArr(A);
        static uint64_t lenB = lenArr(B);
        static uint64_t lenC = lenArr(C);
        static uint64_t lenR = lenArr(result);

        for (uint64_t i = 0; i < lenA; i++)
        {
            for (uint64_t j = 0; j < lenB; j++)
            {
                for (uint64_t k = 0; k < lenC; k++)
                {
                    if ((A[i] == B[j]) && (C[k] == B[j]) && (A[i] != '0'))
                    {
                        result[lenR] = A[i];
                        lenR++;
                    }
                }
            }
        }
    }

    /******************************************************************************
     * @brief The function for finding the union of two sets
     * @param A set
     * @param B set
     * @returns void
     ******************************************************************************/
    void unionArr(char *A, char *result)
    {
        static uint64_t lenA = lenArr(A);
        static uint64_t lenR = lenArr(result);

        for (uint64_t i = 0; i < lenA; i++)
        {
            if (!ArrContains(result, A[i]))
            {
                result[lenR] = A[i];
                lenR++;
            }
        }
    }

    /******************************************************************************
     * @brief The function which implements the operation (A ∧ B ∧ C) ∨ D
     * @param A set
     * @param B set
     * @param C set
     * @param D set
     * @param result reult set
     * @returns void
     ******************************************************************************/
    void processArr(char *A, char *B, char *C, char *D, char *result)
    {
        intersectArr(A, B, C, result); // result = (A ∧ B ∧ C)
        unionArr(D, result);           // result = (A ∧ B ∧ C) ∨ D
    }

    /******************************************************************************
     * @brief The function which calculates the length of the set
     * @param set
     * @returns @param int
     ******************************************************************************/
    int setLen(Set *set)
    {
        int count = 0;
        for (Set *i = set; i != nullptr; i = i->next)
        {
            count++;
        }
        return count;
    }

    /******************************************************************************
     * @brief The function for checking the presence of an element in sets
     * @param A set
     * @param el element
     * @returns @param bool true/false
     ******************************************************************************/
    bool SetContains(Set *A, char el)
    {
        for (Set *i = A; i != nullptr; i = i->next)
        {
            if (i->el == el)
            {
                return true;
            }
        }
        return false;
    }

    /******************************************************************************
     * @brief The function for finding the intersection of two sets
     * @param A set
     * @param B set
     * @returns @param result new set
     ******************************************************************************/
    Set *intersectSets(Set *A, Set *B)
    {
        Set *result = nullptr;
        for (Set *i = A; i != nullptr; i = i->next)
        {
            for (Set *j = B; j != nullptr; j = j->next)
            {
                if (i->el == j->el)
                {
                    if (!SetContains(result, i->el))
                    {
                        result = new Set(i->el, result);
                    }
                }
            }
        }
        return result;
    }

    /******************************************************************************
     * @brief The function for finding the union of two sets
     * @param A set
     * @param B set
     * @returns @param result new set
     ******************************************************************************/
    Set *unionSets(Set *A, Set *B)
    {
        Set *result = A;
        for (Set *i = B; i != nullptr; i = i->next)
        {
            if (!SetContains(result, i->el))
            {
                Set *newSet = new Set(i->el);
                newSet->next = result;
                result = newSet;
            }
        }
        return result;
    }

    /******************************************************************************
     * @brief The function which implements the operation (A ∧ B ∧ C) ∨ D
     * @param A set
     * @param B set
     * @param C set
     * @param D set
     * @returns @param result new set
     ******************************************************************************/
    Set *processList(Set *A, Set *B, Set *C, Set *D)
    {
        // Find the intersection of A and B
        Set *intersectionAB = intersectSets(A, B);

        // Find the intersection of intersectionAB and C
        Set *intersectionABC = intersectSets(intersectionAB, C);

        // Find the union of the intersection and D
        Set *result = unionSets(intersectionABC, D);

        return result;
    }

    /******************************************************************************
     * @brief The function which print a set
     * @param set set
     * @returns void
     ******************************************************************************/
    void printSet(Set *set)
    {
        for (Set *i = set; i != nullptr; i = i->next)
        {
            std::cout << i->el << " ";
        }
        std::cout << std::endl;
    }

    /******************************************************************************
     * @brief The function which frees the list
     * @param head list
     * @returns void
     ******************************************************************************/
    void freeList(Set *&head)
    {
        while (head != nullptr)
        {
            Set *temp = head;
            head = head->next;
            delete temp;
        }
    }

    /******************************************************************************
     * @brief The function which fills the bitset
     * @param arr massive
     * @param bitArr bitset
     * @returns void
     ******************************************************************************/
    void fillBitSet(char *arr, bool *bitArr)
    {
        for (uint64_t i = 0; i < lenArr(arr); i++)
        {
            bitArr[findIndex(arr[i])] = true;
        }
    }

    /******************************************************************************
     * @brief The function which processes the bitsets
     * @param bA massive
     * @param bB massive
     * @param bC massive
     * @param bD massive
     * @param bRes massive
     * @returns void
     ******************************************************************************/
    void processBitSet(bool *bA, bool *bB, bool *bC, bool *bD, bool *bRes)
    {
        for (uint64_t i = 0; i < maxsize; ++i)
            bRes[i] = ((bA[i] && bB[i]) && bC[i]) || bD[i];
    }

    /******************************************************************************
     * @brief The function which prints the bitset
     * @param bitSet massive
     * @returns void
     ******************************************************************************/
    void printBitSet(bool *bitSet)
    {
        for (uint64_t i = 0; i < maxsize; ++i)
        {
            if (bitSet[i])
            {
                std::cout << operators[i] << " ";
            }
        }
        std::cout << std::endl;
    }

    /******************************************************************************
     * @brief The function which fills the wordset
     * @param arr massive
     * @param wA wordset
     * @returns void
     ******************************************************************************/
    void fillWordSet(char *arr, word *wA)
    {
        for (uint64_t i = 0; i < lenArr(arr); i++)
        {
            uint64_t digit = findIndex(arr[i]);
            *wA |= (1 << digit);
        }
    }

    /******************************************************************************
     * @brief The function which processes the wordset
     * @param wA word
     * @param wB word
     * @param wC word
     * @param wD word
     * @param wRes res word
     * @returns void
     ******************************************************************************/
    void processWordSet(word *wA, word *wB, word *wC, word *wD, word *wRes)
    {
        *wRes = (*wA & *wB & *wC) | *wD;
    }

    /******************************************************************************
     * @brief The function which prints the wordset
     * @param word word
     * @returns void
     ******************************************************************************/
    void printWordSet(word *word)
    {
        for (int i = 0; i < maxsize; ++i)
        {
            if ((*word >> i) & 1)
            {
                std::cout << operators[i] << " ";
            }
        }
        std::cout << std::endl;
    }

    /******************************************************************************
     * @brief The function which fills the array with random operators
     * @param arr massive
     * @returns void
     ******************************************************************************/
    void randomFill(char *arr)
    {
        uint64_t random_size = 0, random_index = 0;

        random_size = rand() % maxsize + 1;
        while (lenArr(arr) < random_size)
        {
            random_index = rand() % maxsize;
            if (!ArrContains(arr, operators[random_index]))
            {
                arr[lenArr(arr)] = operators[random_index];
            }
        }
    }

} // namespace options

int main()
{
    srand(time(0));
    char A[maxsize], B[maxsize], C[maxsize], D[maxsize], resArr[maxsize];
    bool bA[maxsize] = {0}, bB[maxsize] = {0}, bC[maxsize] = {0}, bD[maxsize] = {0}, bRes[maxsize] = {0};
    word wA = {0}, wB = {0}, wC = {0}, wD = {0}, wRes = {0};
    char choice;

    std::cout << "Random or manual input (R/M)? ";
    std::cin >> choice;
    std::cout << std::endl;

    options::fillArrNull(A);
    options::fillArrNull(B);
    options::fillArrNull(C);
    options::fillArrNull(D);
    options::fillArrNull(resArr);

    if (choice == 'R' || choice == 'r')
    {
        options::randomFill(A);
        options::randomFill(B);
        options::randomFill(C);
        options::randomFill(D);
    }
    else
    {
        std::cout << "Enter data for set A (0 to complete entry):" << std::endl;
        options::fillarray(A);
        std::cout << "Enter data for set B (0 to complete entry):" << std::endl;
        options::fillarray(B);
        std::cout << "Enter data for set C (0 to complete entry):" << std::endl;
        options::fillarray(C);
        std::cout << "Enter data for set D (0 to complete entry):" << std::endl;
        options::fillarray(D);
        std::cout << std::endl;
    }

    // Create sets
    Set *setA = options::createSet(A, options::lenArr(A));
    Set *setB = options::createSet(B, options::lenArr(B));
    Set *setC = options::createSet(C, options::lenArr(C));
    Set *setD = options::createSet(D, options::lenArr(D));

    options::fillBitSet(A, bA);
    options::fillBitSet(B, bB);
    options::fillBitSet(C, bC);
    options::fillBitSet(D, bD);

    options::fillWordSet(A, &wA);
    options::fillWordSet(B, &wB);
    options::fillWordSet(C, &wC);
    options::fillWordSet(D, &wD);

    std::cout << "Set A: ";
    options::printSet(setA);
    std::cout << "Set B: ";
    options::printSet(setB);
    std::cout << "Set C: ";
    options::printSet(setC);
    std::cout << "Set D: ";
    options::printSet(setD);
    std::cout << std::endl;

    // Perfomed operations

    // Array result
    auto start_time_arr = std::chrono::high_resolution_clock::now();

    options::processArr(A, B, C, D, resArr);

    auto end_time_arr = std::chrono::high_resolution_clock::now();
    auto duration_arr = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(end_time_arr - start_time_arr).count();
    std::cout << std::fixed << std::setprecision(7) << "Lead time array: " << duration_arr << " microseconds" << std::endl;

    // List result
    auto start_time_list = std::chrono::high_resolution_clock::now();

    Set *resultSet = options::processList(setA, setB, setC, setD);

    auto end_time_list = std::chrono::high_resolution_clock::now();
    auto duration_list = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(end_time_list - start_time_list).count();
    std::cout << std::fixed << std::setprecision(7) << "Lead time list: " << duration_list << " microseconds" << std::endl;

    // Bit result
    auto start_time_bit = std::chrono::high_resolution_clock::now();

    options::processBitSet(bA, bB, bC, bD, bRes);

    auto end_time_bit = std::chrono::high_resolution_clock::now();
    auto duration_bit = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(end_time_bit - start_time_bit).count();
    std::cout << std::fixed << std::setprecision(7) << "Lead time bitset: " << duration_bit << " microseconds" << std::endl;

    // Word result
    auto start_time_word = std::chrono::high_resolution_clock::now();

    options::processWordSet(&wA, &wB, &wC, &wD, &wRes);

    auto end_time_word = std::chrono::high_resolution_clock::now();
    auto duration_word = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(end_time_word - start_time_word).count();
    std::cout << std::fixed << std::setprecision(7) << "Lead time wordset: " << duration_word << " microseconds" << std::endl;
    std::cout << std::endl;

    std::cout << "Array result: ";
    options::printArr(resArr);
    std::cout << "Set result: ";
    options::printSet(resultSet);
    std::cout << "Bit result: ";
    options::printBitSet(bRes);
    std::cout << "Word result: ";
    options::printWordSet(&wRes);

    return 0;
}