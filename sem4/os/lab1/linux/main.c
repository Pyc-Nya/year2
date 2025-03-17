#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <inttypes.h>
#include <mutex>
#include <condition_variable>
#include <fstream>      // Для работы с файлом-логом
#include <iomanip>      // Для std::put_time или strftime

using namespace std;
using namespace std::chrono;

#define kb4 4096
#define kb16 16384
#define mb 1048576
#define mb10 10485760

// Глобальные переменные
int chunk;              // Текущий размер «кластера» для чтения/записи
int fileReadHandler;
int fileWriteHandler;
int operationsCount;    // Текущее количество одновременных aio-операций
int fileSize;
std::mutex gMutex;
std::condition_variable gCv;

struct AioOperation {
    struct aiocb* aio;       // Указатель на структуру асинхронной операции ввода/вывода
    char *buffer;            // Буфер данных
    int  isReading;          // Флаг: 1 - операция чтения, 0 - операция записи
    int  isWorking;          // Флаг: 1 - операция ещё активна, 0 - завершена
};

// Прототипы функций
void freeMemory(struct AioOperation* operations, int n);
void aioCompletionHandler(sigval_t sigval);
void initOperations(struct AioOperation* operations, int cluster);
void startOperations(struct AioOperation* operations);
void await(struct AioOperation* operations, int count);

int main() {
    struct stat fileStat;
    struct AioOperation* operations;
    string pathRead = "1g\0", pathWrite = "1gc\0";

    // cout << "Введите путь к копируемому файлу: ";
    // cin >> pathRead;

    // cout << "Введите путь к файлу записи: ";
    // cin >> pathWrite;

    int tempFileHandler = open(pathRead.c_str(), O_RDONLY);
    if (tempFileHandler == -1) {
        cerr << "Ошибка: Невозможно открыть исходный файл!" << endl;
        return -1;
    }
    if (fstat(tempFileHandler, &fileStat) == -1) {
        cerr << "Ошибка: Нет доступа к информации о файле!" << endl;
        close(tempFileHandler);
        return -1;
    }
    fileSize = fileStat.st_size;
    close(tempFileHandler);

    cout << "Размер файла: " << fileSize << " байт" << endl;

    for (int currentChunk = mb; currentChunk <= mb10; currentChunk += mb) {
        for (int j = 0; j <= 4; j++) {
            chunk = currentChunk;
            operationsCount = (1 << j);  

            for (int iteration = 0; iteration < 5; iteration++) {
                cout << "\n=== Размер кластера: " << chunk
                     << ", операций: " << operationsCount
                     << ", повторение #" << (iteration+1) << " ===\n";

                operations = (struct AioOperation*)calloc(operationsCount, sizeof(struct AioOperation));

                auto startTime = high_resolution_clock::now();

                fileReadHandler = open(pathRead.c_str(), O_RDONLY  | O_NONBLOCK, 0666);
                fileWriteHandler = open(pathWrite.c_str(), O_CREAT  | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666);

                if (fileReadHandler == -1 || fileWriteHandler == -1) {
                    cerr << "Ошибка: Невозможно открыть один из файлов (чтения или записи)!" << endl;
                    free(operations);
                    continue; 
                }

                initOperations(operations, chunk);

                startOperations(operations);

                if (fsync(fileWriteHandler) == -1) {
                    cerr << "Ошибка: fsync не удался: " << strerror(errno) << endl;
                }
                sync();

                close(fileReadHandler);
                close(fileWriteHandler);

                auto endTime = high_resolution_clock::now();
                auto durationMs = duration_cast<milliseconds>(endTime - startTime).count();

                cout << "Копирование завершено за " << durationMs << " мс" << endl;

                {
                    ofstream logFile("lab.log", ios::app);
                    if (!logFile) {
                        cerr << "Ошибка: невозможно открыть файл lab.log для записи!\n";
                    } else {
                        auto t = std::time(nullptr);
                        std::tm tmBuf;
                        localtime_r(&t, &tmBuf);
                        char timeStr[64];
                        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &tmBuf);

                        logFile << "[" << timeStr << "] "
                                << fileSize << " "
                                << chunk << " "
                                << operationsCount << " "
                                << durationMs
                                << "\n";
                    }
                }
                freeMemory(operations, operationsCount);
                sleep(2);
            }
        }
    }

    return 0;
}

void freeMemory(struct AioOperation* operations, int n) {
    for (int i = 0; i < n; i++) {
        free(operations[i].buffer);
        free(operations[i].aio);
    }
    free(operations);
}

void aioCompletionHandler(sigval_t sigval) {
    struct AioOperation *aioOp = (struct AioOperation *)sigval.sival_ptr;

    if (aioOp->isReading) {
        aioOp->isReading = 0;
        aioOp->aio->aio_fildes = fileWriteHandler;

        if (aio_write(aioOp->aio) == -1) {
            cerr << "Ошибка: Асинхронная запись невозможна!" << endl;
            aioOp->isWorking = 0;
            gCv.notify_all();
            return;
        }
    } else {
        if (aioOp->isWorking) {
            aioOp->aio->aio_offset += chunk * operationsCount;
            aioOp->aio->aio_fildes = fileReadHandler;

            if (aioOp->aio->aio_offset >= fileSize) {
                aioOp->isWorking = 0;
            } else {
                aioOp->isReading = 1;
                if (aioOp->aio->aio_offset + chunk > fileSize) {
                    aioOp->aio->aio_nbytes = fileSize - aioOp->aio->aio_offset;
                }

                if (aio_read(aioOp->aio) == -1) {
                    cerr << "Ошибка: Асинхронное чтение невозможно!" << endl;
                    aioOp->isWorking = 0;
                    gCv.notify_all();
                    return;
                }
            }
        }
    }

    gCv.notify_all();
}

void initOperations(struct AioOperation* operations, int cluster) {
    for (int i = 0; i < operationsCount; i++) {
        operations[i].aio = (struct aiocb*)calloc(1, sizeof(struct aiocb));
        memset(operations[i].aio, 0, sizeof(*(operations[i].aio)));
        operations[i].buffer = (char*)calloc(cluster, sizeof(char));

        operations[i].aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
        operations[i].aio->aio_sigevent.sigev_notify_function = aioCompletionHandler;
        operations[i].aio->aio_sigevent.sigev_value.sival_ptr = &operations[i];

        if (cluster > fileSize) {
            operations[i].aio->aio_nbytes = fileSize;
        } else {
            operations[i].aio->aio_nbytes = cluster;
        }

        operations[i].aio->aio_fildes = fileReadHandler;
        operations[i].aio->aio_offset = cluster * i;
        operations[i].aio->aio_buf = operations[i].buffer;

        operations[i].isWorking = 1;
        operations[i].isReading = 1;
    }
}

void startOperations(struct AioOperation* operations) {
    for (int i = 0; i < operationsCount; i++) {
        if (operations[i].aio->aio_offset >= fileSize) {
            operations[i].isWorking = 0;
        } else {
            if (operations[i].aio->aio_offset + chunk > fileSize) {
                operations[i].aio->aio_nbytes = fileSize - operations[i].aio->aio_offset;
            }
            if (aio_read(operations[i].aio) == -1) {
                cerr << "Ошибка: Асинхронное чтение невозможно!" << endl;
                operations[i].isWorking = 0;
            }
        }
    }

    await(operations, operationsCount);
}

void await(struct AioOperation* operations, int count) {
    unique_lock<mutex> lock(gMutex);
    auto allDone = [&]() {
        for (int i = 0; i < count; i++) {
            if (operations[i].isWorking != 0) {
                return false;
            }
        }
        return true;
    };

    gCv.wait(lock, allDone);
}
