if exist openssl-install (
	echo openssl-install directory already exists, skipping...
	exit /b
)

set OLDPATH=%PATH%

if exist nasm-%NASMVER%\nasm.exe goto pastnasm
	if exist nasm-%NASMVER% %RMDIR% nasm-%NASMVER%

	if not exist nasm-%NASMVER%-win32.zip (
		%WGET% http://www.nasm.us/pub/nasm/releasebuilds/%NASMVER%/win32/nasm-%NASMVER%-win32.zip
		if errorlevel 1 exit /b 1
	)

	%SEVENZ% x nasm-%NASMVER%-win32.zip
	if errorlevel 1 exit /b 1

	set PATH=%CD%\nasm-%NASMVER%;%PATH%
:pastnasm

if exist openssl-%OSSLVER% %RMDIR% openssl-%OSSLVER%

if not exist openssl-%OSSLVER%.tar.gz (
	%WGET% http://www.openssl.org/source/openssl-%OSSLVER%.tar.gz
	if errorlevel 1 exit /b 1
)

if exist openssl-%OSSLVER%.tar %RM% openssl-%OSSLVER%.tar
%SEVENZ% x openssl-%OSSLVER%.tar.gz
if errorlevel 1 exit /b 1
%SEVENZ% x openssl-%OSSLVER%.tar
if errorlevel 1 exit /b 1
%RM% openssl-%OSSLVER%.tar

pushd openssl-%OSSLVER%
if errorlevel 1 exit /b 1

%PERL% Configure VC-WIN32 --prefix="%INSTALLPREFIX%"\openssl-install
if errorlevel 1 exit /b 1
call ms\do_nasm.bat
if errorlevel 1 exit /b 1
nmake -f ms\ntdll.mak
if errorlevel 1 exit /b 1
nmake -f ms\ntdll.mak test
if errorlevel 1 exit /b 1
nmake -f ms\ntdll.mak install
if errorlevel 1 exit /b 1

popd

set PATH=%OLDPATH%

%CP% "%INSTALLPREFIX%"\openssl-%OSSLVER%\LICENSE "%INSTALLBASE%"\LICENSE.OpenSSL
if errorlevel 1 exit /b 1

%CP% "%INSTALLPREFIX%"\openssl-install\bin\libeay32.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%CP% "%INSTALLPREFIX%"\openssl-install\bin\ssleay32.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
