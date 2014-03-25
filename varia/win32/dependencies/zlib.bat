@echo off

echo.
echo Building zlib
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result zlib ZLIB
if errorlevel 1 goto fail

if %ZLIB_RESULT% EQU 1 goto downloaded
if %ZLIB_RESULT% EQU 2 goto unpacked
if %ZLIB_RESULT% EQU 3 (
	pushd zlib-%ZLIBVER%
	if errorlevel 1 goto fail

	pushd build
	if errorlevel 1 goto fail2

	goto configured
)
if %ZLIB_RESULT% EQU 4 goto compiled
if %ZLIB_RESULT% EQU 5 goto ready

if exist zlib-%ZLIBVER%.tar.xz %RM% zlib-%ZLIBVER%.tar.xz
if errorlevel 1 goto fail

%WGET% http://zlib.net/zlib-%ZLIBVER%.tar.xz
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result zlib 1
if errorlevel 1 goto fail

:downloaded

if exist zlib-%ZLIBVER%.tar %RM% zlib-%ZLIBVER%.tar
if errorlevel 1 goto fail

%SEVENZ% x zlib-%ZLIBVER%.tar.xz
if errorlevel 1 goto fail

if exist zlib-%ZLIBVER% %RMDIR% zlib-%ZLIBVER%
if errorlevel 1 goto fail

%SEVENZ% x zlib-%ZLIBVER%.tar
if errorlevel 1 goto fail

%RM% zlib-%ZLIBVER%.tar
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result zlib 2
if errorlevel 1 goto fail

:unpacked

pushd zlib-%ZLIBVER%
if errorlevel 1 goto fail

mkdir build
if errorlevel 1 goto fail2

pushd build
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

REM TODO: Improve this CMake file; maybe use masm? (nmake makefile can use it).
%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\zlib-install -DBUILD_SHARED_LIBS:BOOL=ON ..
if errorlevel 1 goto fail3

call "%~dp0\..\utils.bat" store-result zlib 3
if errorlevel 1 goto fail3

:configured

%CMAKE_MAKE_INSTALL%
if errorlevel 1 goto fail3

popd
if errorlevel 1 goto fail3

popd
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result zlib 4
if errorlevel 1 goto fail

:compiled

%CP% "%INSTALLPREFIX%"\zlib-install\bin\zlib1.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result zlib 5
if errorlevel 1 goto fail

:ready

echo.
echo zlib build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo zlib: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
