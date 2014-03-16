if exist libgpg-error-install (
	echo libgpg-error-install directory already exists, skipping...
	exit /b
)

set OLDPATH=%PATH%

rem Download dependencies.

if not exist gnuwin32 mkdir gnuwin32
set PATH=%CD%\gnuwin32\bin;%PATH%

if exist gnuwin32\bin\gawk.exe goto pastgawk
	if not exist gawk-%GNUWIN32GAWKVER%-bin.zip (
		%WGET% http://sourceforge.net/projects/gnuwin32/files/gawk/%GNUWIN32GAWKVER%/gawk-%GNUWIN32GAWKVER%-bin.zip/download
		if errorlevel 1 exit /b 1
	)

	%SEVENZ% x -ognuwin32 gawk-%GNUWIN32GAWKVER%-bin.zip
	if errorlevel 1 exit /b 1
:pastgawk

rem Now libgpg-error itself.

if exist libgpg-error-%GPGERRORVER% %MY_RMDIR% libgpg-error-%GPGERRORVER%
if errorlevel 1 exit /b 1

if not exist libgpg-error-%GPGERRORVER%.tar.bz2 (
	%WGET% ftp://ftp.gnupg.org/gcrypt/libgpg-error/libgpg-error-%GPGERRORVER%.tar.bz2
	if errorlevel 1 exit /b 1
)

if exist libgpg-error-%GPGERRORVER%.tar %MY_RM% libgpg-error-%GPGERRORVER%.tar
%SEVENZ% x libgpg-error-%GPGERRORVER%.tar.bz2
if errorlevel 1 exit /b 1
%SEVENZ% x libgpg-error-%GPGERRORVER%.tar
if errorlevel 1 exit /b 1
%MY_RM% libgpg-error-%GPGERRORVER%.tar
if errorlevel 1 exit /b 1

pushd libgpg-error-%GPGERRORVER%
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libgpg-error-r267-20101205.diff
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libgpg-error-cmake-1.12.diff
if errorlevel 1 exit /b 1

mkdir build
if errorlevel 1 exit /b 1
pushd build
if errorlevel 1 exit /b 1

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\libgpg-error-install ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 exit /b 1

popd
popd

set PATH=%OLDPATH%

%MY_CP% "%INSTALLPREFIX%"\libgpg-error-install\bin\libgpg-error.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
