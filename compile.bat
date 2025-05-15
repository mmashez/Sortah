@echo off
setlocal enabledelayedexpansion
cls

set misc=misc
set exe=app.exe
set worker_exe=worker.exe

set build=main
set output_folder=output
set full_output=.\%output_folder%\%build%\%exe%

set include=include
set source_folder=src

set target=main.cpp
set input=%source_folder%\%target%
set arguments=-std=c++17 -I.\%include%

set worker_target=worker.cpp
set worker_input=%source_folder%\%worker_target%
set worker_arguments=%arguments%

set log=%output_folder%\%build%\main.log
set worker_log=%output_folder%\%build%\worker.log

set "crucial_files=config.json NOTE.txt icon.ico"

:: New customizable icon path (relative to script folder)
set icon_file=%misc%\icon.ico

set title=sortah compiler! @mmashez

:main
call :init
call :ensure
call :prompt
call :check_files
call :prepare_icon
call :compile
call :finalize
goto :done

:init
title %title%
exit /b

:ensure
if not exist %output_folder% mkdir %output_folder%
if not exist %output_folder%\%build% mkdir %output_folder%\%build%
exit /b

:check_files
for %%f in (%crucial_files%) do (
    if not exist "%misc%\%%f" (
        echo crucial file "%misc%\%%f" missing! exiting..
        pause
        exit /b
    )
)
exit /b

:prompt
set /p clean=clean the output folder? (y/n): 
if /i "%clean%"=="y" (
    call :clean
) else if /i "%clean%"=="n" (
    echo skipping cleaning process...
) else (
    echo invalid input! assuming "n".
)
exit /b

:clean
echo cleaning output folder
del /q "%output_folder%\%build%\*.*" >nul 2>&1
exit /b

:prepare_icon
echo generating icon resource from "%icon_file%"...
echo IDI_APP_ICON ICON "%icon_file%" > resource.rc
windres resource.rc -O coff -o resource.o > nul
if errorlevel 1 (
    echo failed to compile resource.rc with icon!
    del resource.rc >nul 2>&1
    pause
    exit /b
)
del resource.rc >nul 2>&1

exit /b

:compile
echo compiling main app...
g++ %arguments% %input% resource.o -o %full_output% > "%log%" 2>&1
if errorlevel 1 (
    echo compilation failed! see %log% for details.
    pause
    exit /b
)
echo compilation succeeded! >> "%log%"

echo compiling worker...
g++ %worker_arguments% %worker_input% resource.o -o %output_folder%\%build%\%worker_exe% > "%worker_log%" 2>&1
if errorlevel 1 (
    echo compilation of the worker failed! see %worker_log% for details.
    pause
    exit /b
)
echo compilation succeeded! >> "%worker_log%"
exit /b

:finalize
echo copying misc files...
xcopy /s /e /q "%misc%" "%output_folder%\%build%" > nul

del resource.o >nul 2>&1

exit /b

:done
echo press any key to exit...
pause > nul
exit /b
