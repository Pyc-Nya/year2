// main.c
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100000000
#define B (10 * 331131)

typedef struct {
    int    thread_idx;
    LONG   start_idx;
    LONG   end_idx;
} ThreadParams;

static double   global_sum      = 0.0;
static BOOL    *worker_active   = NULL;
static HANDLE  *hThreads        = NULL;
static HANDLE   hMutex;
static LONG     next_block_start;
static ThreadParams *params     = NULL;

DWORD WINAPI Worker(LPVOID lpParam) {
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

        // Засыпаем до следующего возобновления
        SuspendThread(GetCurrentThread());

        // Если получен сигнал завершения (start_idx ≥ N) — выходим
        if (p->start_idx >= N) 
            break;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <thread_count>\n", argv[0]);
        return 1;
    }
    int THREAD_COUNT = atoi(argv[1]);
    if (THREAD_COUNT <= 0) {
        fprintf(stderr, "Error: invalid thread count %d\n", THREAD_COUNT);
        return 1;
    }

    // Выделяем динамические массивы
    worker_active = (BOOL*)   malloc(THREAD_COUNT * sizeof(BOOL));
    hThreads      = (HANDLE*) malloc(THREAD_COUNT * sizeof(HANDLE));
    params        = (ThreadParams*)
                    malloc(THREAD_COUNT * sizeof(ThreadParams));
    if (!worker_active || !hThreads || !params) {
        fprintf(stderr, "Error: allocation failed\n");
        return 1;
    }

    // Инициализация
    global_sum        = 0.0;
    next_block_start  = 0;
    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        fprintf(stderr, "Error: CreateMutex failed\n");
        return 1;
    }

    // Засекаем время
    LARGE_INTEGER freq, t_start, t_end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_start);

    // Создаём и запускаем потоки
    for (int i = 0; i < THREAD_COUNT; ++i) {
        LONG start = next_block_start;
        next_block_start += B;
        LONG end   = next_block_start;

        params[i].thread_idx = i;
        params[i].start_idx  = start;
        params[i].end_idx    = end;
        worker_active[i]     = TRUE;

        hThreads[i] = CreateThread(
            NULL, 0, Worker, &params[i],
            CREATE_SUSPENDED, NULL
        );
        if (hThreads[i] == NULL) {
            fprintf(stderr, "Error: CreateThread failed (i=%d)\n", i);
            return 1;
        }
    }
    for (int i = 0; i < THREAD_COUNT; ++i) {
        ResumeThread(hThreads[i]);
    }

    // Основной цикл перераспределения работы
    while (1) {
        BOOL all_done = TRUE;
        WaitForSingleObject(hMutex, INFINITE);

        for (int i = 0; i < THREAD_COUNT; ++i) {
            if (!worker_active[i] && next_block_start < N) {
                LONG start = next_block_start;
                LONG end   = start + B;
                next_block_start += B;

                params[i].start_idx   = start;
                params[i].end_idx     = end;
                worker_active[i]      = TRUE;
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

    // Завершаем потоки
    for (int i = 0; i < THREAD_COUNT; ++i) {
        params[i].start_idx = N;
        ResumeThread(hThreads[i]);
    }
    WaitForMultipleObjects(THREAD_COUNT, hThreads, TRUE, INFINITE);

    // Останавливаем таймер
    QueryPerformanceCounter(&t_end);
    double elapsed = (double)(t_end.QuadPart - t_start.QuadPart)
                     / (double)freq.QuadPart;

    // Выводим результат
    printf("Threads: %d, Time: %.6f\n", THREAD_COUNT, elapsed);

    // Освобождаем ресурсы
    for (int i = 0; i < THREAD_COUNT; ++i) {
        CloseHandle(hThreads[i]);
    }
    CloseHandle(hMutex);
    free(worker_active);
    free(hThreads);
    free(params);

    return 0;
}
