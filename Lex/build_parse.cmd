@echo off
setlocal EnableExtensions
cd /d "%~dp0"

REM lex.yy.c and ytab_runtime.c MUST be compiled as C (gcc), not g++,
REM or C symbols (symbol_lookup, yylex) will not link with lex.yy.o.

where gcc >nul 2>&1
if errorlevel 1 goto NO_GCC

gcc -std=c11 -Wall -Wextra -Wno-unused-function -c -I. -Iinclude lex.yy.c -o lex.yy.o
if errorlevel 1 exit /b 1

gcc -std=c11 -Wall -Wextra -c -I. -Iinclude ytab_runtime.c -o ytab_runtime.o
if errorlevel 1 exit /b 1

set "CXXFLAGS=-std=c++17 -Wall -Wextra -O0 -g"
set "CXXOBJ=parser\grammar.cpp parser\seu_lr1_parser.cpp ast.cpp ytab_runtime.o lex.yy.o parser\c99_parse_main.cpp"

g++ %CXXFLAGS% -I. -Iinclude %CXXOBJ% -o c99_parse.exe
if errorlevel 1 exit /b 1

g++ %CXXFLAGS% -I. -Iinclude parser\c99_ir_main.cpp semantic\ir_from_ast.cpp parser\grammar.cpp parser\seu_lr1_parser.cpp ast.cpp ytab_runtime.o lex.yy.o -o c99_ir.exe
if errorlevel 1 exit /b 1

g++ %CXXFLAGS% -o yacc_subset_scan.exe tools\yacc_subset_scan.cpp
if errorlevel 1 exit /b 1

echo === Lexer smoke: test_input.c ===
gcc -std=c11 -Wall -Wextra -I. -Iinclude -o test_lexer.exe test_lexer.c ytab_runtime.o lex.yy.o
if errorlevel 1 exit /b 1
test_lexer.exe test_input.c
echo.
echo === Parser: parser\test_syntax_ok.c ===
c99_parse.exe parser\test_syntax_ok.c
echo.
echo === IR (three-address skeleton): parser\test_syntax_ok.c ===
c99_ir.exe parser\test_syntax_ok.c
echo.
echo === Yacc subset scan: c99_subset.y ===
yacc_subset_scan.exe c99_subset.y
goto :EOF

:NO_GCC
echo ERROR: gcc not in PATH. Install MinGW-w64 and add its bin folder to PATH.
exit /b 1
