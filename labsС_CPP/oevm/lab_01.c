#include <stdio.h>
#include <string.h>
#include <math.h>

union Data
{
    long double ld;
    u_int64_t ll;
    struct
    {
        u_int64_t mantisa : 52;
        u_int64_t exponent : 11;
        u_int64_t sign : 1;
    } parts;
};

/*************************************************
 * CHAR START
 *************************************************/

int binaryToDecimalInt(int binary)
{
    int decimal = 0;
    int base = 1;

    while (binary > 0)
    {
        decimal += (binary & 1) * base;
        binary >>= 1;
        base *= 2;
    }

    return decimal;
}

void outputCh(int x)
{
    for (int i = 8 - 1; i > -1; i--)
        printf("%d", x >> i & 1);
    printf("\n");
}

void processChar(char input)
{
    for (int i = 0; i < 8; i++)
    {
        printf("%d", ((input >> (8 - i - 1)) & 1));
    }
    printf("\n");
}

void mirrorChar(unsigned int input, int groupSize, int msbPosition)
{
    int mirroredGroup = 0;
    int mask = 0, group = 0;
    int dec;

    // создаем маску для извлечения группы разрядов
    mask = ((1 << groupSize) - 1) << (msbPosition - groupSize + 1);

    // извлекаем группу разрядов
    group = (input & mask) >> (msbPosition - groupSize + 1);

    // отражаем группу разрядов
    for (int i = 0; i < groupSize; i++)
    {
        mirroredGroup = (mirroredGroup << 1) | (group & 1);
        group >>= 1;
    }

    // сдвигаем отраженную группу в нужное положение
    mirroredGroup = mirroredGroup << (msbPosition - groupSize + 1);

    printf("Result of mirroring: \n");
    printf("(2) ");
    outputCh((input & ~mask) | mirroredGroup);
    dec = binaryToDecimalInt((input & ~mask) | mirroredGroup);
    printf("(10) %d\n", dec);
}

/*************************************************
 * CHAR END
 *************************************************/

/*************************************************
 * LONG DOUBLE START
 *************************************************/

long double binaryToDecimalLD(u_int64_t exponent, u_int64_t mantisa, u_int64_t sign)
{
    int exp = 0;
    for (int i = 0; i < 11; i++)
    {
        exp = exp * 2 + ((exponent >> (10 - i)) & 1);
    }

    long double mant = 0;
    for (int i = 0; i < 52; i++)
    {
        mant += ((mantisa >> (51 - i)) & 1) * pow(2, -i - 1);
    }

    long double result = pow(2, exp - 1023) * (1 + mant);

    if (sign == 1)
    {
        result = -result;
    }
    return result;
}

void outputL(u_int64_t x)
{
    int cnt = 0;
    for (int i = 8 * sizeof(x) - 1; i > -1; i--)
    {
        if ((cnt % 8) != 0)
        {
            printf("%llu", x >> i & 1);
        }
        else
        {
            printf("%llu ", x >> i & 1);
        }
        cnt++;
    }
}
void processLD(long double input)
{
    union Data res;

    res.ld = input;

    {
        for (int i = 8 * sizeof(u_int64_t) - 1; i > -1; i--)
        {
            printf("%llu", res.ll >> i & 1);
        }
        printf("\n");
    }
}

void printSEM(union Data output)
{
    puts("\n/************************__COMPONENTS__*************************");
    printf("* sign : ");
    for (int i = 0; i > -1; i--)
    {
        printf("%d", output.parts.sign >> i & 1);
    }

    printf("\n* exponent : ");
    for (int i = 11 - 1; i > -1; i--)
    {
        printf("%d", output.parts.exponent >> i & 1);
    }
    printf("\n* mantisa : ");
    for (int i = 52 - 1; i > -1; i--)
    {
        printf("%llu", output.parts.mantisa >> i & 1);
    }
    puts("\n***************************************************************/\n");
}

void mirrorLD(long double input, int groupSize, int msbPosition)
{
    union Data temp;
    union Data res;
    u_int64_t mask = 0, group = 0, mirrorGroup = 0;
    u_int64_t mant = 0, exp = 0;
    double dec;

    temp.ld = input;

    // создаем маску для извлечения группы разрядов
    mask = ((1ULL << groupSize) - 1) << (msbPosition - groupSize + 1);

    puts("\n/**************************__SHIFTS__***************************");
    printf("Mask : ");
    outputL(mask);
    printf("\nGroup : ");

    // извлекаем группу разрядов
    group = (temp.ll & mask) >> (msbPosition - groupSize + 1);
    outputL(group);

    // отражаем группу разрядов
    for (int i = 0; i < groupSize; ++i)
    {
        mirrorGroup = (mirrorGroup << 1) | (group & 1);
        group >>= 1;
    }

    // сдвигаем отраженную группу в нужное положение
    mirrorGroup = mirrorGroup << (msbPosition - groupSize + 1);

    printf("\nMirrored group : ");
    outputL(mirrorGroup);
    puts("\n***************************************************************/\n");

    res.ll = (temp.ll & ~mask) | mirrorGroup;

    for (int i = 11 - 1; i > -1; i--)
    {
        exp = (exp << 1) | (res.parts.exponent >> i & 1);
    }

    for (int i = 52 - 1; i > -1; i--)
    {
        mant = (mant << 1) | (res.parts.mantisa >> i & 1);
    }

    printf("Result before mirroring: (2) ");
    outputL(temp.ll);
    printf(" : (10) %Lf\n", temp.ld);
    printf("Result after  mirroring: (2) ");
    outputL((temp.ll & ~mask) | mirrorGroup);
    printf(" : (10) %.50Lf\n",binaryToDecimalLD(exp, mant, res.parts.sign));
    printSEM(res);
}

/*************************************************
 * LONG DOUBLE END
 *************************************************/

/*************************************************
 * LONG DOUBLE END
 *************************************************/

int main()
{
    char choice = 'l';
    char inputChar;
    char temp;
    int binChar;
    int groupSize;
    int msbPosition;
    union Data data;

    do
    {
        temp = 'n';
        printf("Choose data type: с - char, ld - long double\n");
        scanf("%c", &choice);
        if ((choice == 'c') || (choice == 'C'))
        {
            while (getchar() != '\n')
                ;
            printf("Enter your char:\n");
            scanf("%c", &inputChar);
            printf("\nChar binary result: ");
            processChar(inputChar);
            puts("Enter group size for mirroring:");
            scanf("%d", &groupSize);
            puts("Enter the number of the highest digit of the group (right -> left):");
            scanf("%d", &msbPosition);
            mirrorChar(inputChar, groupSize, msbPosition);
        }
        else
        {
            while (getchar() != '\n')
                ;
            printf("Enter your number:\n");
            scanf("%LG", &data.ld);
            printf("\nLong double binary result: ");
            processLD(data.ld);
            printSEM(data);
            puts("Enter group size for mirroring:");
            scanf("%d", &groupSize);
            while (getchar() != '\n')
                ;
            puts("Enter the number of the highest digit of the group (right -> left):");
            scanf("%d", &msbPosition);
            mirrorLD(data.ld, groupSize, msbPosition);
        }
        while (getchar() != '\n')
            ;
        puts("Do you want to continue? (y/n)");
        scanf("%c", &temp);
    } while (temp != 'n');

    return 0;
}
