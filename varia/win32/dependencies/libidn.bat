@echo off

echo.
echo Building libidn
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result libidn LIBIDN
if errorlevel 1 goto fail

if %LIBIDN_RESULT% EQU 1 goto downloaded
if %LIBIDN_RESULT% EQU 2 goto unpacked
if %LIBIDN_RESULT% EQU 3 goto compiled
if %LIBIDN_RESULT% EQU 4 goto installed

if exist libidn-%IDNVER%.tar.gz %RM% libidn-%IDNVER%.tar.gz
if errorlevel 1 goto fail

%WGET% http://ftp.gnu.org/gnu/libidn/libidn-%IDNVER%.tar.gz
if errorlevel 1 exit /b 1

call "%~dp0\..\utils.bat" store-result libidn 1
if errorlevel 1 goto fail

:downloaded

if exist libidn %RMDIR% libidn
if errorlevel 1 goto fail

if exist libidn libidn-%IDNVER% libidn
if errorlevel 1 goto fail

if exist libidn-%IDNVER%.tar %RM% libidn-%IDNVER%.tar
if errorlevel 1 goto fail

%SEVENZ% x libidn-%IDNVER%.tar.gz
if errorlevel 1 goto fail

%SEVENZ% x libidn-%IDNVER%.tar
if errorlevel 1 goto fail

%RM% libidn-%IDNVER%.tar
if errorlevel 1 goto fail

rename libidn-%IDNVER% libidn
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result libidn 2
if errorlevel 1 goto fail

:unpacked

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

pushd libidn\windows
if errorlevel 1 goto fail

VCUpgrade libidn.vcproj
if errorlevel 1 goto fail2

%MSBUILD% libidn.vcxproj
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result libidn 3
if errorlevel 1 goto fail2

:compiled

%CP% "%INSTALLPREFIX%"\libidn\windows\lib\libidn.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result libidn 4
if errorlevel 1 goto fail

:installed

echo.
echo libidn build: Success
echo.
goto end

:fail2
popd

:fail
echo.
echo libidn: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
