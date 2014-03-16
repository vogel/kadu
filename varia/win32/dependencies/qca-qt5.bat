if exist qca-install (
	echo qca-install directory already exists, skipping...
	exit /b
)

if exist qca %MY_RMDIR% qca
if errorlevel 1 exit /b 1

set OLDPATH=%PATH%
set QTDIR=%INSTALLPREFIX%\qt5\qtbase
set PATH=%QTDIR%\bin;%INSTALLPREFIX%\icu-install\lib;%INSTALLPREFIX%\zlib-install\bin;%PATH%

%GIT% clone git://anongit.kde.org/qca
if errorlevel 1 exit /b 1

pushd qca
if errorlevel 1 exit /b 1

%GIT% checkout %QCAVER%
if errorlevel 1 exit /b 1

mkdir build
if errorlevel 1 exit /b 1
pushd build
if errorlevel 1 exit /b 1

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\qca-install -DQCA_SUFFIX="qt5" -DBUILD_PLUGINS="ossl" -DBUILD_TESTS:BOOL=OFF -DBUILD_TOOLS:BOOL=OFF -DQT4_BUILD:BOOL=OFF -DOPENSSL_ROOT_DIR:PATH="%INSTALLPREFIX%"\openssl-install ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 exit /b 1

popd
popd

set PATH=%OLDPATH%

%MY_CP% "%INSTALLPREFIX%"\qca-install\bin\qca-qt5.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\qt-plugins\crypto mkdir "%INSTALLBASE%"\qt-plugins\crypto
%MY_CP% "%INSTALLPREFIX%"\qca-install\lib\qca-qt5\crypto\qca-ossl.dll "%INSTALLBASE%"\qt-plugins\crypto
if errorlevel 1 exit /b 1
