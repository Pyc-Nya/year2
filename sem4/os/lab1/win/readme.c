#include <windows.h>

/**
 * Retrieves a bitmask representing available logical drives.
 * Each bit corresponds to a drive letter (A-Z).
 *
 * @return DWORD - Bitmask of available logical drives. 
 *         If 0, an error occurred (use GetLastError() for details).
 */
DWORD GetLogicalDrives(void);

/**
 * Retrieves a list of available logical drives as null-terminated strings.
 *
 * @param nBufferLength The size of the buffer in characters.
 * @param lpBuffer A buffer to store the drive strings.
 * @return DWORD - Number of characters written to the buffer (excluding null terminator). 
 *         If 0, an error occurred (use GetLastError()).
 */
DWORD GetLogicalDriveStrings(DWORD nBufferLength, LPSTR lpBuffer);

/**
 * Determines the type of a drive (e.g., fixed, removable, network).
 *
 * @param lpRootPathName The root directory of the drive (e.g., "C:\\").
 * @return UINT - One of the following:
 *         DRIVE_UNKNOWN (0) - Type cannot be determined.
 *         DRIVE_NO_ROOT_DIR (1) - Invalid path.
 *         DRIVE_REMOVABLE (2) - USB, Floppy, etc.
 *         DRIVE_FIXED (3) - HDD, SSD.
 *         DRIVE_REMOTE (4) - Network drive.
 *         DRIVE_CDROM (5) - Optical drive.
 *         DRIVE_RAMDISK (6) - RAM disk.
 */
UINT GetDriveType(LPCSTR lpRootPathName);

/**
 * Retrieves information about a volume, such as its file system and serial number.
 *
 * @param lpRootPathName The root directory of the volume (e.g., "C:\\").
 * @param lpVolumeNameBuffer Buffer for the volume name.
 * @param nVolumeNameSize Size of the volume name buffer.
 * @param lpVolumeSerialNumber Pointer to receive the volume serial number.
 * @param lpMaximumComponentLength Pointer to receive the max file name length.
 * @param lpFileSystemFlags Pointer to receive file system flags.
 * @param lpFileSystemNameBuffer Buffer for the file system name.
 * @param nFileSystemNameSize Size of the file system name buffer.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL GetVolumeInformation(
    LPCSTR lpRootPathName,
    LPSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize
);

/**
 * Retrieves disk space information, including cluster size and free space.
 *
 * @param lpRootPathName The root directory of the volume (e.g., "C:\\").
 * @param lpSectorsPerCluster Pointer to receive sectors per cluster.
 * @param lpBytesPerSector Pointer to receive bytes per sector.
 * @param lpNumberOfFreeClusters Pointer to receive free clusters.
 * @param lpTotalNumberOfClusters Pointer to receive total clusters.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL GetDiskFreeSpace(
    LPCSTR lpRootPathName,
    LPDWORD lpSectorsPerCluster,
    LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters,
    LPDWORD lpTotalNumberOfClusters
);

/**
 * Creates a new directory.
 *
 * @param lpPathName The path of the directory to create.
 * @param lpSecurityAttributes Security attributes (NULL for default).
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL CreateDirectory(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

/**
 * Removes an empty directory.
 *
 * @param lpPathName The path of the directory to remove.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL RemoveDirectory(LPCSTR lpPathName);

/**
 * Copies an existing file to a new location.
 *
 * @param lpExistingFileName The source file path.
 * @param lpNewFileName The destination file path.
 * @param bFailIfExists If TRUE, the function fails if the destination file exists.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL CopyFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);

/**
 * Moves or renames a file or directory.
 *
 * @param lpExistingFileName The source file or directory.
 * @param lpNewFileName The destination file or directory.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL MoveFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);

/**
 * Retrieves file attributes (read-only, hidden, system, etc.).
 *
 * @param lpFileName The path to the file or directory.
 * @return DWORD - File attributes bitmask. If INVALID_FILE_ATTRIBUTES (-1), an error occurred.
 */
DWORD GetFileAttributes(LPCSTR lpFileName);

/**
 * Sets file attributes (e.g., hidden, read-only, system).
 *
 * @param lpFileName The path to the file or directory.
 * @param dwFileAttributes The attributes to set.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL SetFileAttributes(LPCSTR lpFileName, DWORD dwFileAttributes);

/**
 * Retrieves detailed file information, including file size, creation time, and index.
 *
 * @param hFile A handle to the file (must be opened first).
 * @param lpFileInformation Pointer to a structure to receive file information.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL GetFileInformationByHandle(HANDLE hFile, LPBY_HANDLE_FILE_INFORMATION lpFileInformation);

/**
 * Retrieves the creation, last access, and last write times of a file.
 *
 * @param hFile A handle to the file (must be opened first).
 * @param lpCreationTime Pointer to receive the file creation time.
 * @param lpLastAccessTime Pointer to receive the last access time.
 * @param lpLastWriteTime Pointer to receive the last write time.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL GetFileTime(
    HANDLE hFile,
    LPFILETIME lpCreationTime,
    LPFILETIME lpLastAccessTime,
    LPFILETIME lpLastWriteTime
);

/**
 * Sets the creation, last access, and last write times of a file.
 *
 * @param hFile A handle to the file (must be opened with write access).
 * @param lpCreationTime Pointer to the new creation time (NULL to keep unchanged).
 * @param lpLastAccessTime Pointer to the new last access time (NULL to keep unchanged).
 * @param lpLastWriteTime Pointer to the new last write time (NULL to keep unchanged).
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL SetFileTime(
    HANDLE hFile,
    const FILETIME *lpCreationTime,
    const FILETIME *lpLastAccessTime,
    const FILETIME *lpLastWriteTime
);

/**
 * Converts a FILETIME structure (100-nanosecond intervals since 1601) to local system time.
 *
 * @param lpFileTime The input FILETIME structure.
 * @param lpLocalFileTime Pointer to receive the local FILETIME.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime);

/**
 * Converts a FILETIME structure to a SYSTEMTIME structure (human-readable format).
 *
 * @param lpFileTime The input FILETIME structure.
 * @param lpSystemTime Pointer to receive the converted SYSTEMTIME.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL FileTimeToSystemTime(const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime);

/**
 * Converts a SYSTEMTIME structure (YYYY-MM-DD HH:MM:SS) to a FILETIME structure.
 *
 * @param lpSystemTime The input SYSTEMTIME structure.
 * @param lpFileTime Pointer to receive the converted FILETIME.
 * @return BOOL - TRUE if successful, FALSE if an error occurred.
 */
BOOL SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime);

