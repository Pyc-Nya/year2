:: run_measurements.bat
@echo off
setlocal

:: Удаляем старый лог
if exist result.log del result.log

:: Компиляция
gcc time.c -o app.exe
if ERRORLEVEL 1 (
    echo Compilation error.
    exit /b 1
)

:: Запуск для разных чисел потоков
for %%T in (1 2 4 8 12 16 20 30 40 60 80 100 150 200 300 400 500 600 700 800 900 1000) do (
    echo Running with %%T threads...
    for /L %%I in (1,1,10) do (
        app.exe %%T >> result.log
    )
)

echo result.log is ready
endlocal
