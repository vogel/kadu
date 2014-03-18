@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist libidn (
	echo libidn directory already exists, skipping...
	goto end
)

if not exist libidn-%IDNVER%.tar.gz (
	%WGET% http://ftp.gnu.org/gnu/libidn/libidn-%IDNVER%.tar.gz
	if errorlevel 1 exit /b 1
)

if exist libidn-%IDNVER%.tar %RM% libidn-%IDNVER%.tar
%SEVENZ% x libidn-%IDNVER%.tar.gz
if errorlevel 1 goto fail
%SEVENZ% x libidn-%IDNVER%.tar
if errorlevel 1 goto fail
%RM% libidn-%IDNVER%.tar

rename libidn-%IDNVER% libidn
if errorlevel 1 goto fail

pushd libidn\windows
if errorlevel 1 goto fail
VCUpgrade libidn.vcproj
if errorlevel 1 goto fail2
%MSBUILD% libidn.vcxproj
if errorlevel 1 goto fail2
popd

%CP% "%INSTALLPREFIX%"\libidn\windows\lib\libidn.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

echo.
echo libidn build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo libidn: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
