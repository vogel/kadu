@echo off

echo.
echo Building nasm
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result nasm NASM
if errorlevel 1 goto fail2

if %NASM_RESULT% EQU 1 goto downloaded
if %NASM_RESULT% EQU 2 goto ready

if exist nasm-%NASMVER%-win32.zip %RM% nasm-%NASMVER%-win32.zip
if errorlevel 1 goto fail

%WGET% http://www.nasm.us/pub/nasm/releasebuilds/%NASMVER%/win32/nasm-%NASMVER%-win32.zip
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result nasm 1
if errorlevel 1 goto fail

:downloaded

if exist nasm-%NASMVER% %RMDIR% nasm-%NASMVER%
if errorlevel 1 goto fail

%SEVENZ% x nasm-%NASMVER%-win32.zip
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result nasm 2
if errorlevel 1 goto fail

:ready

echo.
echo nasm download: Success
echo.
goto end

:fail
echo.
echo nasm: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
