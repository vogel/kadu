@echo off

echo.
echo Building python
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result python PYTHON
if errorlevel 1 goto fail2

if %PYTHON_RESULT% EQU 1 goto downloaded
if %PYTHON_RESULT% EQU 2 goto ready

if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z %RM% python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
if errorlevel 1 goto fail2

%WGET% http://python-common-portable.googlecode.com/files/python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result python 1
if errorlevel 1 goto fail2

:downloaded

if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32 %RMDIR% python-%PYTHONCOMMONPORTABLEVER%-bin-win32
if errorlevel 1 goto fail2

mkdir python-%PYTHONCOMMONPORTABLEVER%-bin-win32
if errorlevel 1 goto fail2

%SEVENZ% x -opython-%PYTHONCOMMONPORTABLEVER%-bin-win32 python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result python 2
if errorlevel 1 goto fail2

:ready
popd
echo Setting up python succeeded
exit

:fail2
popd

:fail
echo Setting up python failed
exit /b 1
