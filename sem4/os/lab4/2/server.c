#include <windows.h>
#include <stdio.h>
#include <string.h>

#define PIPE_NAME "\\\\.\\pipe\\ChatPipe"
// максимальный размер сообщения
#define BUF 512
// макимальное количество сообщений в чате
#define MAX_HISTORY 1024  

HANDLE hPipe = NULL;
int clientOn = 0;
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

void createPipe(void) {
    if (hPipe) CloseHandle(hPipe);
    hPipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
        1, BUF, BUF, 0, NULL);
    puts(hPipe == INVALID_HANDLE_VALUE ? "CreateNamedPipe error" : "Pipe created.");
}

void waitClient(void) {
    if (!hPipe) { 
        puts("Create pipe first."); 
        return; 
    }
    puts("Waiting client...");
    if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) { 
        clientOn = 1; 
        puts("Client connected."); 
    }
    else puts("Connect error.");
}

void sendMessage(void) {
    if (!clientOn) { 
        puts("No client."); 
        return; 
    }
    char buf[BUF];
    printf("Message: "); fgets(buf, BUF, stdin);
    buf[strcspn(buf, "\n")] = 0;
    DWORD wr;
    if (WriteFile(hPipe, buf, (DWORD)strlen(buf) + 1, &wr, NULL))
        addToHistory("[SERVER]", buf);
    else
        puts("Write error.");
}

void checkMessage(void) {
    if (!clientOn) { 
        puts("No client."); 
        return; 
    }
    char buf[BUF] = {0};
    DWORD rd = 0;
    if (ReadFile(hPipe, buf, BUF, &rd, NULL)) {
        printf("[CLIENT] %s\n", buf);
    }
}

void disconnect(void) {
    if (hPipe) { 
        DisconnectNamedPipe(hPipe); 
        clientOn = 0; 
        puts("Disconnected."); 
    }
}

int main(void) {
    while(1) {
        system("cls");
        printHistory();              
        puts("=== SERVER ===");
        puts("1 Create pipe");
        puts("2 Wait client");
        puts("3 Send message");
        puts("4 Check messages");
        puts("5 Disconnect");
        puts("6 Exit");
        printf("> ");
        int c; 
        scanf("%d", &c);
        getchar();
        switch (c) {
            case 1: createPipe(); break;
            case 2: waitClient(); break;
            case 3: sendMessage(); break;
            case 4: checkMessage(); break;
            case 5: disconnect(); break;
            case 6: return 0;
        }
        getchar();
    }
}
