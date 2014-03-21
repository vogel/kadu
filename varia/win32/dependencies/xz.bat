@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist xz (
	echo xz directory already exists, skipping...
	goto end
)

if not exist xz-%XZVER%-windows.7z (
	%WGET% http://tukaani.org/xz/xz-%XZVER%-windows.7z
	if errorlevel 1 goto fail
)

mkdir xz
if errorlevel 1 goto fail

pushd xz
if errorlevel 1 goto fail

%SEVENZ% x ..\xz-%XZVER%-windows.7z
if errorlevel 1 goto fail2

mkdir lib
if errorlevel 1 goto fail2

lib /def:doc\liblzma.def /out:lib\liblzma.lib /machine:ix86
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail2

%CP% "%INSTALLPREFIX%"\xz\bin_i486\liblzma.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

echo.
echo gz build: Success
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
