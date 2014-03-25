@echo off

echo.
echo Building gperf
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result gperf GPERF
if errorlevel 1 goto fail2

if %GPERF_RESULT% EQU 1 goto downloaded
if %GPERF_RESULT% EQU 2 goto ready

if exist gperf-%GNUWIN32GPERFVER%-bin.zip %RM% gperf-%GNUWIN32GPERFVER%-bin.zip
if errorlevel 1 goto fail2

%WGET% http://sourceforge.net/projects/gnuwin32/files/gperf/%GNUWIN32GPERFVER%/gperf-%GNUWIN32GPERFVER%-bin.zip/download
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result gperf 1
if errorlevel 1 goto fail2

:downloaded

if not exist gnuwin32 mkdir gnuwin32
if errorlevel 1 goto fail2

if exist gnuwin32\bin\gperf.exe %RM% gnuwin32\bin\gperf.exe
if errorlevel 1 goto fail2

%SEVENZ% x -y -ognuwin32 gperf-%GNUWIN32GPERFVER%-bin.zip
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result gperf 2
if errorlevel 1 goto fail2

:ready
popd
echo Setting up gperf succeeded
exit

:fail2
popd

:fail
echo Setting up gperf failed
exit /b 1
