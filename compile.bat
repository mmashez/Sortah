@echo off
cls

if not exist misc mkdir misc
if not exist binTemp mkdir binTemp

set output=binTemp\main.exe
set input=src\main.cpp
set arguments=-std=c++17

g++ %input% -o %output% %arguments% 2> error.txt > nul
if %errorlevel% neq 0 (
    echo caught error:
    type error.txt
    echo saved to error.txt.
    pause
    exit
)

echo compiled successfully!

if exist output rmdir /S /Q output > nul

if not exist output mkdir output\

xcopy /E /Y misc output\
copy .\%output% output\

rmdir /S /Q binTemp

pause
exit
