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
int mb = 1024 * 1024;

// Глобальные переменные, которые будут меняться в циклах
int chunk;              // Текущий размер «кластера» для чтения/записи
int fileReadHandler;
int fileWriteHandler;
int operationsCount;    // Текущее количество одновременных aio-операций
int fileSize;
std::mutex gMutex;
std::condition_variable gCv;

struct AioOperation {
    struct aiocb* aio;       // Указатель для описания асинхронной операции ввода/вывода
    char *buffer;            // Буфер данных
    int  isReading;          // Флаг операции: (1 - чтение, 0 - запись)
    int  isWorking;          // Флаг: операция ещё активна (1) или уже закончена (0)
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
    string pathRead, pathWrite;

    // 1. Читаем пути к файлам
    cout << "Введите путь к копируемому файлу: ";
    cin >> pathRead;

    cout << "Введите путь к файлу записи: ";
    cin >> pathWrite;

    // 2. Открываем исходный файл лишь для выяснения его размера, затем закроем
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

    // 3. Внешний цикл: изменяем размер «кластера» от 512 до 8192 с шагом 512
    for (int currentChunk = mb; currentChunk <= mb; currentChunk += 8192) {
        // 4. Вложенный цикл по количеству операций 2^j, где j=0..4 (1,2,4,8,16)
        for (int j = 4; j <= 10; j++) {
            // Устанавливаем глобальные переменные для каждой под-итерации
            chunk = currentChunk;
            operationsCount = (1 << j);  // 2^j

            // 5. Запускаем 5 копирований подряд, чтобы собрать серию бенчмарков
            for (int iteration = 0; iteration < 3; iteration++) {
                cout << "\n=== Размер кластера: " << chunk
                     << ", операций: " << operationsCount
                     << ", повторение #" << (iteration+1) << " ===\n";

                // Подготовка массива операций
                operations = (struct AioOperation*)calloc(operationsCount, sizeof(struct AioOperation));

                // Засекаем время
                auto startTime = high_resolution_clock::now();

                // Открываем исходный и результирующий файлы (НЕ забывайте O_NONBLOCK)
                fileReadHandler = open(pathRead.c_str(), O_RDONLY | O_NONBLOCK, 0666);
                fileWriteHandler = open(pathWrite.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666);

                if (fileReadHandler == -1 || fileWriteHandler == -1) {
                    cerr << "Ошибка: Невозможно открыть один из файлов (чтения или записи)!" << endl;
                    free(operations);
                    continue; // Переходим к следующей итерации
                }

                // Инициализация операций
                initOperations(operations, chunk);

                // Запуск асинхронных операций
                startOperations(operations);

                // Закрываем файлы чтения/записи, когда все операции завершены
                close(fileReadHandler);
                close(fileWriteHandler);

                // Фиксируем время окончания
                auto endTime = high_resolution_clock::now();
                auto durationMs = duration_cast<milliseconds>(endTime - startTime).count();

                cout << "Копирование завершено за " << durationMs << " мс" << endl;

                // 6. Логирование результатов в lab.log
                {
                    ofstream logFile("lab.log", ios::app);
                    if (!logFile) {
                        cerr << "Ошибка: невозможно открыть файл lab.log для записи!\n";
                    } else {
                        // Получаем текущее время для лога
                        auto t = std::time(nullptr);
                        std::tm tmBuf;
                        localtime_r(&t, &tmBuf); // локальное время (потокобезопасно)

                        // Форматируем дату и время, например "2025-03-07 17:21:05"
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

                // Освобождаем динамическую память после итерации
                freeMemory(operations, operationsCount);
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

// Обработчик, вызываемый по завершению любой aio-операции (SIGEV_THREAD)
void aioCompletionHandler(sigval_t sigval) {
    struct AioOperation *aioOp = (struct AioOperation *)sigval.sival_ptr;

    // Проверяем, чтение это или запись в нашем "пинг-понге"
    if (aioOp->isReading) {
        // Мы только что закончили чтение, теперь надо делать запись
        aioOp->isReading = 0;
        aioOp->aio->aio_fildes = fileWriteHandler;

        if (aio_write(aioOp->aio) == -1) {
            cerr << "Ошибка: Асинхронная запись невозможна!" << endl;
            aioOp->isWorking = 0;
            gCv.notify_all();
            return;
        }
    } else {
        // Завершилась запись; проверяем, не нужно ли продолжать чтение
        if (aioOp->isWorking) {
            aioOp->aio->aio_offset += chunk * operationsCount;
            aioOp->aio->aio_fildes = fileReadHandler;

            // Проверяем, не вышли ли мы за границы файла
            if (aioOp->aio->aio_offset >= fileSize) {
                // Больше читать нечего => завершаем операцию
                aioOp->isWorking = 0;
            } else {
                // Нужно опять читать => включаем флаг "после чтения будет запись"
                aioOp->isReading = 1;

                // Корректируем размер, если следующая порция выходит за предел файла
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

    // По окончании операции уведомляем ждущий поток
    gCv.notify_all();
}

// Инициализация набора операций
void initOperations(struct AioOperation* operations, int cluster) {
    for (int i = 0; i < operationsCount; i++) {
        operations[i].aio = (struct aiocb*)calloc(1, sizeof(struct aiocb));
        memset(operations[i].aio, 0, sizeof(*(operations[i].aio)));

        operations[i].buffer = (char*)calloc(cluster, sizeof(char));

        // Настраиваем сигнальное событие: по окончании вызовется aioCompletionHandler
        operations[i].aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
        operations[i].aio->aio_sigevent.sigev_notify_function = aioCompletionHandler;
        // Передаем в колбэк указатель на текущую структуру операции
        operations[i].aio->aio_sigevent.sigev_value.sival_ptr = &operations[i];

        // Количество байт, которое будем читать или писать
        if (cluster > fileSize) {
            operations[i].aio->aio_nbytes = fileSize;
        } else {
            operations[i].aio->aio_nbytes = cluster;
        }

        // Начальные настройки для чтения
        operations[i].aio->aio_fildes = fileReadHandler;
        operations[i].aio->aio_offset = cluster * i;
        operations[i].aio->aio_buf = operations[i].buffer;

        // Считаем операцию активной и начинаем с чтения
        operations[i].isWorking = 1;
        operations[i].isReading = 1;
    }
}

// Запуск всех операций
void startOperations(struct AioOperation* operations) {
    // Изначально запускаем "чтение" для каждого элемента массива
    for(int i = 0; i < operationsCount; i++) {
        // Если смещение уже за пределами файла, операцию не стартуем
        if (operations[i].aio->aio_offset >= fileSize) {
            operations[i].isWorking = 0;
        } else {
            // Корректируем размер, если кусок выходит за границы файла
            if (operations[i].aio->aio_offset + chunk > fileSize) {
                operations[i].aio->aio_nbytes = fileSize - operations[i].aio->aio_offset;
            }

            // Стартуем асинхронное чтение
            if (aio_read(operations[i].aio) == -1) {
                cerr << "Ошибка: Асинхронное чтение невозможно!" << endl;
                operations[i].isWorking = 0;
            }
        }
    }

    // Дожидаемся завершения всех операций
    await(operations, operationsCount);
}

// Ожидание завершения всех aio-операций
void await(struct AioOperation* operations, int count) {
    unique_lock<mutex> lock(gMutex);

    auto allDone = [&](){
        for (int i = 0; i < count; i++) {
            if (operations[i].isWorking != 0) {
                return false;
            }
        }
        return true;
    };

    gCv.wait(lock, allDone);
}
