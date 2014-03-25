@echo off

echo.
echo Building libgadu
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result libgadu LIBGADU
if errorlevel 1 goto fail

if %LIBGADU_RESULT% EQU 1 goto downloaded
if %LIBGADU_RESULT% EQU 2 goto unpacked
if %LIBGADU_RESULT% EQU 3 goto ready

if exist mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.rpm %RM% mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.rpm
if errorlevel 1 goto fail

%WGET% http://download.opensuse.org/repositories/home:/tomkiewicz:/libgadu/win32/noarch/mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.rpm
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result libgadu 1
if errorlevel 1 goto fail

:downloaded

if exist mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.cpio %RM% mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.cpio
if errorlevel 1 goto fail

%SEVENZ% x mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.rpm
if errorlevel 1 goto fail

%RM% mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.rpm
if errorlevel 1 goto fail

if exist libgadu-%GADUVER%-win32.zip %RM% libgadu-%GADUVER%-win32.zip
if errorlevel 1 goto fail

%SEVENZ% x mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.cpio
if errorlevel 1 goto fail

%RM% mingw32-libgadu-release-%GADUVER%-%GADUPACKAGEVER%.noarch.cpio
if errorlevel 1 goto fail

if exist libgadu-%GADUVER%-win32 %RMDIR% libgadu-%GADUVER%-win32
if errorlevel 1 goto fail

%SEVENZ% x libgadu-%GADUVER%-win32.zip
if errorlevel 1 goto fail

%RM% libgadu-%GADUVER%-win32.zip
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result libgadu 2
if errorlevel 1 goto fail

:unpacked

if exist libgadu-install %RMDIR% libgadu-install
if errorlevel 1 goto fail

mkdir libgadu-install
if errorlevel 1 goto fail

mkdir libgadu-install\include
if errorlevel 1 goto fail

mkdir libgadu-install\lib
if errorlevel 1 goto fail

pushd libgadu-%GADUVER%-win32
if errorlevel 1 goto fail

%CP% "bin\libgadu-3.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libnettle-4-6.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libintl-8.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libhogweed-2-4.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libwinpthread-1.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libtasn1-6.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libgnutls-28.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libp11-kit-0.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libffi-6.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libgcc_s_sjlj-1.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libgmp-10.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "deps\libprotobuf-c-0.dll" "%INSTALLBASE%"
if errorlevel 1 goto fail2

%CP% "bin\libgadu-3.dll" "%INSTALLPREFIX%"\libgadu-install\lib\gadu-3.dll
if errorlevel 1 goto fail2

%CP% "dev\libgadu.h" "%INSTALLPREFIX%"\libgadu-install\include
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

lib /machine:i386 /def:dev\libgadu.def /out:"%INSTALLPREFIX%"\libgadu-install\lib\gadu.lib

popd

call "%~dp0\..\utils.bat" store-result libgadu 3
if errorlevel 1 goto fail

:ready

echo.
echo libgadu build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo libgadu: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
