@echo off

echo.
echo Building xz
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result xz XZ
if errorlevel 1 goto fail

if %XZ_RESULT% EQU 1 goto downloaded
if %XZ_RESULT% EQU 2 (
	pushd xz
	goto unpacked
)
if %XZ_RESULT% EQU 3 goto ready

if exist xz-%XZVER%-windows.7z %RM% xz-%XZVER%-windows.7z
if errorlevel 1 goto fail

%WGET% http://tukaani.org/xz/xz-%XZVER%-windows.7z
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result xz 1
if errorlevel 1 goto fail

:downloaded

if exist xz %RMDIR% xz
if errorlevel 1 goto fail

mkdir xz
if errorlevel 1 goto fail

pushd xz
if errorlevel 1 goto fail

%SEVENZ% x ..\xz-%XZVER%-windows.7z
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result xz 2
if errorlevel 1 goto fail

:unpacked

mkdir lib
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

lib /def:doc\liblzma.def /out:lib\liblzma.lib /machine:ix86
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail2

%CP% "%INSTALLPREFIX%"\xz\bin_i486\liblzma.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result xz 3
if errorlevel 1 goto fail

:ready

echo.
echo xz build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo gz: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
