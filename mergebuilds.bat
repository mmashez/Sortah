@echo off
cls

if exist merge rmdir /S /Q merge

mkdir merge

xcopy /E /Y output merge
xcopy /E /Y outputWorker merge

pause
exit