if exist zlib-install (
	echo zlib-install directory already exists, skipping...
	exit /b
)

if exist zlib-%ZLIBVER% %RMDIR% zlib-%ZLIBVER%

if not exist zlib-%ZLIBVER%.tar.bz2 (
	%WGET% http://zlib.net/zlib-%ZLIBVER%.tar.bz2
	if errorlevel 1 exit /b 1
)

if exist zlib-%ZLIBVER%.tar %RM% zlib-%ZLIBVER%.tar
%SEVENZ% x zlib-%ZLIBVER%.tar.bz2
if errorlevel 1 exit /b 1
%SEVENZ% x zlib-%ZLIBVER%.tar
if errorlevel 1 exit /b 1
%RM% zlib-%ZLIBVER%.tar

pushd zlib-%ZLIBVER%
if errorlevel 1 exit /b 1

mkdir build
if errorlevel 1 exit /b 1
pushd build
if errorlevel 1 exit /b 1

REM TODO: Improve this CMake file; maybe use masm? (nmake makefile can use it).
%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\zlib-install -DBUILD_SHARED_LIBS:BOOL=ON ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 exit /b 1

popd
popd

%CP% "%INSTALLPREFIX%"\zlib-install\bin\zlib1.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
