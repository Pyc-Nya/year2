#include <stdio.h>

const int sizeofLong = sizeof(long long) * 8;

union Data {
    double d;
    long long l;
};

void printBinary(long long n) {
    for (int i = sizeofLong - 1; i >= 0; i--) {
        int bit = (n >> i) & 1; 
        printf("%d", bit);
        if (i == 63 || i == 52) {
            printf(" ");
        }
    }
    printf("\n");
}

int main() {
    union Data data;

    printf("Enter float number: ");
    scanf("%lf", &data.d);
    printf("Your number: %lf\n", data.d);

    printf("Original number in binary:\n");
    printBinary(data.l);

    return 0;
}
