@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist zlib-install (
	echo zlib-install directory already exists, skipping...
	goto end
)

if exist zlib-%ZLIBVER% %RMDIR% zlib-%ZLIBVER%

if not exist zlib-%ZLIBVER%.tar.xz (
	%WGET% http://zlib.net/zlib-%ZLIBVER%.tar.xz
	if errorlevel 1 goto fail
)

if exist zlib-%ZLIBVER%.tar %RM% zlib-%ZLIBVER%.tar
%SEVENZ% x zlib-%ZLIBVER%.tar.xz
if errorlevel 1 goto fail
%SEVENZ% x zlib-%ZLIBVER%.tar
if errorlevel 1 goto fail
%RM% zlib-%ZLIBVER%.tar

pushd zlib-%ZLIBVER%
if errorlevel 1 goto fail

mkdir build
if errorlevel 1 goto fail2
pushd build
if errorlevel 1 goto fail2

REM TODO: Improve this CMake file; maybe use masm? (nmake makefile can use it).
%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\zlib-install -DBUILD_SHARED_LIBS:BOOL=ON ..
if errorlevel 1 goto fail3

%CMAKE_MAKE_INSTALL%
if errorlevel 1 goto fail3

popd
popd

%CP% "%INSTALLPREFIX%"\zlib-install\bin\zlib1.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

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
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
