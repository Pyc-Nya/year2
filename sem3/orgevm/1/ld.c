#include <stdio.h>

#define ldBytes sizeof(long double)

union Data {
    long double number;
    unsigned char arr[ldBytes];
};

// int isLittleEndian() {
//     unsigned int x = 1;
//     return *((unsigned char*)&x) == 1;
// }
void printBinaryChar(unsigned char n) {
    int i, bit;
    for (i = 7; i >= 0; i--) {
        bit = (n >> i) & 1; 
        printf("%d", bit);
    }
}
void printBits(unsigned char arr[]) {
    int i;

    for (i = ldBytes - 1; i >= 0; i--) {
        printBinaryChar(arr[i]);
        printf(" ");
    }
    // if (isLittleEndian()) {
    //     for (i = ldBytes - 1; i >= 0; i--) {
    //         printBinaryChar(arr[i]);
    //         printf(" ");
    //     }
    // } else {
    //     for (i = 0; i < ldBytes; i++) {
    //         printBinaryChar(arr[i]);
    //         printf(" ");
    //     }
    // }
    printf("\n");
}

void swapBitGroups(union Data* data) {
    int pos1, len, pos2, i, j, bitIndex, byteIndex1, byteIndex2, bitIndex1, bitIndex2, shift;
    unsigned char group1[ldBytes] = {0}, group2[ldBytes] = {0};

    printf("Enter the position of the fisrt bit of the first group (from left to right): ");
    scanf("%d", &pos1);
    printf("Enter the position of the first bit of the second group (from left to right): ");
    scanf("%d", &pos2);
    printf("Enter the number of bits in the groups: ");
    scanf("%d", &len);

    if (pos1 < 0 || pos1 >= ldBytes * 8 || len <= 0 || (pos1 + len) > ldBytes * 8) {
        printf("Error: invalid parameters for the first group.\n");
        return;
    }

    if (pos2 < 0 || pos2 >= ldBytes * 8 || len <= 0 || (pos2 + len) > ldBytes * 8) {
        printf("Error: invalid parameters for the second group.\n");
        return;
    }

    if ((pos2 + len > ldBytes * 8) || (pos1 + len > ldBytes * 8)) {
        printf("Error: groups are too long to rearrange.\n");
        return;
    }

    // for (i = 0; i < ldBytes; i++) {
    //     for (j = 0; j < 8; j++) {
    //         bitIndex = i * 8 + j;

    //         if ((bitIndex >= pos1) && (bitIndex < (pos1 + len))) {
    //             if (data->arr[ldBytes - i - 1] & (1 << (7 - j))) {
    //                 group1[ldBytes - i - 1] |= (1 << (7 - j));
    //             }
    //         }

    //         if ((bitIndex >= pos2) && (bitIndex < (pos2 + len))) {
    //             if (data->arr[ldBytes - i - 1] & (1 << (7 - j))) {
    //                 group2[ldBytes - i - 1] |= (1 << (7 - j));
    //             }
    //         }
    //     }
    // }
    for (i = 0; i < len; i++) {
        byteIndex1 = ldBytes - (i + pos1) / 8 - 1;
        byteIndex2 = ldBytes - (i + pos2) / 8 - 1;
        bitIndex1 = 7 - ((i + pos1) % 8);
        bitIndex2 = 7 - ((i + pos2) % 8);
        if (data->arr[byteIndex1] & (1 << bitIndex1)) {
            group1[byteIndex1] |= (1 << bitIndex1);
        }
        if (data->arr[byteIndex2] & (1 << bitIndex2)) {
            group2[byteIndex2] |= (1 << bitIndex2);
        }
    }

    // for (i = 0; i < ldBytes; i++) {
    //     for (j = 0; j < 8; j++) {
    //         bitIndex = i * 8 + j;

    //         if ((bitIndex >= pos1) && (bitIndex < (pos1 + len))) {
    //             data->arr[ldBytes - i - 1] &= ~(1 << (7 - j));  
    //         }

    //         if ((bitIndex >= pos2) && (bitIndex < (pos2 + len))) {
    //             data->arr[ldBytes - i - 1] &= ~(1 << (7 - j)); 
    //         }
    //     }
    // }
    for (i = 0; i < len; i++) {
        byteIndex1 = ldBytes - (i + pos1) / 8 - 1;
        byteIndex2 = ldBytes - (i + pos2) / 8 - 1;
        bitIndex1 = 7 - ((i + pos1) % 8);
        bitIndex2 = 7 - ((i + pos2) % 8);
        data->arr[byteIndex1] &= ~(1 << bitIndex1);
        data->arr[byteIndex2] &= ~(1 << bitIndex2);
    }

    printf("group1: \n");
    printBits(group1);
    printf("group2: \n");
    printBits(group2);

    for (i = 0; i < len; i++) {
        byteIndex1 = ldBytes - (i + pos1) / 8 - 1;
        byteIndex2 = ldBytes - (i + pos2) / 8 - 1;
        bitIndex1 = ((i + pos1) % 8);
        bitIndex2 = ((i + pos2) % 8);
        shift = bitIndex1 - bitIndex2;
        if (shift < 0) {
            data->arr[byteIndex1] |= group2[byteIndex2] << (-shift);
            data->arr[byteIndex2] |= group1[byteIndex1] >> (-shift);
        } else {
            data->arr[byteIndex1] |= group2[byteIndex2] >> (shift);
            data->arr[byteIndex2] |= group1[byteIndex1] << (shift);
        }
    }
}

int main() {
    union Data data;

    printf("Enter long double number: ");
    scanf("%Lf", &data.number);
    printf("Your number: %Lf\n", data.number);

    printf("Original number in binary:\n");
    printBits(data.arr);

    swapBitGroups(&data);

    printf("After swapping:\n");
    printBits(data.arr);
    printf("Your number: %Lf\n", data.number);

    return 0;
}
