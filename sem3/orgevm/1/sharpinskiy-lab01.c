#include <stdio.h>

const int sizeOfInt = sizeof(int) * 8;

/* io */

void printBinaryInteger(int n) {
    for (int i = sizeOfInt - 1; i >= 0; i--) {
        int bit = (n >> i) & 1; 
        printf("%d", bit);
        if ((i % 8) == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

void printLine() {
    printf("+");
    for (int i = 1; i <= sizeOfInt; i++) {
        printf("---");
        if (i != sizeOfInt) {
            printf("+");
        }
    }
    printf("+\n");
}

void beautifulPrint(int n) {

    printLine();

    printf("|");
    for (int i = 1; i <= sizeOfInt; i++) {
        printf("%2d |", i);
    }
    printf("\n");

    printLine();

    printf("|");
    for (int i = sizeOfInt - 1; i >= 0; i--) {
        int bit = (n >> i) & 1;
        printf("%2d |", bit);
    }
    printf("\n");
    
    printLine();

    printf("|");
    for (int i = sizeOfInt; i >= 1; i--) {
        printf("%2d |", i);
    }
    printf("\n");
    
    printLine();
}

/* logic */

int getBitGroup(int number, int pos, int len) {
    int result = number >> (sizeOfInt - pos - len + 1);
    int mask = (1 << len) - 1;
    result &= mask;
    return result;
}

int setBitGroup(int number, int pos, int len, int value) {
    int delta = (sizeOfInt - pos - len + 1);
    int mask = ((1 << len) - 1) << delta;
    number &= ~mask;
    number |= (value << delta);
    return number;
}

int swapBitGroups(int number) {
    int pos1, len, pos2;

    printf("Enter the position of the fisrt bit of the first group (from left to right): ");
    scanf("%d", &pos1);
    printf("Enter the position of the first bit of the second group (from left to right): ");
    scanf("%d", &pos2);
    printf("Enter the number of bits in the groups: ");
    scanf("%d", &len);

    if (pos1 < 0 || pos1 >= sizeOfInt || len <= 0 || (pos1 + len) > sizeOfInt) {
        printf("Error: invalid parameters for the first group.\n");
        return number;
    }

    if (pos2 < 0 || pos2 >= sizeOfInt || len <= 0 || (pos2 + len) > sizeOfInt) {
        printf("Error: invalid parameters for the second group.\n");
        return number;
    }

    if ((pos2 + len > sizeOfInt) || (pos1 + len > sizeOfInt)) {
        printf("Error: groups are too long to rearrange.\n");
        return number;
    }

    int group1 = getBitGroup(number, pos1, len);
    printf("Your group 1:\n");
    printBinaryInteger(group1);
    int group2 = getBitGroup(number, pos2, len);
    printf("Your group 2:\n");
    printBinaryInteger(group2);

    number = setBitGroup(number, pos1, len, group2);
    number = setBitGroup(number, pos2, len, group1);

    return number;
}

int main() {
    int number;
    printf("Enter integer: ");
    scanf("%d", &number);
    getchar();
    printf("Your number:\n");
    printBinaryInteger(number);
    beautifulPrint(number);
    number = swapBitGroups(number);
    printf("Your number:\n");
    printBinaryInteger(number);
    beautifulPrint(number);
    printf("In decimal format: %d", number);

    return 0;
}
