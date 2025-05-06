#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// все по аналогии с readers.c
#define SHM_NAME     L"SharedBuffer"
#define MUTEX_NAME   L"WritersMutex"
#define SEM_NAME     L"ReadersSemaphore"
#define PAGE_COUNT   9
#define PAGE_SIZE    4096
#define TOTAL_SIZE   (4 + PAGE_COUNT * PAGE_SIZE)
#define LOG_FMT      "writers_logs/writer_%02d.log"

#define COUNTER_OFFSET 0
#define BUFFER_OFFSET  4

BYTE* pageBase = NULL;
FILE* logFile = NULL;
HANDLE hMap = NULL;
HANDLE hMutex = NULL;
HANDLE hReadersSemaphore = NULL;
int writerId = -1;

void logEvent(const char* state, int pageIndex, DWORD timestamp) {
    if (logFile) {
        if (pageIndex >= 0)
            fprintf(logFile, "WRITER_%02d %s PAGE_%d %lu\n", writerId, state, pageIndex, timestamp);
        else
            fprintf(logFile, "WRITER_%02d %s %lu\n", writerId, state, timestamp);
        fflush(logFile);
    }
}

void initLog(int id) {
    char filename[64];
    sprintf(filename, LOG_FMT, id);
    logFile = fopen(filename, "w");
    if (!logFile) {
        fprintf(stderr, "Failed to open log file\n");
        exit(1);
    }
}

void initSharedMemory() {
    hMap = CreateFileMappingW(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        TOTAL_SIZE,
        SHM_NAME
    );

    if (!hMap) {
        fprintf(stderr, "CreateFileMapping failed: %lu\n", GetLastError());
        exit(1);
    }

    BYTE* base = (BYTE*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, TOTAL_SIZE);
    if (!base) {
        fprintf(stderr, "MapViewOfFile init failed: %lu\n", GetLastError());
        exit(1);
    }
    if (GetLastError() != ERROR_ALREADY_EXISTS) {
        // если мы первые, то надо заполнить память
        memset(base, 0, TOTAL_SIZE);
    }
    pageBase = base + BUFFER_OFFSET;
}

void initSyncObjects() {
  hMutex = CreateMutexW(NULL, FALSE, MUTEX_NAME);
  if (!hMutex) {
      fprintf(stderr,"CreateMutex failed:%lu\n",GetLastError());
      exit(1);
  }

  hReadersSemaphore = CreateSemaphoreW(
      NULL,         
      1,             // initial count
      1,             // max count
      SEM_NAME       
  );
  if (!hReadersSemaphore) {
      fprintf(stderr,"CreateSemaphore failed:%lu\n",GetLastError());
      exit(1);
  }
}

void cleanup() {
    if (logFile) fclose(logFile);
    if (pageBase) UnmapViewOfFile(pageBase - BUFFER_OFFSET);
    if (hMap) CloseHandle(hMap);
    if (hMutex) CloseHandle(hMutex);
}

void performWriteIteration() {
    int pageIndex = rand() % PAGE_COUNT;
    DWORD tWait = GetTickCount();
    logEvent("WAIT_WRITE", -1, tWait);

    // ждем, пока все читатели покинут "читальный зал"
    WaitForSingleObject(hMutex, INFINITE);

    DWORD tWrite = GetTickCount();
    logEvent("WRITE", pageIndex, tWrite);

    BYTE* target = pageBase + pageIndex * PAGE_SIZE;
    char content[16] = {0};
    sprintf(content, "hello_%02d", writerId);
    memcpy(target, content, strlen(content) + 1);

    Sleep(5 + rand() % 10); 

    DWORD tRelease = GetTickCount();
    logEvent("RELEASE", -1, tRelease);

    // покидаем "читальный зал" сами
    ReleaseMutex(hMutex);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: Writer.exe <writer_id>\n");
        return 1;
    }

    writerId = atoi(argv[1]);
    srand((unsigned int)(time(NULL) + writerId));

    initLog(writerId);
    initSharedMemory();
    initSyncObjects();

    for (int i = 0; i < 5; ++i) {
        performWriteIteration();
    }

    cleanup();
    printf("Writer %d finished\n", writerId);
    return 0;
}
