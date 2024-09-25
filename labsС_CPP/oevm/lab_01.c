#include <stdio.h>
#include <string.h>
#include <locale.h>

typedef union
{
    unsigned long long ll;
    long double ld;
} type;

typedef union
{
    long double ld;
    struct
    {
        unsigned long long mantisa : 52;
        unsigned long long exponent : 11;
        unsigned long long sign : 1;
    } parts;
} longer;

int binaryToDecimal(char *binary)
{
    int decimal = 0;
    int base = 1;
    int len = strlen(binary);

    for (int i = len - 1; i >= 0; i--)
    {
        if (binary[i] == '1')
        {
            decimal += base;
        }
        base *= 2;
    }

    return decimal;
}

void output(unsigned long long x)
{
    int i;
    for (i = 8 * sizeof(x) - 1; i > -1; i--)
        printf("%llu", x >> i & 1);
    printf("\n");
}

void processChar(char input)
{
    unsigned int bit;

    for (int i = 0; i < 8; i++)
    {
        bit = ((input >> (8 - i - 1)) & 1);
        printf("%d", bit);
    }
    printf("\n");
}

void processLD(long double input)
{
    type res;

    {
        res.ld = input;
        for (int i = 8 * sizeof(res.ll) - 1; i > -1; i--)
            printf("%llu", res.ll >> i & 1);
        printf("\n");
    }
}

void printSEM(longer output)
{
    printf("sign : ");
    for (int i = 0; i > -1; i--)
    {
        printf("%d", output.parts.sign >> i & 1);
    }
    printf(" (2) : %d (10)\n", output.parts.sign);

    printf("exponent : ");
    for (int i = 11 - 1; i > -1; i--)
    {
        printf("%d", output.parts.exponent >> i & 1);
    }
    printf(" (2) : %d (10)\n", output.parts.exponent);

    printf("mantisa : ");
    for (int i = 52 - 1; i > -1; i--)
    {
        printf("%llu", output.parts.mantisa >> i & 1);
    }
    printf(" (2) : %llu (10)\n", output.parts.mantisa);
}

void mirrorChar(unsigned int input, int groupSize, int msbPosition)
{
    int mirrored = 0;
    int mask = 0, group = 0;

    // создаем маску для извлечения группы разрядов
    mask = ((1 << groupSize) - 1) << (msbPosition - groupSize + 1);

    // извлекаем группу разрядов
    group = (input & mask) >> (msbPosition - groupSize + 1);

    // отражаем группу разрядов
    for (int i = 0; i < groupSize; i++)
    {
        mirrored = (mirrored << 1) | (group & 1);
        group >>= 1;
    }

    // сдвигаем отраженную группу в нужное положение
    mirrored = mirrored << (msbPosition - groupSize + 1);

    // очищаем биты в input перед записью отраженной группы
    input = input & ~(mask);

    // записываем отраженную группу обратно в двоичное представление символа 
    input = input | mirrored;

    printf("Результат зеркальной перестановки: \n");
    printf("(2) ");
    processChar(input);
    binaryToDecimal((char *)&input);
    printf("(10) %u\n", input);
}

void mirrorLD(long double input, int groupSize, int msbPosition)
{
    type temp;
    u_int64_t mask = 0, group = 0, highBits = 0;

    temp.ld = input;

    // создаем маску для извлечения группы разрядов
    mask = ((1 << (u_int64_t)groupSize) - 1) << ((u_int64_t)msbPosition - (u_int64_t)groupSize + 1);

    // извлекаем группу разрядов
    group = (temp.ll & mask) >> (msbPosition - groupSize + 1);

    // сохраняем старшие разряды
    highBits = temp.ll & ~(mask);

    // отражаем группу разрядов
    for (int i = 0; i < groupSize; i++)
    {
        temp.ll = (temp.ll << 1) | (group & 1);
        group >>= 1;
    }

    // объединяем старшие разряды с отраженной группой разрядов
    temp.ll = highBits | (temp.ll << (msbPosition - groupSize + 1));

    printf("Результат зеркальной перестановки: \n");
    printf("(2) ");
    output(temp.ll);
    binaryToDecimal((char *)&temp.ll);
    printf("(10) %llu\n", temp.ll);
}

int main()
{
    setlocale(LC_ALL, "rus");
    char choice;
    char inputChar;
    char temp;
    int binChar;
    int groupSize;
    int msbPosition;
    long double inputDouble;
    longer inputLD;

    do
    {
        temp = 'n';
        printf("Выберите тип данных: с - char, ld - long double\n");
        scanf("%c", &choice);
        if ((choice == 'c') || (choice == 'C'))
        {
            while (getchar() != '\n')
                ;
            printf("Введите символ:\n");
            scanf("%c", &inputChar);
            printf("Результат: ");
            processChar(inputChar);
            puts("Введите размер группы для перестановки:");
            scanf("%d", &groupSize);
            puts("Введите номер страшего разряда:");
            scanf("%d", &msbPosition);
            mirrorChar(inputChar, groupSize, msbPosition);
        }
        else
        {
            while (getchar() != '\n')
                ;
            printf("Введите число:\n");
            scanf("%Lf", &inputDouble);
            printf("Результат: ");
            processLD(inputDouble);
            inputLD.ld = inputDouble;
            printSEM(inputLD);
            puts("Введите размер группы для перестановки:");
            scanf("%d", &groupSize);
            puts("Введите номер страшего разряда:");
            scanf("%d", &msbPosition);
            mirrorLD(inputDouble, groupSize, msbPosition);
        }
        while (getchar() != '\n')
            ;
        puts("Хотите продолжить? (y/n)");
        scanf("%c", &temp);
    } while (temp != 'n');

    return 0;
}