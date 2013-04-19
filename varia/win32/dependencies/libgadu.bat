if exist libgadu-install (
	echo libgadu-install directory already exists, skipping...
	exit /b
)

if exist libgadu %RMDIR% libgadu

%SVN% co -r %GADUVER% http://toxygen.net/svn/libgadu/trunk libgadu
if errorlevel 1 exit /b 1

pushd libgadu
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\libgadu-win32.patch
if errorlevel 1 exit /b 1

mkdir build
if errorlevel 1 exit /b 1
pushd build
if errorlevel 1 exit /b 1

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\libgadu-install -DBUILD_SHARED:BOOL=ON -DBUILD_STATIC:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DBUILD_DOC:BOOL=OFF -DWITH_PTHREAD:BOOL=OFF -DWITH_ZLIB:BOOL=ON -DWITH_GNUTLS:BOOL=OFF -DWITH_OPENSSL:BOOL=ON -DZLIB_ROOT:PATH="%INSTALLPREFIX%"\zlib-install -DOPENSSL_ROOT_DIR:PATH="%INSTALLPREFIX%"\openssl-install ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 exit /b 1

popd
popd

%CP% "%INSTALLPREFIX%"\libgadu-install\bin\gadu.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
