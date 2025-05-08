@echo off
cls

if not exist misc mkdir misc
if not exist binTempWorker mkdir binTempWorker
if not exist outputWorker mkdir outputWorker

set output=binTempWorker\worker.exe
set input=src\worker.cpp
set arguments=-std=c++17

g++ %input% -o %output% %arguments% 2> errorWorker.txt > nul
if %errorlevel% neq 0 (
    echo caught error:
    type errorWorker.txt
    echo saved to errorWorker.txt.
    pause
    exit
)

echo compiled successfully!

if exist output rmdir /S /Q output > nul

mkdir outputWorker\

xcopy /E /Y misc outputWorker\ > nul
copy .\%output% outputWorker\ > nul

rmdir /S /Q binTempWorker

pause
exit
