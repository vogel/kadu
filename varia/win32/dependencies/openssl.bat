@echo off

echo.
echo Building openssl
echo.

set ret=0

call "%~dp0\..\utils.bat" build-dependency perl
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency nasm
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 goto fail

set PATH=%CD%\nasm-%NASMVER%;%PATH%

call "%~dp0\..\utils.bat" load-result openssl OPENSSSL
if errorlevel 1 goto fail

if %OPENSSSL_RESULT% EQU 1 goto downloaded
if %OPENSSSL_RESULT% EQU 2 (
	pushd openssl-%OSSLVER%
	goto unpacked
)
if %OPENSSSL_RESULT% EQU 3 (
	pushd openssl-%OSSLVER%
	goto configured
)
if %OPENSSSL_RESULT% EQU 4 goto compiled
if %OPENSSSL_RESULT% EQU 5 goto ready

if exist openssl-%OSSLVER% %RMDIR% openssl-%OSSLVER%
if errorlevel 1 goto fail

if exist openssl-%OSSLVER%.tar.gz %RM% openssl-%OSSLVER%.tar.gz
if errorlevel 1 goto fail

%WGET% http://www.openssl.org/source/openssl-%OSSLVER%.tar.gz
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result openssl 1
if errorlevel 1 goto fail

:downloaded

if exist openssl-%OSSLVER% %RMDIR% openssl-%OSSLVER%
if errorlevel 1 goto fail

if exist openssl-%OSSLVER%.tar %RM% openssl-%OSSLVER%.tar
if errorlevel 1 goto fail

%SEVENZ% x openssl-%OSSLVER%.tar.gz
if errorlevel 1 goto fail

%SEVENZ% x openssl-%OSSLVER%.tar
if errorlevel 1 goto fail

%RM% openssl-%OSSLVER%.tar
if errorlevel 1 goto fail

pushd openssl-%OSSLVER%
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result openssl 2
if errorlevel 1 goto fail2

:unpacked

%PERL% Configure VC-WIN32 enable-md2 --prefix="%INSTALLPREFIX%"\openssl-install
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result openssl 3
if errorlevel 1 goto fail2

:configured

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

call ms\do_nasm.bat
if errorlevel 1 goto fail2

nmake -f ms\ntdll.mak
if errorlevel 1 goto fail2

nmake -f ms\ntdll.mak test
if errorlevel 1 goto fail2

nmake -f ms\ntdll.mak install
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result openssl 4
if errorlevel 1 goto fail2

:compiled

%CP% "%INSTALLPREFIX%"\openssl-%OSSLVER%\LICENSE "%INSTALLBASE%"\LICENSE.OpenSSL
if errorlevel 1 goto fail

%CP% "%INSTALLPREFIX%"\openssl-install\bin\libeay32.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%INSTALLPREFIX%"\openssl-install\bin\ssleay32.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result openssl 5
if errorlevel 1 goto fail

:ready

echo.
echo openssl build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo openssl: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
