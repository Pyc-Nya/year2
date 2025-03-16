#include <stdio.h>

const int sizeofLong = sizeof(int) * 8;

union Data {
    float f;
    int i;
};

void printBinary(int n) {
    for (int i = sizeofLong - 1; i >= 0; i--) {
        int bit = (n >> i) & 1; 
        printf("%d", bit);
        if (i == 31 || i == 23) {
            printf(" ");
        }
    }
    printf("\n");
}

int main() {
    union Data data;

    printf("Enter float number: ");
    scanf("%f", &data.f);
    printf("Your number: %f\n", data.f);

    printf("Original number in binary:\n");
    printBinary(data.i);

    return 0;
}
