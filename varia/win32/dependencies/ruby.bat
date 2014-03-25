@echo off

echo.
echo Building ruby
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result ruby RUBY
if errorlevel 1 goto fail2

if %RUBY_RESULT% EQU 1 goto downloaded
if %RUBY_RESULT% EQU 2 goto ready

if exist ruby-%RUBYVER%-i386-mingw32.7z %RM% ruby-%RUBYVER%-i386-mingw32.7z
if errorlevel 1 goto fail2

%WGET% http://rubyforge.org/frs/download.php/76807/ruby-%RUBYVER%-i386-mingw32.7z
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result ruby 1
if errorlevel 1 goto fail2

:downloaded

if exist ruby-%RUBYVER%-i386-mingw32 %RMDIR% ruby-%RUBYVER%-i386-mingw32
if errorlevel 1 goto fail

%SEVENZ% x ruby-%RUBYVER%-i386-mingw32.7z
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result ruby 2
if errorlevel 1 goto fail2

:ready
popd
echo Setting up ruby succeeded
exit

:fail2
popd

:fail
echo Setting up ruby failed
exit /b 1
