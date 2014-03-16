if exist libotr-install (
	echo libotr-install directory already exists, skipping...
	exit /b
)

if exist libotr-%OTRVER% %MY_RMDIR% libotr-%OTRVER%
if errorlevel 1 exit /b 1

if not exist libotr-%OTRVER%.tar.gz (
	%WGET% http://www.cypherpunks.ca/otr/libotr-%OTRVER%.tar.gz
	if errorlevel 1 exit /b 1
)

if exist libotr-%OTRVER%.tar %MY_RM% libotr-%OTRVER%.tar
%SEVENZ% x libotr-%OTRVER%.tar.gz
if errorlevel 1 exit /b 1
%SEVENZ% x libotr-%OTRVER%.tar
if errorlevel 1 exit /b 1
%MY_RM% libotr-%OTRVER%.tar
if errorlevel 1 exit /b 1

pushd libotr-%OTRVER%
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libotr-4.0.0-c89.diff
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libotr-4.0.0.diff
if errorlevel 1 exit /b 1

mkdir build
if errorlevel 1 exit /b 1
pushd build
if errorlevel 1 exit /b 1

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\libotr-install -DCMAKE_PREFIX_PATH:PATH="%INSTALLPREFIX%"\libgcrypt-install ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 exit /b 1

popd
popd

%MY_CP% "%INSTALLPREFIX%"\libgcrypt-install\include\*.h "%INSTALLPREFIX%"\libotr-install\include\
if errorlevel 1 exit /b 1

%MY_CP% "%INSTALLPREFIX%"\libgpg-error-install\lib\libgpg-error.lib "%INSTALLPREFIX%"\libotr-install\lib\
if errorlevel 1 exit /b 1

%MY_CP% "%INSTALLPREFIX%"\libgcrypt-install\lib\gcrypt.lib "%INSTALLPREFIX%"\libotr-install\lib\
if errorlevel 1 exit /b 1
