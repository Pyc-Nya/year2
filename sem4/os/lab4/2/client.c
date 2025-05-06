#include <windows.h>
#include <stdio.h>
#include <string.h>

#define PIPE_NAME "\\\\.\\pipe\\ChatPipe"
#define BUF 512
#define MAX_HISTORY 1024

HANDLE hPipe = INVALID_HANDLE_VALUE;
char history[MAX_HISTORY][BUF];
int histCount = 0;

void addToHistory(const char *tag, const char *msg) {
    if (histCount >= MAX_HISTORY) return;
    _snprintf_s(history[histCount], BUF, _TRUNCATE, "%s %s", tag, msg);
    ++histCount;
}

void printHistory(void) {
    puts("=== HISTORY ===");
    for (int i = 0; i < histCount; ++i) puts(history[i]);
    puts("================\n");
}

VOID CALLBACK ReadCompleted(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
);

void connectToServer(void) {
    if (hPipe != INVALID_HANDLE_VALUE) {
        puts("Already connected.");
        return;
    }
    hPipe = CreateFile(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,         
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,  
        NULL
    );
    if (hPipe == INVALID_HANDLE_VALUE) {
        puts("Connect error.");
    } else {
        puts("Connected.");
    }
}

void sendMessage(void) {
    if (hPipe == INVALID_HANDLE_VALUE) { 
      puts("Not connected."); 
      return; 
    }
    char buf[BUF];
    printf("Message: "); 
    fgets(buf, BUF, stdin);
    buf[strcspn(buf, "\n")] = 0;
    DWORD wr;
    if (WriteFile(hPipe, buf, (DWORD)strlen(buf) + 1, &wr, NULL)) {
        addToHistory("[CLIENT]", buf);
    } else {
        puts("Write error.");
    }
}

void startAsyncRead(void) {
    static OVERLAPPED ov = { 0 };
    static char buf[BUF] = { 0 };

	ov.Pointer = buf;

    // Запускаем асинхронное чтение с callback
    if (!ReadFileEx(
            hPipe,
            buf,
            BUF,
            &ov,
            ReadCompleted
        )) {
        printf("ReadFileEx error: %lu\n", GetLastError());
    }
}

void checkMessage(void) {
    if (hPipe == INVALID_HANDLE_VALUE) { 
		puts("Not connected."); 
		return; 
	}
    startAsyncRead();
    puts("Async read started.");
}

// Функция-обработчик завершения чтения
VOID CALLBACK ReadCompleted(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
) {
    if (dwErrorCode == 0 && dwNumberOfBytesTransfered > 0) {
        char* buf = (char*)lpOverlapped->Pointer;
        printf("[SERVER] %s\n", buf);
        addToHistory("[SERVER]", buf);
    } else {
        printf("Read error or no data: code=%lu bytes=%lu\n",
               dwErrorCode, dwNumberOfBytesTransfered);
    }
}

int main(void) {
    while(1) {
        system("cls");
        printHistory();
        puts("=== CLIENT ===");
        puts("1 Connect");
        puts("2 Send message");
        puts("3 Check messages (async)");
        puts("4 Exit");
        printf("> ");
        int c; 
		scanf("%d", &c);
        getchar();
        switch (c) {
            case 1: connectToServer(); break;
            case 2: sendMessage(); break;
            case 3: checkMessage(); break;
            case 4: return 0;
        }
        getchar();
		SleepEx(0, TRUE);
    }
}
