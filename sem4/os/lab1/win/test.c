#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

void showLogicalDrives();
void showLogicalDriveStrings();
void showDriveTypes();
void showVolumeInformation();
void showDiskFreeSpace();
void createDirectory();
void removeDirectory();
void ls();
void createFile();
void clearStdIn();
void copyFile();
void moveFile();
DWORD getFileAttributes(char filePath[MAX_PATH]);
void setFileAttributes();
void getFileInformationByHandle();
void printFileTime(const FILETIME* ft);
void getFileTime();
void setFileTime();

int main() {
    int choice = -1;
	char filePath[MAX_PATH];

    do {
        printf("=== Menu ===\n");
        printf("0. Exit\n");
        printf("1. Show Logical Drives\n");
		printf("2. Show Logical Drive Strings\n");
		printf("3. Show Drive Types\n");
		printf("4. Get Volume Information\n");
		printf("5. Get Disk Free Space\n");
		printf("6. Create Directory\n");
		printf("7. Remove Directory\n");
		printf("8. ls\n");
		printf("9. Create File\n");
		printf("10. Copy File\n");
		printf("11. Move File or Directory\n");
		printf("12. Get File Attributes\n");
		printf("13. Set File Attributes\n");
		printf("14. Get File Information by Handle\n");
		printf("15. Get File Time\n");
		printf("16. Set File Time\n");
        printf("Select option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Incorrect input! Please try again.\n");
			clearStdIn();
            continue;
        }
		clearStdIn();
		printf("\n");

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
				createDirectory();
				break;
			case 7:
				removeDirectory();
				break;
			case 8:
				ls();
				break;
			case 9:
				createFile();
				break;
			case 10:
				copyFile();
				break;
			case 11:
				moveFile();
				break;
			case 12:
				getFileAttributes(filePath);
				break;
			case 13:
				setFileAttributes();
				break;
			case 14:
				getFileInformationByHandle();
				break;
			case 15:
				getFileTime();
				break;
			case 16:
				setFileTime();
				break;
            default:
                printf("Incorrect input! Please try again.\n");
        }
		if (choice != 0) {
			printf("\nPress any key to continue...");
			getchar();
			system("cls");
		}
    } while (choice != 0);

    return 0;
}

void showLogicalDrives() {
    DWORD drives = GetLogicalDrives();

    if (drives == 0) {
        printf("Error retrieving logical drives.\n");
        return;
    }

    printf("Available Logical Drives:\n");
    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            printf("%c\n", 'A' + i);
        }
    }
}

void showLogicalDriveStrings() {
    char drives[256];
    DWORD length = GetLogicalDriveStrings(sizeof(drives), drives);

    if (length == 0) {
        printf("Error retrieving logical drive strings.\n");
        return;
    }

    printf("Available Logical Drives:\n");
    for (char* drive = drives; *drive; drive += strlen(drive) + 1) {
        printf("%s\n", drive);
    }
}

void showDriveTypes() {
    char drives[256];
    DWORD length = GetLogicalDriveStrings(sizeof(drives), drives);

    if (length == 0) {
        printf("Error retrieving logical drive strings.\n");
        return;
    }

    printf("Drive Types:\n");
    for (char* drive = drives; *drive; drive += strlen(drive) + 1) {
        UINT type = GetDriveType(drive);

        printf("%s - ", drive);

        switch (type) {
            case DRIVE_UNKNOWN:
                printf("Unknown\n");
                break;
            case DRIVE_NO_ROOT_DIR:
                printf("Invalid Root Path\n");
                break;
            case DRIVE_REMOVABLE:
                printf("Removable (USB, Floppy, etc.)\n");
                break;
            case DRIVE_FIXED:
                printf("Fixed (HDD, SSD)\n");
                break;
            case DRIVE_REMOTE:
                printf("Network Drive\n");
                break;
            case DRIVE_CDROM:
                printf("CD/DVD Drive\n");
                break;
            case DRIVE_RAMDISK:
                printf("RAM Disk\n");
                break;
            default:
                printf("Unrecognized Type\n");
        }
    }
}

void showVolumeInformation() {
    char drive[4];

    printf("Enter drive letter (e.g., C): ");
    scanf(" %c", &drive[0]); 
    clearStdIn();

    drive[1] = ':';
    drive[2] = '\\';
    drive[3] = '\0'; 

    char volumeName[MAX_PATH] = {0};
    DWORD serialNumber = 0;
    DWORD maxComponentLength = 0;
    DWORD fileSystemFlags = 0;
    char fileSystemName[MAX_PATH] = {0};

    if (GetVolumeInformation(drive, volumeName, sizeof(volumeName),
                             &serialNumber, &maxComponentLength,
                             &fileSystemFlags, fileSystemName, sizeof(fileSystemName))) {
        printf("\nVolume Information for %s\n", drive);
        printf("Volume Name: %s\n", volumeName[0] ? volumeName : "N/A");
        printf("Serial Number: %08X\n", serialNumber);
        printf("Max File Name Length: %lu\n", maxComponentLength);
        printf("File System: %s\n", fileSystemName);
        printf("Flags: %lu\n", fileSystemFlags);

        // Расшифровка флагов fileSystemFlags
        printf("File System Flags:\n");
        if (fileSystemFlags & FILE_CASE_SENSITIVE_SEARCH)
            printf("  - Case-sensitive file names\n");
        if (fileSystemFlags & FILE_CASE_PRESERVED_NAMES)
            printf("  - Case-preserved file names\n");
        if (fileSystemFlags & FILE_UNICODE_ON_DISK)
            printf("  - Supports Unicode in file names\n");
        if (fileSystemFlags & FILE_PERSISTENT_ACLS)
            printf("  - Supports persistent ACLs\n");
        if (fileSystemFlags & FILE_FILE_COMPRESSION)
            printf("  - Supports file compression\n");
        if (fileSystemFlags & FILE_VOLUME_QUOTAS)
            printf("  - Supports disk quotas\n");
        if (fileSystemFlags & FILE_SUPPORTS_SPARSE_FILES)
            printf("  - Supports sparse files\n");
        if (fileSystemFlags & FILE_SUPPORTS_REPARSE_POINTS)
            printf("  - Supports reparse points (e.g., symbolic links, junctions)\n");
        if (fileSystemFlags & FILE_SUPPORTS_REMOTE_STORAGE)
            printf("  - Supports remote storage\n");
        if (fileSystemFlags & FILE_VOLUME_IS_COMPRESSED)
            printf("  - Volume is compressed\n");
        if (fileSystemFlags & FILE_SUPPORTS_OBJECT_IDS)
            printf("  - Supports object identifiers\n");
        if (fileSystemFlags & FILE_SUPPORTS_ENCRYPTION)
            printf("  - Supports encryption\n");
        if (fileSystemFlags & FILE_NAMED_STREAMS)
            printf("  - Supports named streams\n");
        if (fileSystemFlags & FILE_READ_ONLY_VOLUME)
            printf("  - Read-only volume\n");
        if (fileSystemFlags & FILE_SEQUENTIAL_WRITE_ONCE)
            printf("  - Sequential write-once volume\n");
        if (fileSystemFlags & FILE_SUPPORTS_TRANSACTIONS)
            printf("  - Supports transactions\n");
        if (fileSystemFlags & FILE_SUPPORTS_HARD_LINKS)
            printf("  - Supports hard links\n");
        if (fileSystemFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
            printf("  - Supports extended attributes (EAs)\n");
        if (fileSystemFlags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
            printf("  - Supports opening files by FileID\n");
        if (fileSystemFlags & FILE_SUPPORTS_USN_JOURNAL)
            printf("  - Supports USN Journal\n");
    } else {
        printf("Failed to retrieve volume information. Error: %lu\n", GetLastError());
    }
}

void showDiskFreeSpace() {
    char drive[4];

    printf("Enter drive letter (e.g., C): ");
    scanf(" %1s", drive);  
	clearStdIn();

    drive[1] = ':';
    drive[2] = '\\';
    drive[3] = '\0';  

    DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;

    if (GetDiskFreeSpace(drive, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters)) {
        DWORDLONG clusterSize = (DWORDLONG)sectorsPerCluster * bytesPerSector;
        DWORDLONG freeSpace = (DWORDLONG)freeClusters * clusterSize;
        DWORDLONG totalSpace = (DWORDLONG)totalClusters * clusterSize;

        printf("\nDisk Free Space for %s\n", drive);
        printf("Sectors per Cluster: %lu\n", sectorsPerCluster);
        printf("Bytes per Sector: %lu\n", bytesPerSector);
        printf("Total Clusters: %lu\n", totalClusters);
        printf("Free Clusters: %lu\n", freeClusters);
        printf("Cluster Size: %llu bytes\n", clusterSize);
        printf("Total Space: %.2f GB\n", (double)totalSpace / (1024 * 1024 * 1024));
        printf("Free Space: %.2f GB\n", (double)freeSpace / (1024 * 1024 * 1024));
    } else {
        printf("Failed to retrieve disk space information. Error: %lu\n", GetLastError());
    }
}

void createDirectory() {
    char dirName[MAX_PATH];

    while (1) {
        printf("\nEnter directory name (e.g., C:\\NewFolder): ");
        scanf(" %[^\n]", dirName);
        clearStdIn();

        if (CreateDirectory(dirName, NULL)) {
            printf("Directory created successfully: %s\n", dirName);
            return;
        } else {
            DWORD error = GetLastError();

            if (error == ERROR_ALREADY_EXISTS) {
                printf("Error: Directory already exists.\n");
                printf("1. Keep existing directory\n");
                printf("2. Enter a new directory path\n");
                printf("3. Delete existing directory and create a new one\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();

                if (choice == 1) {
                    return;
                } else if (choice == 2) {
                    continue;
                } else if (choice == 3) {
                    if (RemoveDirectory(dirName)) {
                        printf("Old directory removed. Creating new directory...\n");
                        CreateDirectory(dirName, NULL);
                        printf("Directory created successfully: %s\n", dirName);
                    } else {
                        printf("Failed to remove existing directory. Error code: %lu\n", GetLastError());
                    }
                    return;
                }
            } else if (error == ERROR_PATH_NOT_FOUND) {
                printf("Error: Parent directory does not exist.\n");
                printf("1. Create missing parent directories\n");
                printf("2. Enter a new directory path\n");
                printf("3. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();

                if (choice == 1) {
                    char parentDir[MAX_PATH];
                    snprintf(parentDir, MAX_PATH, "%s", dirName);
                    for (int i = strlen(parentDir) - 1; i >= 0; i--) {
                        if (parentDir[i] == '\\') {
                            parentDir[i] = '\0';
                            break;
                        }
                    }
                    if (CreateDirectory(parentDir, NULL)) {
                        printf("Parent directory created: %s\n", parentDir);
                        continue;
                    } else {
                        printf("Failed to create parent directory. Error code: %lu\n", GetLastError());
                    }
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }
            } else {
                printf("Failed to create directory. Error code: %lu\n", error);
                return;
            }
        }
    }
}

void removeDirectory() {
    char dirName[MAX_PATH];

    while (1) {
        printf("\nEnter directory name to remove (e.g., C:\\TestDir): ");
        scanf(" %[^\n]", dirName);
        clearStdIn();

        if (RemoveDirectory(dirName)) {
            printf("Directory removed successfully: %s\n", dirName);
            return;
        } else {
            DWORD error = GetLastError();

            if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
                printf("Error: Directory does not exist.\n");
                printf("1. Enter a new directory path\n");
                printf("2. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    continue;
                } else {
                    return;
                }

            } else if (error == ERROR_DIR_NOT_EMPTY) {
                printf("Error: Directory is not empty.\n");
                printf("1. Delete all contents and remove the directory\n");
                printf("2. Enter a new directory path\n");
                printf("3. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    char delCmd[MAX_PATH];
                    snprintf(delCmd, MAX_PATH, "rmdir /s /q \"%s\"", dirName);
                    system(delCmd);
                    if (RemoveDirectory(dirName)) {
                        printf("Directory removed successfully: %s\n", dirName);
                    } else {
                        printf("Failed to remove directory. Error code: %lu\n", GetLastError());
                    }
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }

            } else if (error == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied.\n");
                printf("1. Retry with administrator privileges\n");
                printf("2. Enter a new directory path\n");
                printf("3. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    printf("Try running the program as Administrator.\n");
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }

            } else {
                printf("Failed to remove directory. Error code: %lu\n", error);
                return;
            }
        }
    }
}

void ls() {
    char dirPath[MAX_PATH];

    printf("Enter directory path (e.g., C:\\TestDir): ");
    scanf(" %[^\n]", dirPath); 
	clearStdIn();

    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", dirPath);

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Cannot open directory or directory does not exist.\n");
        return;
    }

    printf("\nSubdirectories in %s:\n", dirPath);

	do {
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                printf("  [DIR]  %s\n", findData.cFileName); 
            } else {
                printf("  [FILE] %s\n", findData.cFileName);
            }
        }
    } while (FindNextFile(hFind, &findData));   

    FindClose(hFind);
}

void createFile() {
    char filePath[MAX_PATH];

    while (1) {
        printf("\nEnter file path (e.g., C:\\TestFolder\\TestFile.txt): ");
        scanf(" %[^\n]", filePath);
        clearStdIn();

        HANDLE hFile = CreateFile(
            filePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();

            if (error == ERROR_FILE_EXISTS) {
                printf("Error: File already exists.\n");
                printf("1. Overwrite file\n");
                printf("2. Enter a new file path\n");
                printf("3. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();

                if (choice == 1) {
                    hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (hFile == INVALID_HANDLE_VALUE) {
                        printf("Failed to overwrite file. Error code: %lu\n", GetLastError());
                    } else {
                        printf("File overwritten successfully: %s\n", filePath);
                        CloseHandle(hFile);
                    }
                    return;
                } else if (choice == 2) {
                    continue; 
                } else {
                    return;
                }

            } else if (error == ERROR_PATH_NOT_FOUND) {
                printf("Error: Invalid path or directory does not exist.\n");
                printf("1. Create the missing directory\n");
                printf("2. Enter a new file path\n");
                printf("3. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();

                if (choice == 1) {
                    char dirPath[MAX_PATH];
                    snprintf(dirPath, MAX_PATH, "%s", filePath);
                    for (int i = strlen(dirPath) - 1; i >= 0; i--) {
                        if (dirPath[i] == '\\') {
                            dirPath[i] = '\0';
                            break;
                        }
                    }
                    if (CreateDirectory(dirPath, NULL)) {
                        printf("Directory created: %s\n", dirPath);
                    } else {
                        printf("Failed to create directory. Error code: %lu\n", GetLastError());
                    }
                    continue; 
                } else if (choice == 2) {
                    continue; 
                } else {
                    return; 
                }
            } else {
                printf("Failed to create file. Error code: %lu\n", error);
                return;
            }
        } else {
            printf("File created successfully: %s\n", filePath);
            CloseHandle(hFile);
            return;
        }
    }
}

void clearStdIn() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

void copyFile() {
    char sourcePath[MAX_PATH];
    char destinationPath[MAX_PATH];

    while (1) {
        printf("\nEnter source file path (e.g., C:\\source.txt): ");
        scanf(" %[^\n]", sourcePath);
        clearStdIn();

        printf("Enter destination file path (e.g., C:\\TestFolder\\backup.txt): ");
        scanf(" %[^\n]", destinationPath);
        clearStdIn();

        if (CopyFile(sourcePath, destinationPath, FALSE)) {
            printf("File copied successfully to %s\n", destinationPath);
            return;
        } else {
            DWORD error = GetLastError();

            if (error == ERROR_FILE_NOT_FOUND) {
                printf("Error: Source file does not exist.\n");
                printf("1. Enter a new source file path\n");
                printf("2. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    continue; 
                } else {
                    return; 
                }

            } else if (error == ERROR_PATH_NOT_FOUND) {
                printf("Error: Destination path is invalid.\n");
                printf("1. Enter a new destination file path\n");
                printf("2. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    continue; 
                } else {
                    return;
                }

            } else if (error == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied. Check permissions.\n");
                printf("1. Retry with a different file path\n");
                printf("2. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    continue; 
                } else {
                    return;
                }

            } else if (error == ERROR_ALREADY_EXISTS) {
                printf("Error: Destination file already exists.\n");
                printf("1. Overwrite file\n");
                printf("2. Enter a new destination file path\n");
                printf("3. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    if (CopyFile(sourcePath, destinationPath, TRUE)) {
                        printf("File overwritten successfully: %s\n", destinationPath);
                    } else {
                        printf("Failed to overwrite file. Error code: %lu\n", GetLastError());
                    }
                    return;
                } else if (choice == 2) {
                    continue; 
                } else {
                    return; 
                }

            } else {
                printf("Failed to copy file. Error code: %lu\n", error);
                return;
            }
        }
    }
}

void moveFile() {
    char sourcePath[MAX_PATH];
    char destinationPath[MAX_PATH];

    while (1) {
        printf("\nEnter source file or directory path (e.g., C:\\source.txt or C:\\OldFolder): ");
        scanf(" %[^\n]", sourcePath);
        clearStdIn();

        printf("Enter destination path (e.g., C:\\TestFolder\\destination.txt or C:\\NewFolder): ");
        scanf(" %[^\n]", destinationPath);
        clearStdIn();

        if (MoveFile(sourcePath, destinationPath)) {
            printf("File or directory moved successfully to %s\n", destinationPath);
            return;
        } else {
            DWORD error = GetLastError();

            if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
                printf("Error: Source file or directory does not exist.\n");
                printf("1. Enter a new source path\n");
                printf("2. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    continue;
                } else {
                    return;
                }

            } else if (error == ERROR_ALREADY_EXISTS) {
                printf("Error: A file or directory with the same name already exists at the destination.\n");
                printf("1. Overwrite the existing file/directory\n");
                printf("2. Enter a new destination path\n");
                printf("3. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    if (MoveFileEx(sourcePath, destinationPath, MOVEFILE_REPLACE_EXISTING)) {
                        printf("File or directory successfully moved (overwritten): %s\n", destinationPath);
                    } else {
                        printf("Failed to overwrite. Error code: %lu\n", GetLastError());
                    }
                    return;
                } else if (choice == 2) {
                    continue;
                } else {
                    return;
                }

            } else if (error == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied. Check permissions or try running as Administrator.\n");
                printf("1. Retry with a different path\n");
                printf("2. Cancel\n");
                printf("Choose an option: ");

                int choice;
                scanf("%d", &choice);
                clearStdIn();
                if (choice == 1) {
                    continue;
                } else {
                    return;
                }

            } else {
                printf("Failed to move file or directory. Error code: %lu\n", error);
                return;
            }
        }
    }
}

DWORD getFileAttributes(char filePath[MAX_PATH]) {

    while (1) {
        printf("\nEnter file or directory path (e.g., C:\\file.txt or C:\\MyFolder): ");
        scanf(" %[^\n]", filePath);
        clearStdIn();

        DWORD attributes = GetFileAttributes(filePath);

        if (attributes == INVALID_FILE_ATTRIBUTES) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                printf("Error: The file does not exist.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                printf("Error: The specified path is invalid.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied.\n");
            } else {
                printf("Failed to retrieve attributes. Error code: %lu\n", error);
            }

            printf("1. Enter a new file path\n");
            printf("2. Cancel\n");
            printf("Choose an option: ");

            int choice;
            scanf("%d", &choice);
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return 0;
            }
        }

        printf("\nAttributes of %s:\n", filePath);
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) printf("  - Directory\n");
        if (attributes & FILE_ATTRIBUTE_READONLY) printf("  - Read-only\n");
        if (attributes & FILE_ATTRIBUTE_HIDDEN) printf("  - Hidden\n");
        if (attributes & FILE_ATTRIBUTE_SYSTEM) printf("  - System file\n");
        if (attributes & FILE_ATTRIBUTE_ARCHIVE) printf("  - Archive\n");
        if (attributes & FILE_ATTRIBUTE_ENCRYPTED) printf("  - Encrypted\n");
        if (attributes & FILE_ATTRIBUTE_COMPRESSED) printf("  - Compressed\n");
        if (attributes & FILE_ATTRIBUTE_TEMPORARY) printf("  - Temporary file\n");
        if (attributes & FILE_ATTRIBUTE_OFFLINE) printf("  - Offline storage\n");

        return attributes;
    }
}

void setFileAttributes() {
    char filePath[MAX_PATH];

    while (1) {
        DWORD attributes = getFileAttributes(filePath);

        printf("\nChoose action:\n");
        printf("1. Add an attribute\n");
        printf("2. Remove an attribute\n");
        printf("3. Cancel\n");
        printf("Choose an option: ");

        int action;
        scanf("%d", &action);
        clearStdIn();

        if (action == 3) return;

        printf("\nSelect attribute:\n");
        printf("1. Read-only\n");
        printf("2. Hidden\n");
        printf("3. System file\n");
        printf("4. Archive\n");
        printf("Choose an option: ");

        int attrChoice;
        scanf("%d", &attrChoice);
        clearStdIn();

        DWORD attrFlag = 0;
        if (attrChoice == 1) attrFlag = FILE_ATTRIBUTE_READONLY;
        else if (attrChoice == 2) attrFlag = FILE_ATTRIBUTE_HIDDEN;
        else if (attrChoice == 3) attrFlag = FILE_ATTRIBUTE_SYSTEM;
        else if (attrChoice == 4) attrFlag = FILE_ATTRIBUTE_ARCHIVE;
        else {
            printf("Invalid choice.\n");
            return;
        }

        if (action == 1) {
            attributes |= attrFlag;
        } else if (action == 2) {
            attributes &= ~attrFlag;
        }

        if (SetFileAttributes(filePath, attributes)) {
            printf("File attributes updated successfully.\n");
        } else {
            printf("Failed to update attributes. Error code: %lu\n", GetLastError());
        }
        return;
    }
}

void printFileTime(const FILETIME* ft) {
    SYSTEMTIME st;
    FileTimeToSystemTime(ft, &st);
    printf("%04d-%02d-%02d %02d:%02d:%02d\n",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);
}

void getFileInformationByHandle() {
    char filePath[MAX_PATH];

    while (1) {
        printf("\nEnter file path (e.g., C:\\file.txt): ");
        scanf(" %[^\n]", filePath);
        clearStdIn();

        HANDLE hFile = CreateFile(
            filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                printf("Error: The file does not exist.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                printf("Error: The specified path is invalid.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied.\n");
            } else {
                printf("Failed to open file. Error code: %lu\n", error);
            }

            printf("1. Enter a new file path\n");
            printf("2. Cancel\n");
            printf("Choose an option: ");

            int choice;
            scanf("%d", &choice);
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return;
            }
        }

        BY_HANDLE_FILE_INFORMATION fileInfo;
        if (GetFileInformationByHandle(hFile, &fileInfo)) {
            printf("\nFile Information for %s:\n", filePath);
            printf("Volume Serial Number: %lu\n", fileInfo.dwVolumeSerialNumber);
            printf("File Index: %lu-%lu\n", fileInfo.nFileIndexHigh, fileInfo.nFileIndexLow);
            printf("File Size: %llu bytes\n", ((unsigned long long)fileInfo.nFileSizeHigh << 32) | fileInfo.nFileSizeLow);
            printf("Created: "); printFileTime(&fileInfo.ftCreationTime);
            printf("Last Accessed: "); printFileTime(&fileInfo.ftLastAccessTime);
            printf("Last Modified: "); printFileTime(&fileInfo.ftLastWriteTime);

            printf("Attributes:\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) printf("  - Directory\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY) printf("  - Read-only\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) printf("  - Hidden\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) printf("  - System file\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) printf("  - Archive\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) printf("  - Compressed\n");
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) printf("  - Encrypted\n");
        } else {
            printf("Failed to retrieve file information. Error code: %lu\n", GetLastError());
        }

        CloseHandle(hFile);
        return;
    }
}

void getFileTime() {
    char filePath[MAX_PATH];

    while (1) {
        printf("\nEnter file path (e.g., C:\\file.txt): ");
        scanf(" %[^\n]", filePath);
        clearStdIn();

        HANDLE hFile = CreateFile(
            filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                printf("Error: The file does not exist.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                printf("Error: The specified path is invalid.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied.\n");
            } else {
                printf("Failed to open file. Error code: %lu\n", error);
            }

            printf("1. Enter a new file path\n");
            printf("2. Cancel\n");
            printf("Choose an option: ");

            int choice;
            scanf("%d", &choice);
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return;
            }
        }

        FILETIME creationTime, accessTime, writeTime;
        if (GetFileTime(hFile, &creationTime, &accessTime, &writeTime)) {

            printf("\nFile Time for %s:\n", filePath);
            printf("Created: "); printFileTime(&creationTime);
            printf("Last Accessed: "); printFileTime(&accessTime);
            printf("Last Modified: "); printFileTime(&writeTime);
        } else {
            printf("Failed to retrieve file time. Error code: %lu\n", GetLastError());
        }

        CloseHandle(hFile);
        return;
    }
}

void setFileTime() {
    char filePath[MAX_PATH];

    while (1) {
        printf("\nEnter file path (e.g., C:\\file.txt): ");
        scanf(" %[^\n]", filePath);
        clearStdIn();

        HANDLE hFile = CreateFile(
            filePath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                printf("Error: The file does not exist.\n");
            } else if (error == ERROR_PATH_NOT_FOUND) {
                printf("Error: The specified path is invalid.\n");
            } else if (error == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied.\n");
            } else {
                printf("Failed to open file. Error code: %lu\n", error);
            }

            printf("1. Enter a new file path\n");
            printf("2. Cancel\n");
            printf("Choose an option: ");

            int choice;
            scanf("%d", &choice);
            clearStdIn();
            if (choice == 1) {
                continue;
            } else {
                return;
            }
        }

        FILETIME newTime;
        SYSTEMTIME st;
        
        printf("\nEnter new time (YYYY MM DD HH MM SS): ");
        scanf("%hu %hu %hu %hu %hu %hu", 
              &st.wYear, &st.wMonth, &st.wDay, 
              &st.wHour, &st.wMinute, &st.wSecond);
        clearStdIn();

        if (!SystemTimeToFileTime(&st, &newTime)) {
            printf("Invalid date format.\n");
            CloseHandle(hFile);
            return;
        }

        if (SetFileTime(hFile, &newTime, &newTime, &newTime)) {
            printf("File time updated successfully.\n");
        } else {
            printf("Failed to update file time. Error code: %lu\n", GetLastError());
        }

        CloseHandle(hFile);
        return;
    }
}
