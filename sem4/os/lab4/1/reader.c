#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>

// задаем имя нашему буферу, мьютексу и семафору
#define SHM_NAME L"SharedBuffer"
#define MUTEX_NAME L"WritersMutex"
#define SEM_NAME L"ReadersSemaphore"
// 331131 => 3 + 1 + 1 + 3 + 1 = 9
#define PAGE_COUNT 9
#define PAGE_SIZE 4096
#define LOG_NAME_FMT "readers_logs/reader_%02d.log"

// чтобы хранить количество читателей в данный момент, я выделил для этого 4 байта в нашей шейред мемори
// это нужно, чтобы понимать, является ли наш читатель первым или последним
#define READER_COUNTER_OFFSET 0
// ну а сами данные начинаются соответственно с оффсетом в 4 байта
#define BUFFER_OFFSET 4

volatile LONG* readersCount = NULL;
BYTE* pageBase = NULL;

HANDLE hWritersMutex = NULL;
HANDLE hReadersSemaphore = NULL;
HANDLE hMap = NULL;
FILE* logFile = NULL;

// через args в мейн я передаю номер читателя
int readerId = -1;

void logEvent(const char* state, int pageIndex, DWORD timestamp) {
    if (logFile) {
        if (pageIndex >= 0)
            fprintf(logFile, "READER_%02d %s PAGE_%d %lu\n", readerId, state, pageIndex, timestamp);
        else
            fprintf(logFile, "READER_%02d %s %lu\n", readerId, state, timestamp);
        fflush(logFile);
    }
}

void initLog(int id) {
    char filename[64];
    sprintf(filename, LOG_NAME_FMT, id);
    logFile = fopen(filename, "w");
    if (!logFile) {
        fprintf(stderr, "Failed to open log file.\n");
        exit(1);
    }
}

// читатели начинают свою работу уже после писателей, так что им не надо заполнять память, это уже сделано писателями
void initSharedObjects() {
    hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
    if (!hMap) {
        fprintf(stderr, "OpenFileMapping failed: %lu\n", GetLastError());
        exit(1);
    }

    BYTE* base = (BYTE*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!base) {
        fprintf(stderr, "MapViewOfFile failed: %lu\n", GetLastError());
        exit(1);
    }

    readersCount = (volatile LONG*)(base + READER_COUNTER_OFFSET);
    pageBase = base + BUFFER_OFFSET;

    // мьютекс и семафор тоже создаются писателями
    hWritersMutex = OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_NAME);
    if (!hWritersMutex) {
        fprintf(stderr, "OpenMutex failed: %lu\n", GetLastError());
        exit(1);
    }

    hReadersSemaphore = OpenSemaphoreW(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, SEM_NAME);
    if (!hReadersSemaphore) {
        fprintf(stderr, "OpenSemaphore failed: %lu\n", GetLastError());
        exit(1);
    }
}

void cleanup() {
    if (logFile) fclose(logFile);
    if (pageBase) UnmapViewOfFile((LPCVOID)(pageBase - BUFFER_OFFSET));
    if (hMap) CloseHandle(hMap);
    if (hWritersMutex) CloseHandle(hWritersMutex);
    if (hReadersSemaphore) CloseHandle(hReadersSemaphore);
}

void performReadIteration() {
    int pageIndex = rand() % PAGE_COUNT;
    DWORD tStart = timeGetTime();
    logEvent("WAIT_READ", -1, tStart);

    // захватываем семафор, который показывает, читает ли кто-то страницу
    WaitForSingleObject(hReadersSemaphore, INFINITE);
    if (InterlockedIncrement(readersCount) == 1) {
        // но если читает, то надо запретить писать в нее, однако делать это N раз (то есть для каждого читателя)
        // не нужно, вместо этого сделаем это лишь 1 раз для самого первого читателя
        WaitForSingleObject(hWritersMutex, INFINITE);
    }
    // все, мы обновили readersCount, можно отпускать семафор
    ReleaseSemaphore(hReadersSemaphore, 1, NULL);

    DWORD tRead = timeGetTime();
    logEvent("READ", pageIndex, tRead);

    BYTE* page = pageBase + pageIndex * PAGE_SIZE;
    char buffer[16] = {0};
    memcpy(buffer, page, 9);

    fprintf(logFile, "READER_%02d READ_CONTENT PAGE_%d \"%s\"\n", readerId, pageIndex, buffer);
    fflush(logFile);

    Sleep(5 + rand() % 10);

    DWORD tRelease = timeGetTime();
    logEvent("RELEASE", -1, tRelease);

    // все то же самое, но наоброт, теперь мы выходим из "читального зала"
    WaitForSingleObject(hReadersSemaphore, INFINITE);
    if (InterlockedDecrement(readersCount) == 0) {
        // а если были последними, то разрешаем записывать
        ReleaseMutex(hWritersMutex);
    }
    ReleaseSemaphore(hReadersSemaphore, 1, NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: Reader.exe <reader_id>\n");
        return 1;
    }

    readerId = atoi(argv[1]);
    srand((unsigned int)(time(NULL) + readerId));

    initLog(readerId);
    initSharedObjects();

    for (int i = 0; i < 5; ++i) {
        performReadIteration();
    }

    cleanup();
    printf("Reader %d finished\n", readerId);
    return 0;
}
