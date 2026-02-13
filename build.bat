@echo off
setlocal

set build=bin
set bin=pamlc.exe
set src=src\main.c

if exist "%build%" rmdir /s /q "%build%"
mkdir "%build%"

echo compiling %src% -> %build%\%bin%

cl ^
    /nologo ^
    /std:c11 ^
    /W4 ^
    /Zi ^
    /Fo"%build%\\" ^
    /Fd"%build%\vc.pdb" ^
    /Fe"%build%\%bin%" ^
    %src%

endlocal

