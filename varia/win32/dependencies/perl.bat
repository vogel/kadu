@echo off

echo.
echo Building perl
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result perl PERL
if errorlevel 1 goto fail2

if %PERL_RESULT% EQU 1 goto downloaded
if %PERL_RESULT% EQU 2 goto unpacked
if %PERL_RESULT% EQU 3 goto ready

if exist strawberry-perl-%PERLVER%-32bit-portable.zip %RM% strawberry-perl-%PERLVER%-32bit-portable.zip
if errorlevel 1 goto fail

%WGET% http://strawberryperl.com/download/%PERLVER%/strawberry-perl-%PERLVER%-32bit-portable.zip
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result perl 1
if errorlevel 1 goto fail

:downloaded

if exist strawberry-perl-%PERLVER%-32bit-portable %RMDIR% strawberry-perl-%PERLVER%-32bit-portable 
if errorlevel 1 goto fail

mkdir strawberry-perl-%PERLVER%-32bit-portable
if errorlevel 1 goto fail

%SEVENZ% x -ostrawberry-perl-%PERLVER%-32bit-portable strawberry-perl-%PERLVER%-32bit-portable.zip
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result perl 2
if errorlevel 1 goto fail

:unpacked

%SED% -i -e "/cmd \/K/d" -e "/@echo off/d" strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result perl 3
if errorlevel 1 goto fail

:ready

echo.
echo perl download: Success
echo.
goto end

:fail
echo.
echo perl: Error encountered
echo.
popd
pause
exit /b 1

:end
