@echo off

echo.
echo Building winflexbison
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result winflexbison WINFLEXBISON
if errorlevel 1 goto fail2

if %WINFLEXBISON_RESULT% EQU 1 goto downloaded
if %WINFLEXBISON_RESULT% EQU 2 goto ready

if exist win_flex_bison-latest.zip %RM% win_flex_bison-latest.zip
if errorlevel 1 goto fail2

%WGET% http://downloads.sourceforge.net/project/winflexbison/win_flex_bison-latest.zip
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result winflexbison 1
if errorlevel 1 goto fail2

:downloaded

if exist winflexbison %RMDIR% winflexbison
if errorlevel 1 goto fail2

mkdir winflexbison
if errorlevel 1 goto fail2

%SEVENZ% x -owinflexbison win_flex_bison-latest.zip
if errorlevel 1 goto fail

rename winflexbison\win_bison.exe bison.exe
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result winflexbison 2
if errorlevel 1 goto fail2

:ready
popd
echo Setting up winflexbison succeeded
exit

:fail2
popd

:fail
echo Setting up winflexbison failed
exit /b 1
