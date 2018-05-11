@ echo off
::  ------------------------------------------------------------  ::
::      Miniature makefile :)
::  ------------------------------------------------------------  ::

setlocal

set LIBNAME=noexcept
set LIBRARY=lib\lib%LIBNAME%.a

set COMPILE=gcc -c -xc++ -std=c++11 -Wall -Wextra -Wpedantic -Iinclude
set ARCHIVE=ar -rc

set MAKETEST=gcc -xc++ -std=c++11 -Wall -Wextra -Wpedantic -I. -Llib
set TESTLIBS=-l%LIBNAME% -lsupc++
set ALTFLAGS=-fno-rtti -fno-exceptions

:: Create directories
mkdir bin 2> NUL
mkdir lib 2> NUL
mkdir obj 2> NUL
mkdir test\bin 2> NUL
mkdir test\lib 2> NUL
mkdir test\obj 2> NUL

:: Clean
del /Q bin\*
del /Q lib\*
del /Q obj\*
del /Q test\bin\*
del /Q test\lib\*
del /Q test\obj\*

:: Build
%COMPILE% nx-new.cc -o obj\nx-new.o
::%COMPILE% nx-ini.cc -o obj\nx-ini.o

:: Archive
%ARCHIVE% %LIBRARY% obj\nx-new.o
::%ARCHIVE% %LIBRARY% obj\nx-ini.o

:: Test
%MAKETEST% test\test-nx-type.cc %TESTLIBS% -o test\bin\test-nx-type.exe && test\bin\test-nx-type.exe
%MAKETEST% %ALTFLAGS% test\test-nx-type.cc %TESTLIBS% -o test\bin\test-nx-type[alt].exe && test\bin\test-nx-type[alt].exe

%MAKETEST% test\test-nx-util.cc %TESTLIBS% -o test\bin\test-nx-util.exe && test\bin\test-nx-util.exe
%MAKETEST% %ALTFLAGS% test\test-nx-util.cc %TESTLIBS% -o test\bin\test-nx-util[alt].exe && test\bin\test-nx-util[alt].exe


endlocal
