@echo off

..\x64\release\wavegen.exe %1.json %1.wav
if "%errorlevel%" NEQ "0" goto failed

lame %1.wav
if "%errorlevel%" NEQ "0" goto failed

erase %1.wav


echo Success
goto endhere

:failed
echo Failed

:endhere