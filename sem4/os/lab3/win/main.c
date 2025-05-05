#include <windows.h>
#include <stdio.h>

#define N 100000000     
#define B (10 * 331131)
#define THREAD_COUNT 10
// Я ПИСАЛ КОММЕНТАРИИ И КОД ЛИЧНО          

double global_sum = 0.0;     
/** 
 * массив состояний потоков, который помечает, работает ли i-ый поток в данный момент (по аналогии с первой лабораторной)
 */             
BOOL worker_active[THREAD_COUNT] = {0}; 
/**
 * массив самих потоков
 * читает и обновляет - main()
 */
HANDLE hThreads[THREAD_COUNT];
/**
 * мьютекс, который нужен для того, чтобы в 1 момент времени только 1 поток имел доступ к актуальной информации
 * о global_sum и worker_active
 * соответственно мы ожидаем релиза мьютекса другим потоком и только после можем начать работу с общими переменными
 * 
 * в мейне также вызывается, когда нужно передать потоку новые данные для работы
 */            
HANDLE hMutex;                            
/**
 * индекс, который указывает на начало следующего интервала интегрирования
 */
LONG next_block_start = 0;               

/**
 * данную структуру я передаю в воркер, чтобы он мог:
 * 1. получить начало и конец интервала интегрирования (ну в нашем случае суммирования)
 * 2. получить свой индекс в массиве, чтобы обновить состояния active
 */
typedef struct {
    int thread_idx;  
    LONG start_idx;  
    LONG end_idx;    
} ThreadParams;

/**
 * чтобы по 100 раз не выделять память на одни и те же структуры лучше создать массив и переназначать отработавшие значения
 */
ThreadParams params[THREAD_COUNT];

DWORD WINAPI Worker(LPVOID lpParam) {
    // собственно распаковываем данные, переданные из мейна
    ThreadParams* p = (ThreadParams*)lpParam;

    while (1) {
        double local_sum = 0.0;
        double invN = 1.0 / (double)N;

        for (LONG i = p->start_idx; i < p->end_idx && i < N; ++i) {
            double x = (i + 0.5) * invN;
            local_sum += 4.0 / (1.0 + x * x);
        }

        WaitForSingleObject(hMutex, INFINITE);

        global_sum += local_sum;
        worker_active[p->thread_idx] = FALSE;

        ReleaseMutex(hMutex);

        // переводим поток в сон, т к он отработал на переданных параметрах
        SuspendThread(GetCurrentThread());

        // а вот теперь, когда мы в мейне вызвале ResumeThread, в params[i] уже лежат новые данные и мы можем сделать проверку
        // на выход за границы (а если такого не произошло, то производим вычисления на переданном интервале)
        if (p->start_idx >= N) break;
    }

    return 0;
}

int main() {
    hMutex = CreateMutex(NULL, FALSE, NULL); // возвращает HANDLE, так что мы можем работать с ним с помощью WaitForSingleObject

    for (int i = 0; i < THREAD_COUNT; ++i) {
        LONG start = next_block_start;
        next_block_start += B;
        LONG end = next_block_start;

        // вот тут потоки еще не запущены, так что можно без мьютекса обновлять данные
        params[i].thread_idx = i;
        params[i].start_idx = start;
        params[i].end_idx = end;
        worker_active[i] = TRUE;

        hThreads[i] = CreateThread(NULL, 0, Worker, &params[i], CREATE_SUSPENDED, NULL);
    }

    LARGE_INTEGER freq, t_start, t_end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_start);

    // запускаем потоки
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        ResumeThread(hThreads[i]);
    }

    while (1) {
        BOOL all_done = TRUE;

        WaitForSingleObject(hMutex, INFINITE);

        // вот тут мы увидели, что какой-то из потоков закончил работу -> надо
        // 1. понять какой - для этого и существует worker_active
        // 2. передать ему новые данные (если еще не вышли за границы интервала интегрирования)
        for (int i = 0; i < THREAD_COUNT; ++i) {
            if (!worker_active[i] && next_block_start < N) {
                LONG start = next_block_start;
                LONG end = start + B;
                next_block_start += B;

                params[i].start_idx = start;
                params[i].end_idx = end;
                worker_active[i] = TRUE;

                ResumeThread(hThreads[i]);
            }

            if (worker_active[i] || next_block_start < N) {
                all_done = FALSE;
            }
        }

        ReleaseMutex(hMutex);

        if (all_done) break;
        Sleep(1); 
    }

    QueryPerformanceCounter(&t_end);
    double elapsed = (double)(t_end.QuadPart - t_start.QuadPart)
                     / (double)freq.QuadPart * 1000;

    // вот здесь наши потоки уже закончили обработку всего интервала N, но они находятся в состоянии Suspend
    // так что надо освободить ресурсы, продолжить потоки и передать им такие данные, чтобы они вышли из while(1)
    for (int i = 0; i < THREAD_COUNT; ++i) {
        params[i].start_idx = N; 
        ResumeThread(hThreads[i]);
    }

    WaitForMultipleObjects(THREAD_COUNT, hThreads, TRUE, INFINITE);

    for (int i = 0; i < THREAD_COUNT; ++i) {
        CloseHandle(hThreads[i]);
    }
    CloseHandle(hMutex);

    printf("Pi = %.15f\n", global_sum / (double)N);
    printf("Threads: %d, Time: %.6f ms\n", THREAD_COUNT, elapsed);
    return 0;
}
