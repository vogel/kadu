@echo off

echo.
echo Building grep
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result grep GREP
if errorlevel 1 goto fail2

if %GREP_RESULT% EQU 1 goto downloaded1
if %GREP_RESULT% EQU 2 goto downloaded2
if %GREP_RESULT% EQU 3 goto ready

if exist grep-%GNUWIN32GREPVER%-dep.zip %RM% grep-%GNUWIN32GREPVER%-dep.zip
if errorlevel 1 goto fail2

%WGET% http://sourceforge.net/projects/gnuwin32/files/grep/%GNUWIN32GREPVER%/grep-%GNUWIN32GREPVER%-dep.zip/download
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result grep 1
if errorlevel 1 goto fail2

:downloaded1

if exist grep-%GNUWIN32GREPVER%-bin.zip %RM% grep-%GNUWIN32GREPVER%-bin.zip
if errorlevel 1 goto fail2

%WGET% http://sourceforge.net/projects/gnuwin32/files/grep/%GNUWIN32GREPVER%/grep-%GNUWIN32GREPVER%-bin.zip/download
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result grep 2
if errorlevel 1 goto fail2

:downloaded2

if not exist gnuwin32 mkdir gnuwin32
if errorlevel 1 goto fail2

%SEVENZ% x -ognuwin32 grep-%GNUWIN32GREPVER%-dep.zip
if errorlevel 1 goto fail

%SEVENZ% x -ognuwin32 grep-%GNUWIN32GREPVER%-bin.zip
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result grep 3
if errorlevel 1 goto fail2

:ready
popd
echo Setting up grep succeeded
exit

:fail2
popd

:fail
echo Setting up grep failed
exit /b 1
