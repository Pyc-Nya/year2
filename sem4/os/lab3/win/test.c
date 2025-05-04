#include <windows.h>
#include <stdio.h>

#define THREAD_COUNT 3

// Общая переменная, к которой будут обращаться потоки
int globalSum = 0;

// Мьютекс для защиты globalSum
HANDLE hMutex;

DWORD WINAPI Worker(LPVOID lpParam) {
    int id = (int)(size_t)lpParam;

    // Вычислим локальную сумму от 0 до 99
    int localSum = 0;
    for (int i = 0; i < 100; ++i) {
        localSum += i;
        Sleep(1); // имитация работы
    }

    printf("Thread %d: calculated localSum = %d\n", id, localSum);

    // Теперь добавим локальную сумму в globalSum — под защитой мьютекса
    WaitForSingleObject(hMutex, INFINITE);  // захватываем мьютекс

    // Критическая секция: только один поток здесь одновременно
    globalSum += localSum;

    ReleaseMutex(hMutex);  // освобождаем мьютекс

    return 0;
}

int main(void) {
    HANDLE threads[THREAD_COUNT];

    // Создаём мьютекс (в незахваченном состоянии)
    hMutex = CreateMutex(NULL, FALSE, NULL);

    printf("Main: creating threads...\n");

    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads[i] = CreateThread(
            NULL,
            0,
            Worker,
            (LPVOID)(size_t)i,
            0,
            NULL
        );
    }

    // Ждём завершения всех потоков
    WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);

    // Освобождаем ресурсы
    for (int i = 0; i < THREAD_COUNT; ++i) {
        CloseHandle(threads[i]);
    }
    CloseHandle(hMutex);

    printf("Main: all threads finished\n");
    printf("Main: final globalSum = %d\n", globalSum);  // должен быть 3 * 4950 = 14850

    return 0;
}
