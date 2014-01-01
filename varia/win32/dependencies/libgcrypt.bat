if exist libgcrypt-install (
	echo libgcrypt-install directory already exists, skipping...
	exit /b
)

if exist libgcrypt-%GCRYPTVER% %RMDIR% libgcrypt-%GCRYPTVER%

if not exist libgcrypt-%GCRYPTVER%.tar.bz2 (
	%WGET% ftp://ftp.gnupg.org/gcrypt/libgcrypt/libgcrypt-%GCRYPTVER%.tar.bz2
	if errorlevel 1 exit /b 1
)

if exist libgcrypt-%GCRYPTVER%.tar %RM% libgcrypt-%GCRYPTVER%.tar
%SEVENZ% x libgcrypt-%GCRYPTVER%.tar.bz2
if errorlevel 1 exit /b 1
%SEVENZ% x libgcrypt-%GCRYPTVER%.tar
if errorlevel 1 exit /b 1
%RM% libgcrypt-%GCRYPTVER%.tar

pushd libgcrypt-%GCRYPTVER%
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libgcrypt-1.5.0-20110831.diff
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libgcrypt-1.5.3-cmake.diff
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libgcrypt-win64.diff
if errorlevel 1 exit /b 1

mkdir build
if errorlevel 1 exit /b 1
pushd build
if errorlevel 1 exit /b 1

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\libgcrypt-install -DCMAKE_PREFIX_PATH:PATH="%INSTALLPREFIX%"\libgpg-error-install ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 exit /b 1

popd
popd

%CP% "%INSTALLPREFIX%"\libgpg-error-install\include\gpg-error.h "%INSTALLPREFIX%"\libgcrypt-install\include\
if errorlevel 1 exit /b 1

%CP% "%INSTALLPREFIX%"\libgcrypt-install\bin\gcrypt.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
