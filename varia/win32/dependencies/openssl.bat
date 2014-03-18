@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist openssl-install (
	echo openssl-install directory already exists, skipping...
	goto end
)

set OLDPATH=%PATH%

call strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 goto fail

if exist nasm-%NASMVER%\nasm.exe goto pastnasm
	if exist nasm-%NASMVER% %RMDIR% nasm-%NASMVER%

	if not exist nasm-%NASMVER%-win32.zip (
		%WGET% http://www.nasm.us/pub/nasm/releasebuilds/%NASMVER%/win32/nasm-%NASMVER%-win32.zip
		if errorlevel 1 goto fail
	)

	%SEVENZ% x nasm-%NASMVER%-win32.zip
	if errorlevel 1 goto fail
:pastnasm

set PATH=%CD%\nasm-%NASMVER%;%PATH%

if exist openssl-%OSSLVER% %RMDIR% openssl-%OSSLVER%

if not exist openssl-%OSSLVER%.tar.gz (
	%WGET% http://www.openssl.org/source/openssl-%OSSLVER%.tar.gz
	if errorlevel 1 goto fail
)

if exist openssl-%OSSLVER%.tar %RM% openssl-%OSSLVER%.tar
%SEVENZ% x openssl-%OSSLVER%.tar.gz
if errorlevel 1 goto fail
%SEVENZ% x openssl-%OSSLVER%.tar
if errorlevel 1 goto fail
%RM% openssl-%OSSLVER%.tar

pushd openssl-%OSSLVER%
if errorlevel 1 goto fail2

%PERL% Configure VC-WIN32 enable-md2 --prefix="%INSTALLPREFIX%"\openssl-install
if errorlevel 1 goto fail2

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

set PATH=%OLDPATH%

%CP% "%INSTALLPREFIX%"\openssl-%OSSLVER%\LICENSE "%INSTALLBASE%"\LICENSE.OpenSSL
if errorlevel 1 goto fail

%CP% "%INSTALLPREFIX%"\openssl-install\bin\libeay32.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%INSTALLPREFIX%"\openssl-install\bin\ssleay32.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

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
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
