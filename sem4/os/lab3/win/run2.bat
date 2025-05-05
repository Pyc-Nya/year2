@echo off
setlocal

:: Удаляем старый лог
if exist result2.log del result2.log


:: Запуск с разным числом потоков
for %%T in (1 2 4 8 12 16 20 22 24 26 30 32 34 36 38 40 50 60) do (
    echo Running with %%T threads...
    for /L %%I in (1,1,10) do (
        open_mp.exe %%T
    )
)

echo result2.log is ready
endlocal