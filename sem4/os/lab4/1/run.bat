@echo off
setlocal enableextensions

REM === Создание директорий для логов ===
if not exist writers_logs (
    mkdir writers_logs
)
if not exist readers_logs (
    mkdir readers_logs
)

echo Starting writers...
for /L %%i in (0,1,100) do (
    start "" /B writer.exe %%i
)

REM — Ждём пару секунд, чтобы писатели успели создать shared memory
timeout /t 2 >nul

echo Starting readers...
for /L %%i in (0,1,1) do (
    start "" /B reader.exe %%i
)

echo All processes launched.
endlocal
