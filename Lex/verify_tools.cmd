@echo off
echo === where gcc / g++ (PATH) ===
where gcc 2>nul
if errorlevel 1 echo gcc: NOT FOUND
where g++ 2>nul
if errorlevel 1 echo g++: NOT FOUND
echo.
echo === gcc --version ===
gcc --version 2>nul
echo.
echo === g++ --version ===
g++ --version 2>nul
echo.
echo === default target (expect *mingw* / *msys*) ===
gcc -dumpmachine 2>nul
echo.
echo If both gcc and g++ run and show MinGW/MSYS2, the toolchain is basically OK.
echo The symbol_lookup error was from mixing: lex.yy.o=C gcc, ytab_runtime=g++ on .c file.
