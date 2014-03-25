@echo off

echo.
echo Building aspell
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-perl
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result aspell ASPELL
if errorlevel 1 goto fail

if %ASPELL_RESULT% EQU 1 goto downloaded
if %ASPELL_RESULT% EQU 2 goto unpacked
if %ASPELL_RESULT% EQU 3 (
	pushd aspell-%ASPELLVER%
	if errorlevel 1 goto fail

	goto patched
)
if %ASPELL_RESULT% EQU 4 goto intalled
if %ASPELL_RESULT% EQU 5 goto data-downloaded
if %ASPELL_RESULT% EQU 6 goto data-unpacked
if %ASPELL_RESULT% EQU 7 goto ready

if exist aspell-%ASPELLVER%.tar.gz %RM% aspell-%ASPELLVER%.tar.gz
if errorlevel 1 goto fail

%WGET% ftp://ftp.gnu.org/gnu/aspell/aspell-%ASPELLVER%.tar.gz
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result aspell 1
if errorlevel 1 goto fail

:downloaded

if exist aspell-%ASPELLVER% %RMDIR% aspell-%ASPELLVER%
if errorlevel 1 goto fail

if exist aspell-%ASPELLVER%.tar %RM% aspell-%ASPELLVER%.tar
if errorlevel 1 goto fail

%SEVENZ% x aspell-%ASPELLVER%.tar.gz
if errorlevel 1 goto fail

%SEVENZ% x aspell-%ASPELLVER%.tar
if errorlevel 1 goto fail

%RM% aspell-%ASPELLVER%.tar
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result aspell 2
if errorlevel 1 goto fail

:unpacked

pushd aspell-%ASPELLVER%
if errorlevel 1 goto fail

%PATCH% -p1 < "%~dp0"\patches\aspell-0.60.6-20100726.diff
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result aspell 3
if errorlevel 1 goto fail

:patched

mkdir build
if errorlevel 1 goto fail2

pushd build
if errorlevel 1 goto fail2

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\aspell-install ..
if errorlevel 1 goto fail3

%CMAKE_MAKE_INSTALL%
if errorlevel 1 goto fail3

popd
popd

call "%~dp0\..\utils.bat" store-result aspell 4
if errorlevel 1 goto fail

:installed

if exist aspell-data-bin.7z %RM% aspell-data-bin.7z
if errorlevel 1 goto fail

%WGET% http://download.kadu.im/win32-devel/aspell-data-bin.7z
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result aspell 5
if errorlevel 1 goto fail

:data-downloaded

%SEVENZ% x -o"%INSTALLBASE%"\aspell aspell-data-bin.7z
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result aspell 6
if errorlevel 1 goto fail

:data-unpacked

%CP% "%INSTALLPREFIX%"\aspell-install\bin\aspell.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

if exist "%INSTALLBASE%"\aspell %RMDIR% "%INSTALLBASE%"\aspell
if errorlevel 1 goto fail

%CPDIR% "%INSTALLPREFIX%"\aspell-install\lib\aspell-0.60 "%INSTALLBASE%"\aspell
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result aspell 7
if errorlevel 1 goto fail

:ready

echo.
echo apell build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo apell: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
