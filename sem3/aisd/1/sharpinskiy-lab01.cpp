#include <iostream>
#include <stdlib.h>
#include <time.h>

const int cycleRepeat = 100000000;

const int MAX_DIGITS = 10;

void fillSetByStr(const char A[], int sizeA, bool bitArray[]) {
    for (int i = 0; i < sizeA; i++) {
        bitArray[A[i] - '0'] = true;
    }
}

void outputSet(const char name, const bool bitArray[]) {
    std::cout << "Set " << name << ": [";
    bool first = true;
    for (int i = 0; i < MAX_DIGITS; ++i) {
        if (bitArray[i]) {
            if (!first) std::cout << ", ";
            std::cout << i;
            first = false;
        }
    }
    std::cout << "]" << std::endl;
}

void processSets(const bool a[], const bool b[], const bool c[], const bool d[], bool result[]) {
    for (int i = 0; i < MAX_DIGITS; ++i) {
        result[i] = (a[i] && b[i]) && !(c[i] || d[i]);
    }
}





int cmp(const void *a, const void *b) {
    return *(char*)a - *(char*)b;
}

void randomStrFill(char s[], int& size) {
    size = rand() % 10; 
    for (int i = 0; i < size; i++) {
        int r = rand() % 10;
        s[i] = '0' + r;
    }
    qsort(s, size, sizeof(char), cmp);
    int k = 0;
    for (int i = 0; i < size; i++) {
        if (s[i] != s[k]) {
            s[++k] = s[i];
        }
    }
    size = (k != 0) ? k + 1 : 0;
}

bool existsInArray(const char arr[], int size, char value) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == value) {
            return true;
        }
    }
    return false;
}

void outputArray(const char name, const char arr[], int size) {
    std::cout << "Array " << name << ": [";
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i];
        if (i < size - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void inputArray(char name, char arr[], int& size) {
    char A[80] = {0};  
    std::cout << "Input array " << name << " of decimal digits: ";
    std::cin >> A;

    int j = 0;
    for (int i = 0; i < 80 && A[i] != '\0'; ++i) {
        if ('0' <= A[i] && A[i] <= '9') {
            arr[j++] = A[i];
        }
    }

    std::qsort(arr, j, sizeof(char), cmp);

    int k = 0;
    for (int i = 0; i < j; ++i) {
        if (i == 0 || arr[i] != arr[i - 1]) {
            arr[k++] = arr[i];
        }
    }

    size = k; 
}

struct Node {
    char data;
    Node* next;
};

void append(Node*& head, char value) {
    Node* newNode = new Node;
    newNode->data = value;
    newNode->next = nullptr;
    
    if (head == nullptr) {
        head = newNode;
    } else {
        Node* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

Node* createListFromArray(const char arr[], int size) {
    Node* head = nullptr;
    for (int i = 0; i < size; ++i) {
        append(head, arr[i]);
    }
    return head;
}

void outputList(const char name, Node* head) {
    std::cout << "List " << name << ": [";
    Node* temp = head;
    while (temp != nullptr) {
        std::cout << temp->data;
        if (temp->next != nullptr) {
            std::cout << ", ";
        }
        temp = temp->next;
    }
    std::cout << "]" << std::endl;
}

bool existsInList(Node* head, char value) {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void freeList(Node*& head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}






typedef short int Word;

// bcz power of set = 10 => we need more then 10 bits

Word fillWordByStr(const char A[], int sizeA) {
    Word word = 0;
    for (int i = 0; i < sizeA; ++i) {
        if ('0' <= A[i] && A[i] <= '9') {
            int digit = A[i] - '0';
            word |= (1 << digit);
        }
    }
    return word;
}

void outputWord(const char name, Word word) {
    std::cout << "Array " << name << ": [";
    bool first = true;
    for (int i = 0; i <= 9; ++i) {
        if (word & (1 << i)) {
            if (!first) std::cout << ", ";
            std::cout << i;
            first = false;
        }
    }
    std::cout << "]" << std::endl;
}

Word processWords(Word a, Word b, Word c, Word d) {
    Word result = (a & b) & ~(c | d); // E = (A && B) \ (C || D)
    return result;
}

int main() {
    srand(time(0));
    char ask;
    Word aWord = {0}, bWord = {0}, cWord = {0}, dWord = {0};
    char aChar[80], bChar[80], cChar[80], dChar[80], eChar[10];
    bool aBool[MAX_DIGITS] = {0}, bBool[MAX_DIGITS] = {0}, cBool[MAX_DIGITS] = {0}, dBool[MAX_DIGITS] = {0}, eBool[MAX_DIGITS] = {0};
    int sizeA = 0, sizeB = 0, sizeC = 0, sizeD = 0, eSize = 0;

    std::cout << "Random or manual input (R/M)? ";
    std::cin >> ask;

    if (ask == 'R' || ask == 'r') {
        randomStrFill(aChar, sizeA);
        randomStrFill(bChar, sizeB);
        randomStrFill(cChar, sizeC);
        randomStrFill(dChar, sizeD);
    } else {
        inputArray('A', aChar, sizeA);
        inputArray('B', bChar, sizeB);
        inputArray('C', cChar, sizeC);
        inputArray('D', dChar, sizeD);
    }
    std::cout << std::endl;

    aWord = fillWordByStr(aChar, sizeA);
    bWord = fillWordByStr(bChar, sizeB);
    cWord = fillWordByStr(cChar, sizeC);
    dWord = fillWordByStr(dChar, sizeD);

    fillSetByStr(aChar, sizeA, aBool);
    fillSetByStr(bChar, sizeB, bBool);
    fillSetByStr(cChar, sizeC, cBool);
    fillSetByStr(dChar, sizeD, dBool);

    std::cout << "Result preview:" << std::endl;
    for (int i = 0; i < sizeA; ++i) {
        if (existsInArray(bChar, sizeB, aChar[i]) && !existsInArray(cChar, sizeC, aChar[i]) && !existsInArray(dChar, sizeD, aChar[i])) {
            eChar[eSize++] = aChar[i];
        }
    }
    outputArray('E', eChar, eSize);

    clock_t startArray = clock();

    for (int repeat = 0; repeat < cycleRepeat; ++repeat) {
        eSize = 0; 
        for (int i = 0; i < sizeA; ++i) {
            if (existsInArray(bChar, sizeB, aChar[i]) && !existsInArray(cChar, sizeC, aChar[i]) && !existsInArray(dChar, sizeD, aChar[i])) {
                eChar[eSize++] = aChar[i];
            }
        }
    }

    clock_t endArray = clock();
    double arrayDuration = double(endArray - startArray) / CLOCKS_PER_SEC;

    std::cout << "Char[]" << std::endl;
    outputArray('A', aChar, sizeA);
    outputArray('B', bChar, sizeB);
    outputArray('C', cChar, sizeC);
    outputArray('D', dChar, sizeD);
    std::cout << "Result is array E: E = (A && B) \\ (C || D)" << std::endl;
    outputArray('E', eChar, eSize);
    std::cout << "Time to process arrays 100000000 times: " << arrayDuration << " seconds." << std::endl;
    std::cout << std::endl;

    Node* listA = createListFromArray(aChar, sizeA);
    Node* listB = createListFromArray(bChar, sizeB);
    Node* listC = createListFromArray(cChar, sizeC);
    Node* listD = createListFromArray(dChar, sizeD);
    Node* listE = nullptr;

    clock_t startList = clock();

    for (int repeat = 0; repeat < cycleRepeat - 1; ++repeat) {
        listE = nullptr;
        for (Node* tempA = listA; tempA != nullptr; tempA = tempA->next) {
            if (existsInList(listB, tempA->data) && !existsInList(listC, tempA->data) && !existsInList(listD, tempA->data)) {
                append(listE, tempA->data);
            }
        }
        freeList(listE);
    }
    listE = nullptr;
    for (Node* tempA = listA; tempA != nullptr; tempA = tempA->next) {
        if (existsInList(listB, tempA->data) && !existsInList(listC, tempA->data) && !existsInList(listD, tempA->data)) {
            append(listE, tempA->data);
        }
    }

    clock_t endList = clock();
    double listDuration = double(endList - startList) / CLOCKS_PER_SEC;

    std::cout << "Lists" << std::endl;
    outputList('A', listA);
    outputList('B', listB);
    outputList('C', listC);
    outputList('D', listD);
    std::cout << "Result is list E: E = (A && B) \\ (C || D)" << std::endl;
    outputList('E', listE);
    std::cout << "Time to process lists 100000000 times: " << listDuration << " seconds." << std::endl;
    std::cout << std::endl;
    
    freeList(listA);
    freeList(listB);
    freeList(listC);
    freeList(listD);
    freeList(listE);

    clock_t startWord = clock();
    
    Word eWord;
    for (int i = 0; i < cycleRepeat; ++i) {
        eWord = processWords(aWord, bWord, cWord, dWord);
    }

    clock_t endWord = clock();
    double wordDuration = double(endWord - startWord) / CLOCKS_PER_SEC;

    std::cout << "Words" << std::endl;
    outputWord('A', aWord);
    outputWord('B', bWord);
    outputWord('C', cWord);
    outputWord('D', dWord);
    std::cout << "Result is word E: E = (A && B) \\ (C || D)" << std::endl;
    outputWord('E', eWord);
    std::cout << "Time to process words 100000000 times: " << wordDuration << " seconds." << std::endl;
    std::cout << std::endl;

    clock_t startSet = clock();
    
    for (int i = 0; i < cycleRepeat; ++i) {
        processSets(aBool, bBool, cBool, dBool, eBool);
    }

    clock_t endSet = clock();
    double setDuration = double(endSet - startSet) / CLOCKS_PER_SEC;

    std::cout << "Bit vectors" << std::endl;
    outputSet('A', aBool);
    outputSet('B', bBool);
    outputSet('C', cBool);
    outputSet('D', dBool);
    std::cout << "Result is set E: E = (A && B) \\ (C || D)" << std::endl;
    outputSet('E', eBool);
    std::cout << "Time to process bit sets 100000000 times: " << setDuration << " seconds." << std::endl;

    return 0;
}
