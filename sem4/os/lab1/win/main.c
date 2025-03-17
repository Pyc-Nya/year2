#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>

// Убедимся, что используем UNICODE-версии WinAPI
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

void showLogicalDrives();
void showLogicalDriveStrings();
void showDriveTypes();
void showVolumeInformation();
void showDiskFreeSpace();
void createDirectoryWFunc();
void removeDirectoryWFunc();
void ls();
void createFileWFunc();
void clearStdIn();
void copyFileWFunc();
void moveFileWFunc();
DWORD getFileAttributesWFunc(wchar_t filePath[MAX_PATH]);
void setFileAttributesWFunc();
void getFileInformationByHandle();
void printFileTimeW(const FILETIME* ft);
void getFileTimeWFunc();
void setFileTimeWFunc();

int wmain() 
{
    // Устанавливаем локаль (зависит от ОС), чтобы корректно работали wprintf, wscanf и т.п.
    setlocale(LC_ALL, "");
    // Переключаем потоки ввода/вывода в режим wide-символов (UTF-16)
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin),  _O_U16TEXT);
    // Опционально меняем кодовые страницы консоли для лучшей совместимости
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    int choice = -1;
    wchar_t filePath[MAX_PATH];

    do {
        wprintf(L"=== Меню ===\n");
        wprintf(L"0. Выход\n");
        wprintf(L"1. Показать логические диски (битовая маска)\n");
        wprintf(L"2. Показать логические диски (строки)\n");
        wprintf(L"3. Показать типы дисков\n");
        wprintf(L"4. Информация о томах (GetVolumeInformation)\n");
        wprintf(L"5. Свободное место на диске (GetDiskFreeSpace)\n");
        wprintf(L"6. Создать директорию\n");
        wprintf(L"7. Удалить директорию\n");
        wprintf(L"8. ls\n");
        wprintf(L"9. Создать файл\n");
        wprintf(L"10. Копировать файл\n");
        wprintf(L"11. Переместить файл или директорию\n");
        wprintf(L"12. GetFileAttributes\n");
        wprintf(L"13. SetFileAttributes\n");
        wprintf(L"14. GetFileInformationByHandle\n");
        wprintf(L"15. GetFileTime\n");
        wprintf(L"16. SetFileTime\n");
        wprintf(L"Выберите пункт: ");

        if (wscanf(L"%d", &choice) != 1) {
            wprintf(L"Неверный ввод! Повторите попытку.\n");
            clearStdIn();
            continue;
        }
        clearStdIn();
        wprintf(L"\n");

        switch (choice) {
            case 0:
                break;
            case 1:
                showLogicalDrives();
                break;
            case 2:
                showLogicalDriveStrings();
                break;
            case 3:
                showDriveTypes();
                break;
            case 4:
                showVolumeInformation();
                break;
            case 5:
                showDiskFreeSpace();
                break;
            case 6:
                createDirectoryWFunc();
                break;
            case 7:
                removeDirectoryWFunc();
                break;
            case 8:
                ls();
                break;
            case 9:
                createFileWFunc();
                break;
            case 10:
                copyFileWFunc();
                break;
            case 11:
                moveFileWFunc();
                break;
            case 12:
                getFileAttributesWFunc(filePath);
                break;
            case 13:
                setFileAttributesWFunc();
                break;
            case 14:
                getFileInformationByHandle();
                break;
            case 15:
                getFileTimeWFunc();
                break;
            case 16:
                setFileTimeWFunc();
                break;
            default:
                wprintf(L"Неверный ввод! Повторите попытку.\n");
        }

        if (choice != 0) {
            wprintf(L"\nНажмите Enter для продолжения...");
            clearStdIn();
            getchar();
            _wsystem(L"cls");
        }
    } while (choice != 0);

    return 0;
}

// -----------------------------------------

void showLogicalDrives() 
{
    DWORD drives = GetLogicalDrives();

    if (drives == 0) {
        wprintf(L"Ошибка при получении логических дисков.\n");
        return;
    }

    wprintf(L"Логические диски (битовая маска):\n");
    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            wprintf(L"%c:\n", L'A' + i);
        }
    }
}

void showLogicalDriveStrings() 
{
    wchar_t drives[256];
    DWORD length = GetLogicalDriveStringsW(256, drives);

    if (length == 0) {
        wprintf(L"Ошибка при получении строк логических дисков.\n");
        return;
    }

    wprintf(L"Логические диски (строки):\n");
    for (wchar_t *drive = drives; *drive; drive += wcslen(drive) + 1) {
        wprintf(L"%s\n", drive);
    }
}

void showDriveTypes() 
{
    wchar_t drives[256];
    DWORD length = GetLogicalDriveStringsW(256, drives);

    if (length == 0) {
        wprintf(L"Ошибка при получении строк логических дисков.\n");
        return;
    }

    wprintf(L"Типы дисков:\n");
    for (wchar_t *drive = drives; *drive; drive += wcslen(drive) + 1) {
        UINT type = GetDriveTypeW(drive);
        wprintf(L"%s - ", drive);

        switch (type) {
            case DRIVE_UNKNOWN:
                wprintf(L"Неизвестный\n");
                break;
            case DRIVE_NO_ROOT_DIR:
                wprintf(L"Неверный корневой путь\n");
                break;
            case DRIVE_REMOVABLE:
                wprintf(L"Съемный (USB, Floppy и т.п.)\n");
                break;
            case DRIVE_FIXED:
                wprintf(L"Жесткий диск (HDD, SSD)\n");
                break;
            case DRIVE_REMOTE:
                wprintf(L"Сетевой диск\n");
                break;
            case DRIVE_CDROM:
                wprintf(L"CD/DVD привод\n");
                break;
            case DRIVE_RAMDISK:
                wprintf(L"RAM-диск\n");
                break;
            default:
                wprintf(L"Неизвестный тип\n");
        }
    }
}

void showVolumeInformation() 
{
    wchar_t driveLetter;
    wprintf(L"Введите букву диска (напр. C): ");
    if (wscanf(L" %c", &driveLetter) != 1) {
        wprintf(L"Некорректный ввод.\n");
        clearStdIn();
        return;
    }
    clearStdIn();

    wchar_t drive[4];
    drive[0] = driveLetter;
    drive[1] = L':';
    drive[2] = L'\\';
    drive[3] = L'\0';

    wchar_t volumeName[MAX_PATH] = {0};
    DWORD serialNumber = 0;
    DWORD maxComponentLength = 0;
    DWORD fileSystemFlags = 0;
    wchar_t fileSystemName[MAX_PATH] = {0};

    if (GetVolumeInformationW(
            drive, volumeName, MAX_PATH,
            &serialNumber, &maxComponentLength,
            &fileSystemFlags, fileSystemName, MAX_PATH
        )) 
    {
        wprintf(L"\nИнформация о томе для %s\n", drive);
        wprintf(L"Имя тома: %s\n", volumeName[0] ? volumeName : L"—");
        wprintf(L"Серийный номер: %08X\n", serialNumber);
        wprintf(L"Макс. длина имени файла: %lu\n", maxComponentLength);
        wprintf(L"Файловая система: %s\n", fileSystemName);
        wprintf(L"Флаги: %lu\n", fileSystemFlags);

        wprintf(L"Расшифровка флагов:\n");
        if (fileSystemFlags & FILE_CASE_SENSITIVE_SEARCH)
            wprintf(L"  - Чувствительность к регистру\n");
        if (fileSystemFlags & FILE_CASE_PRESERVED_NAMES)
            wprintf(L"  - Сохранение регистра в именах\n");
        if (fileSystemFlags & FILE_UNICODE_ON_DISK)
            wprintf(L"  - Поддержка Unicode в именах\n");
        if (fileSystemFlags & FILE_PERSISTENT_ACLS)
            wprintf(L"  - Постоянные ACL\n");
        if (fileSystemFlags & FILE_FILE_COMPRESSION)
            wprintf(L"  - Сжатие файлов\n");
        if (fileSystemFlags & FILE_VOLUME_QUOTAS)
            wprintf(L"  - Квоты диска\n");
        if (fileSystemFlags & FILE_SUPPORTS_SPARSE_FILES)
            wprintf(L"  - Разреженные файлы (Sparse)\n");
        if (fileSystemFlags & FILE_SUPPORTS_REPARSE_POINTS)
            wprintf(L"  - Символьные ссылки, Reparse Points\n");
        if (fileSystemFlags & FILE_SUPPORTS_REMOTE_STORAGE)
            wprintf(L"  - Удаленное хранение\n");
        if (fileSystemFlags & FILE_VOLUME_IS_COMPRESSED)
            wprintf(L"  - Том сжат\n");
        if (fileSystemFlags & FILE_SUPPORTS_OBJECT_IDS)
            wprintf(L"  - Идентификаторы объектов\n");
        if (fileSystemFlags & FILE_SUPPORTS_ENCRYPTION)
            wprintf(L"  - Шифрование (EFS)\n");
        if (fileSystemFlags & FILE_NAMED_STREAMS)
            wprintf(L"  - Альтернативные потоки данных\n");
        if (fileSystemFlags & FILE_READ_ONLY_VOLUME)
            wprintf(L"  - Том только для чтения\n");
        if (fileSystemFlags & FILE_SEQUENTIAL_WRITE_ONCE)
            wprintf(L"  - Однократная последовательная запись\n");
        if (fileSystemFlags & FILE_SUPPORTS_TRANSACTIONS)
            wprintf(L"  - Транзакционная ФС\n");
        if (fileSystemFlags & FILE_SUPPORTS_HARD_LINKS)
            wprintf(L"  - Жесткие ссылки\n");
        if (fileSystemFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
            wprintf(L"  - Расширенные атрибуты\n");
        if (fileSystemFlags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
            wprintf(L"  - Открытие по FileID\n");
        if (fileSystemFlags & FILE_SUPPORTS_USN_JOURNAL)
            wprintf(L"  - Журнал USN\n");
    } else {
        wprintf(L"Не удалось получить информацию о томе. Код ошибки: %lu\n", GetLastError());
    }
}

void showDiskFreeSpace() 
{
    wchar_t driveLetter;
    wprintf(L"Введите букву диска (напр. C): ");
    if (wscanf(L" %c", &driveLetter) != 1) {
        wprintf(L"Некорректный ввод.\n");
        clearStdIn();
        return;
    }
    clearStdIn();

    wchar_t drive[4];
    drive[0] = driveLetter;
    drive[1] = L':';
    drive[2] = L'\\';
    drive[3] = L'\0';

    DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;
    if (GetDiskFreeSpaceW(drive, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters)) {
        unsigned long long clusterSize = (unsigned long long)sectorsPerCluster * bytesPerSector;
        unsigned long long freeSpace   = (unsigned long long)freeClusters     * clusterSize;
        unsigned long long totalSpace  = (unsigned long long)totalClusters    * clusterSize;

        wprintf(L"\nСводка по %s\n", drive);
        wprintf(L"Секторов в кластере: %lu\n", sectorsPerCluster);
        wprintf(L"Байт в секторе: %lu\n", bytesPerSector);
        wprintf(L"Всего кластеров: %lu\n", totalClusters);
        wprintf(L"Свободных кластеров: %lu\n", freeClusters);
        wprintf(L"Размер кластера: %llu байт\n", clusterSize);
        wprintf(L"Объем диска: %.2f ГБ\n", (double)totalSpace / (1024.0 * 1024.0 * 1024.0));
        wprintf(L"Свободно: %.2f ГБ\n", (double)freeSpace / (1024.0 * 1024.0 * 1024.0));
    } else {
        wprintf(L"Не удалось получить информацию о свободном пространстве. Код ошибки: %lu\n", GetLastError());
    }
}

void createDirectoryWFunc() 
{
    while (1) {
        wchar_t dirName[MAX_PATH];
        wprintf(L"\nВведите имя директории (напр. C:\\НоваяПапка): ");
        if (!fgetws(dirName, MAX_PATH, stdin)) {
            wprintf(L"Ошибка ввода.\n");
            return;
        }
        // Удалим перевод строки, если он есть
        size_t len = wcslen(dirName);
        if (len > 0 && dirName[len - 1] == L'\n') {
            dirName[len - 1] = L'\0';
        }

        if (CreateDirectoryW(dirName, NULL)) {
            wprintf(L"Директория успешно создана: %s\n", dirName);
            return;
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_ALREADY_EXISTS) {
                wprintf(L"Ошибка: Директория уже существует.\n");
                wprintf(L"1. Оставить как есть\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Удалить существующую и создать новую\n");
                wprintf(L"Выберите опцию: ");

                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    return;
                } else if (choice == 2) {
                    continue;
                } else if (choice == 3) {
                    if (RemoveDirectoryW(dirName)) {
                        wprintf(L"Старая директория удалена. Создаем новую...\n");
                        if (CreateDirectoryW(dirName, NULL)) {
                            wprintf(L"Директория успешно создана: %s\n", dirName);
                        } else {
                            wprintf(L"Не удалось создать директорию. Код ошибки: %lu\n", GetLastError());
                        }
                    } else {
                        wprintf(L"Не удалось удалить старую директорию. Код ошибки: %lu\n", GetLastError());
                    }
                    return;
                }
            } else if (error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Родительская директория не существует.\n");
                wprintf(L"1. Создать недостающие родительские директории\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Отмена\n");
                wprintf(L"Выберите опцию: ");

                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    // Пытаемся создать родительскую директорию
                    wchar_t parentDir[MAX_PATH];
                    wcscpy_s(parentDir, MAX_PATH, dirName);
                    // Ищем последний слеш, чтобы отсечь имя конечной папки
                    for (int i = (int)wcslen(parentDir) - 1; i >= 0; i--) {
                        if (parentDir[i] == L'\\') {
                            parentDir[i] = L'\0';
                            break;
                        }
                    }
                    if (CreateDirectoryW(parentDir, NULL)) {
                        wprintf(L"Родительская директория создана: %s\n", parentDir);
                        // И снова пробуем создать конечную
                        continue;
                    } else {
                        wprintf(L"Не удалось создать родительскую директорию. Код ошибки: %lu\n", GetLastError());
                    }
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }
            } else {
                wprintf(L"Не удалось создать директорию. Код ошибки: %lu\n", error);
                return;
            }
        }
    }
}

void removeDirectoryWFunc() 
{
    while (1) {
        wchar_t dirName[MAX_PATH];
        wprintf(L"\nВведите имя директории для удаления (напр. C:\\Тест): ");
        if (!fgetws(dirName, MAX_PATH, stdin)) {
            wprintf(L"Ошибка ввода.\n");
            return;
        }
        // Удалим перевод строки
        size_t len = wcslen(dirName);
        if (len > 0 && dirName[len - 1] == L'\n') {
            dirName[len - 1] = L'\0';
        }

        if (RemoveDirectoryW(dirName)) {
            wprintf(L"Директория удалена: %s\n", dirName);
            return;
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Директория не существует.\n");
                wprintf(L"1. Ввести новый путь\n");
                wprintf(L"2. Отмена\n");
                wprintf(L"Выберите опцию: ");

                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    continue;
                } else {
                    return;
                }

            } else if (error == ERROR_DIR_NOT_EMPTY) {
                wprintf(L"Ошибка: Директория не пуста.\n");
                wprintf(L"1. Удалить все содержимое и директорию\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Отмена\n");
                wprintf(L"Выберите опцию: ");

                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    // Удаляем рекурсивно
                    wchar_t cmd[MAX_PATH + 50];
                    // Используем rmdir /s /q
                    swprintf(cmd, MAX_PATH + 50, L"rmdir /s /q \"%s\"", dirName);
                    _wsystem(cmd);
                    if (RemoveDirectoryW(dirName)) {
                        wprintf(L"Директория удалена: %s\n", dirName);
                    } else {
                        wprintf(L"Не удалось удалить директорию. Код ошибки: %lu\n", GetLastError());
                    }
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }

            } else if (error == ERROR_ACCESS_DENIED) {
                wprintf(L"Ошибка: Отказано в доступе.\n");
                wprintf(L"1. Повторить с правами администратора\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Отмена\n");
                wprintf(L"Выберите опцию: ");

                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    wprintf(L"Перезапустите программу от имени администратора.\n");
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }

            } else {
                wprintf(L"Не удалось удалить директорию. Код ошибки: %lu\n", error);
                return;
            }
        }
    }
}

void ls() 
{
    wchar_t dirPath[MAX_PATH];
    wprintf(L"Введите путь к директории (напр. C:\\Тест): ");
    if (!fgetws(dirPath, MAX_PATH, stdin)) {
        wprintf(L"Ошибка ввода.\n");
        return;
    }
    // Удалим перевод строки
    size_t len = wcslen(dirPath);
    if (len > 0 && dirPath[len - 1] == L'\n') {
        dirPath[len - 1] = L'\0';
    }

    wchar_t searchPath[MAX_PATH];
    swprintf(searchPath, MAX_PATH, L"%s\\*", dirPath);

    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        wprintf(L"Ошибка: Невозможно открыть директорию или она не существует.\n");
        return;
    }

    wprintf(L"\nСодержимое %s:\n", dirPath);
    do {
        if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                wprintf(L"  [DIR]  %s\n", findData.cFileName);
            } else {
                wprintf(L"  [FILE] %s\n", findData.cFileName);
            }
        }
    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);
}

void createFileWFunc() 
{
    while (1) {
        wchar_t filePath[MAX_PATH];
        wprintf(L"\nВведите путь к файлу (напр. C:\\Test\\Файл.txt): ");
        if (!fgetws(filePath, MAX_PATH, stdin)) {
            wprintf(L"Ошибка ввода.\n");
            return;
        }
        size_t len = wcslen(filePath);
        if (len > 0 && filePath[len - 1] == L'\n') {
            filePath[len - 1] = L'\0';
        }

        HANDLE hFile = CreateFileW(
            filePath, GENERIC_WRITE, 0, NULL,
            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_EXISTS) {
                wprintf(L"Ошибка: Файл уже существует.\n");
                wprintf(L"1. Перезаписать файл\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    hFile = CreateFileW(
                        filePath, GENERIC_WRITE, 0, NULL,
                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
                    );
                    if (hFile == INVALID_HANDLE_VALUE) {
                        wprintf(L"Не удалось перезаписать файл. Код ошибки: %lu\n", GetLastError());
                    } else {
                        wprintf(L"Файл перезаписан: %s\n", filePath);
                        CloseHandle(hFile);
                    }
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }
            } else if (error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Неверный путь или директория не существует.\n");
                wprintf(L"1. Создать недостающую директорию\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    // Отсекаем имя файла, пытаемся создать каталог
                    wchar_t dirPath[MAX_PATH];
                    wcscpy_s(dirPath, MAX_PATH, filePath);
                    for (int i = (int)wcslen(dirPath) - 1; i >= 0; i--) {
                        if (dirPath[i] == L'\\') {
                            dirPath[i] = L'\0';
                            break;
                        }
                    }
                    if (CreateDirectoryW(dirPath, NULL)) {
                        wprintf(L"Директория создана: %s\n", dirPath);
                    } else {
                        wprintf(L"Не удалось создать директорию. Код ошибки: %lu\n", GetLastError());
                    }
                    continue;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }
            } else {
                wprintf(L"Не удалось создать файл. Код ошибки: %lu\n", error);
                return;
            }
        } else {
            wprintf(L"Файл успешно создан: %s\n", filePath);
            CloseHandle(hFile);
            return;
        }
    }
}

void clearStdIn() 
{
    // Считываем символы до перевода строки
    int c;
    while ((c = getwchar()) != L'\n' && c != WEOF) {}
}

void copyFileWFunc() 
{
    while (1) {
        wchar_t sourcePath[MAX_PATH];
        wchar_t destinationPath[MAX_PATH];

        wprintf(L"\nВведите исходный файл (напр. C:\\source.txt): ");
        if (!fgetws(sourcePath, MAX_PATH, stdin)) return;
        size_t len = wcslen(sourcePath);
        if (len > 0 && sourcePath[len - 1] == L'\n') {
            sourcePath[len - 1] = L'\0';
        }

        wprintf(L"Введите путь для копии (напр. C:\\backup.txt): ");
        if (!fgetws(destinationPath, MAX_PATH, stdin)) return;
        len = wcslen(destinationPath);
        if (len > 0 && destinationPath[len - 1] == L'\n') {
            destinationPath[len - 1] = L'\0';
        }

        if (CopyFileW(sourcePath, destinationPath, FALSE)) {
            wprintf(L"Файл скопирован: %s\n", destinationPath);
            return;
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                wprintf(L"Ошибка: Исходный файл не существует.\n");
                wprintf(L"1. Ввести новый исходный файл\n");
                wprintf(L"2. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) continue;
                else return;
            } else if (error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Неверный путь назначения.\n");
                wprintf(L"1. Ввести новый путь\n");
                wprintf(L"2. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) continue;
                else return;
            } else if (error == ERROR_ACCESS_DENIED) {
                wprintf(L"Ошибка: Доступ запрещен.\n");
                wprintf(L"1. Повторить с другим путем\n");
                wprintf(L"2. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) continue;
                else return;
            } else if (error == ERROR_ALREADY_EXISTS) {
                wprintf(L"Ошибка: Файл назначения уже существует.\n");
                wprintf(L"1. Перезаписать\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    // Попытка перезаписать
                    if (CopyFileW(sourcePath, destinationPath, TRUE)) {
                        wprintf(L"Файл перезаписан: %s\n", destinationPath);
                    } else {
                        wprintf(L"Не удалось перезаписать файл. Код ошибки: %lu\n", GetLastError());
                    }
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }
            } else {
                wprintf(L"Не удалось скопировать файл. Код ошибки: %lu\n", error);
                return;
            }
        }
    }
}

void moveFileWFunc() 
{
    while (1) {
        wchar_t sourcePath[MAX_PATH];
        wchar_t destinationPath[MAX_PATH];

        wprintf(L"\nИсходный файл/папка (напр. C:\\source.txt): ");
        if (!fgetws(sourcePath, MAX_PATH, stdin)) return;
        size_t len = wcslen(sourcePath);
        if (len > 0 && sourcePath[len - 1] == L'\n') {
            sourcePath[len - 1] = L'\0';
        }

        wprintf(L"Путь назначения (напр. C:\\destination.txt): ");
        if (!fgetws(destinationPath, MAX_PATH, stdin)) return;
        len = wcslen(destinationPath);
        if (len > 0 && destinationPath[len - 1] == L'\n') {
            destinationPath[len - 1] = L'\0';
        }

        if (MoveFileW(sourcePath, destinationPath)) {
            wprintf(L"Успешно перемещено в %s\n", destinationPath);
            return;
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Исходный файл/папка не существует.\n");
                wprintf(L"1. Ввести заново\n");
                wprintf(L"2. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) continue;
                else return;
            } else if (error == ERROR_ALREADY_EXISTS) {
                wprintf(L"Ошибка: Уже есть элемент с таким именем в папке назначения.\n");
                wprintf(L"1. Заменить\n");
                wprintf(L"2. Ввести новый путь\n");
                wprintf(L"3. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) {
                    if (MoveFileExW(sourcePath, destinationPath, MOVEFILE_REPLACE_EXISTING)) {
                        wprintf(L"Успешно перемещено (заменено): %s\n", destinationPath);
                    } else {
                        wprintf(L"Не удалось заменить. Код ошибки: %lu\n", GetLastError());
                    }
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }
            } else if (error == ERROR_ACCESS_DENIED) {
                wprintf(L"Ошибка: Нет доступа.\n");
                wprintf(L"1. Повторить с другим путем\n");
                wprintf(L"2. Отмена\n");
                int choice;
                if (wscanf(L"%d", &choice) != 1) {
                    clearStdIn();
                    return;
                }
                clearStdIn();
                if (choice == 1) continue;
                else return;
            } else {
                wprintf(L"Не удалось переместить. Код ошибки: %lu\n", error);
                return;
            }
        }
    }
}

DWORD getFileAttributesWFunc(wchar_t filePath[MAX_PATH]) 
{
    while (1) {
        wprintf(L"\nВведите путь к файлу или директории: ");
        if (!fgetws(filePath, MAX_PATH, stdin)) {
            wprintf(L"Ошибка ввода.\n");
            return 0;
        }
        size_t len = wcslen(filePath);
        if (len > 0 && filePath[len - 1] == L'\n') {
            filePath[len - 1] = L'\0';
        }

        DWORD attributes = GetFileAttributesW(filePath);
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                wprintf(L"Ошибка: Файл не существует.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Неверный путь.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                wprintf(L"Ошибка: Нет доступа.\n");
            } else {
                wprintf(L"Не удалось получить атрибуты. Код: %lu\n", error);
            }

            wprintf(L"1. Ввести путь заново\n");
            wprintf(L"2. Отмена\n");
            int choice;
            if (wscanf(L"%d", &choice) != 1) {
                clearStdIn();
                return 0;
            }
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return 0;
            }
        }

        wprintf(L"\nАтрибуты %s:\n", filePath);
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) wprintf(L"  - Директория\n");
        if (attributes & FILE_ATTRIBUTE_READONLY)  wprintf(L"  - Только чтение\n");
        if (attributes & FILE_ATTRIBUTE_HIDDEN)    wprintf(L"  - Скрытый\n");
        if (attributes & FILE_ATTRIBUTE_SYSTEM)    wprintf(L"  - Системный\n");
        if (attributes & FILE_ATTRIBUTE_ARCHIVE)   wprintf(L"  - Архив\n");
        if (attributes & FILE_ATTRIBUTE_ENCRYPTED) wprintf(L"  - Зашифрован\n");
        if (attributes & FILE_ATTRIBUTE_COMPRESSED)wprintf(L"  - Сжат\n");
        if (attributes & FILE_ATTRIBUTE_TEMPORARY) wprintf(L"  - Временный\n");
        if (attributes & FILE_ATTRIBUTE_OFFLINE)   wprintf(L"  - Оффлайн\n");

        return attributes;
    }
}

void setFileAttributesWFunc() 
{
    wchar_t filePath[MAX_PATH];

    while (1) {
        DWORD attributes = getFileAttributesWFunc(filePath);
        if (attributes == 0) return; // Пользователь прервал

        wprintf(L"\nВыберите действие:\n");
        wprintf(L"1. Добавить атрибут\n");
        wprintf(L"2. Удалить атрибут\n");
        wprintf(L"3. Отмена\n");
        wprintf(L"Ваш выбор: ");

        int action;
        if (wscanf(L"%d", &action) != 1) {
            clearStdIn();
            return;
        }
        clearStdIn();

        if (action == 3) return;

        wprintf(L"\nВыберите атрибут:\n");
        wprintf(L"1. Read-only\n");
        wprintf(L"2. Hidden\n");
        wprintf(L"3. System\n");
        wprintf(L"4. Archive\n");
        wprintf(L"Ваш выбор: ");

        int attrChoice;
        if (wscanf(L"%d", &attrChoice) != 1) {
            clearStdIn();
            return;
        }
        clearStdIn();

        DWORD attrFlag = 0;
        if      (attrChoice == 1) attrFlag = FILE_ATTRIBUTE_READONLY;
        else if (attrChoice == 2) attrFlag = FILE_ATTRIBUTE_HIDDEN;
        else if (attrChoice == 3) attrFlag = FILE_ATTRIBUTE_SYSTEM;
        else if (attrChoice == 4) attrFlag = FILE_ATTRIBUTE_ARCHIVE;
        else {
            wprintf(L"Неверный выбор.\n");
            return;
        }

        if (action == 1) {
            attributes |= attrFlag;   // Добавляем флаг
        } else if (action == 2) {
            attributes &= ~attrFlag;  // Снимаем флаг
        }

        if (SetFileAttributesW(filePath, attributes)) {
            wprintf(L"Атрибуты успешно обновлены.\n");
        } else {
            wprintf(L"Не удалось обновить атрибуты. Код ошибки: %lu\n", GetLastError());
        }
        return;
    }
}

void printFileTimeW(const FILETIME* ft) 
{
    SYSTEMTIME st;
    FileTimeToSystemTime(ft, &st);
    wprintf(L"%04d-%02d-%02d %02d:%02d:%02d\n",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond
    );
}

void getFileInformationByHandle() 
{
    while (1) {
        wchar_t filePath[MAX_PATH];
        wprintf(L"\nВведите путь к файлу (напр. C:\\file.txt): ");
        if (!fgetws(filePath, MAX_PATH, stdin)) {
            wprintf(L"Ошибка ввода.\n");
            return;
        }
        size_t len = wcslen(filePath);
        if (len > 0 && filePath[len - 1] == L'\n') {
            filePath[len - 1] = L'\0';
        }

        HANDLE hFile = CreateFileW(
            filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                wprintf(L"Ошибка: Файл не существует.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Неверный путь.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                wprintf(L"Ошибка: Нет доступа.\n");
            } else {
                wprintf(L"Ошибка при открытии файла. Код: %lu\n", error);
            }

            wprintf(L"1. Ввести новый путь\n");
            wprintf(L"2. Отмена\n");
            int choice;
            if (wscanf(L"%d", &choice) != 1) {
                clearStdIn();
                return;
            }
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return;
            }
        }

        BY_HANDLE_FILE_INFORMATION fileInfo;
        if (GetFileInformationByHandle(hFile, &fileInfo)) {
            wprintf(L"\nИнформация о файле %s:\n", filePath);
            wprintf(L"Серийный номер тома: %lu\n", fileInfo.dwVolumeSerialNumber);
            wprintf(L"Индекс файла: %lu-%lu\n", fileInfo.nFileIndexHigh, fileInfo.nFileIndexLow);
            unsigned long long size = ((unsigned long long)fileInfo.nFileSizeHigh << 32) | fileInfo.nFileSizeLow;
            wprintf(L"Размер: %llu байт\n", size);

            wprintf(L"Создан: ");       printFileTimeW(&fileInfo.ftCreationTime);
            wprintf(L"Последний доступ: ");   printFileTimeW(&fileInfo.ftLastAccessTime);
            wprintf(L"Последняя запись: ");   printFileTimeW(&fileInfo.ftLastWriteTime);

            wprintf(L"Атрибуты:\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) wprintf(L"  - Директория\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY)  wprintf(L"  - Только чтение\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)    wprintf(L"  - Скрытый\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)    wprintf(L"  - Системный\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)   wprintf(L"  - Архив\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)wprintf(L"  - Сжат\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) wprintf(L"  - Зашифрован\n");
        } else {
            wprintf(L"Не удалось получить информацию о файле. Код ошибки: %lu\n", GetLastError());
        }

        CloseHandle(hFile);
        return;
    }
}

void getFileTimeWFunc() 
{
    while (1) {
        wchar_t filePath[MAX_PATH];
        wprintf(L"\nВведите путь к файлу: ");
        if (!fgetws(filePath, MAX_PATH, stdin)) {
            wprintf(L"Ошибка ввода.\n");
            return;
        }
        size_t len = wcslen(filePath);
        if (len > 0 && filePath[len - 1] == L'\n') {
            filePath[len - 1] = L'\0';
        }

        HANDLE hFile = CreateFileW(
            filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                wprintf(L"Ошибка: Файл не существует.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Неверный путь.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                wprintf(L"Ошибка: Нет доступа.\n");
            } else {
                wprintf(L"Ошибка при открытии файла. Код: %lu\n", error);
            }

            wprintf(L"1. Ввести новый путь\n");
            wprintf(L"2. Отмена\n");
            int choice;
            if (wscanf(L"%d", &choice) != 1) {
                clearStdIn();
                return;
            }
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return;
            }
        }

        FILETIME creationTime, accessTime, writeTime;
        if (GetFileTime(hFile, &creationTime, &accessTime, &writeTime)) {
            wprintf(L"\nВременные метки для %s:\n", filePath);
            wprintf(L"Создан:          "); printFileTimeW(&creationTime);
            wprintf(L"Последний доступ: "); printFileTimeW(&accessTime);
            wprintf(L"Последняя запись: "); printFileTimeW(&writeTime);
        } else {
            wprintf(L"Не удалось получить время файла. Код: %lu\n", GetLastError());
        }

        CloseHandle(hFile);
        return;
    }
}

void setFileTimeWFunc() 
{
    while (1) {
        wchar_t filePath[MAX_PATH];
        wprintf(L"\nВведите путь к файлу: ");
        if (!fgetws(filePath, MAX_PATH, stdin)) {
            wprintf(L"Ошибка ввода.\n");
            return;
        }
        size_t len = wcslen(filePath);
        if (len > 0 && filePath[len - 1] == L'\n') {
            filePath[len - 1] = L'\0';
        }

        HANDLE hFile = CreateFileW(
            filePath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                wprintf(L"Ошибка: Файл не существует.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                wprintf(L"Ошибка: Неверный путь.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                wprintf(L"Ошибка: Нет доступа.\n");
            } else {
                wprintf(L"Ошибка при открытии файла. Код: %lu\n", error);
            }

            wprintf(L"1. Ввести новый путь\n");
            wprintf(L"2. Отмена\n");
            int choice;
            if (wscanf(L"%d", &choice) != 1) {
                clearStdIn();
                return;
            }
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return;
            }
        }

        wprintf(L"\nВведите новые дату и время (ГГГГ ММ ДД чч мм cc): ");
        SYSTEMTIME st;
        if (wscanf(L"%hu %hu %hu %hu %hu %hu",
            &st.wYear, &st.wMonth, &st.wDay,
            &st.wHour, &st.wMinute, &st.wSecond) != 6) 
        {
            wprintf(L"Неверный формат даты.\n");
            clearStdIn();
            CloseHandle(hFile);
            return;
        }
        clearStdIn();

        FILETIME newTime;
        if (!SystemTimeToFileTime(&st, &newTime)) {
            wprintf(L"Неверная дата.\n");
            CloseHandle(hFile);
            return;
        }

        if (SetFileTime(hFile, &newTime, &newTime, &newTime)) {
            wprintf(L"Время файла успешно обновлено.\n");
        } else {
            wprintf(L"Ошибка при обновлении времени. Код: %lu\n", GetLastError());
        }

        CloseHandle(hFile);
        return;
    }
}
