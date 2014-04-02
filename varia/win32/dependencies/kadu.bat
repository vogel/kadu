rem @echo off

echo.
echo Building kadu
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency zlib
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency libidn
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency openssl
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency libgadu
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency aspell
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency qt
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency qtwebkit-23
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency qca
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency xz
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency libarchive
if errorlevel 1 goto fail

echo.
echo kadu build: Success
echo.
goto end

:fail
echo.
echo kadu: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
