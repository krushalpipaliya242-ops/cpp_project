@echo off
REM Windows build script (needs MinGW-w64 g++ on PATH)
g++ -std=c++17 -Wall -Wextra -O2 -o Executable\Project.exe Source\*.cpp
if %errorlevel% neq 0 ( echo BUILD FAILED & exit /b 1 )
echo Built Executable\Project.exe
echo Run it from inside the Executable folder:  cd Executable ^&^& Project.exe
