if exist qca (
	echo qca directory already exists, skipping...
	exit /b
)

set OLDPATH=%PATH%
set QTDIR=%INSTALLPREFIX%\qt
set PATH=%QTDIR%\bin;%INSTALLPREFIX%\zlib-install\bin;%PATH%
set QMAKEFEATURES=%INSTALLPREFIX%\openssl-install\delta-winlocal

if not exist "%QMAKEFEATURES%" (
	mkdir "%QMAKEFEATURES%"
	if errorlevel 1 exit /b 1
)

echo WINLOCAL_PREFIX = %INSTALLPREFIX%\openssl-install > "%QMAKEFEATURES%"\winlocal.prf

%SVN% co -r %QCAVER% svn://anonsvn.kde.org/home/kde/trunk/kdesupport/qca qca
if errorlevel 1 exit /b 1

pushd qca
if errorlevel 1 exit /b 1

echo CONFIG += release > conf.pri
echo QCA_NO_TESTS = 1 >> conf.pri
if errorlevel 1 exit /b 1

echo CONFIG += release > confapp.pri
if errorlevel 1 exit /b 1

rem It only creates and installs qmake crypto feature, crypto.prf and is
rem actually needed so early.
call .\installwin.bat
if errorlevel 1 exit /b 1

qmake
if errorlevel 1 exit /b 1

nmake
if errorlevel 1 exit /b 1

pushd plugins\qca-ossl
if errorlevel 1 exit /b 1

call .\configwin.bat r
if errorlevel 1 exit /b 1

qmake
if errorlevel 1 exit /b 1

nmake
if errorlevel 1 exit /b 1

popd
if errorlevel 1 exit /b 1

popd
if errorlevel 1 exit /b 1

set PATH=%OLDPATH%

%CP% "%INSTALLPREFIX%"\qca\lib\qca2.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\qt-plugins\crypto mkdir "%INSTALLBASE%"\qt-plugins\crypto
%CP% "%INSTALLPREFIX%"\qca\plugins\qca-ossl\lib\qca-ossl2.dll "%INSTALLBASE%"\qt-plugins\crypto
if errorlevel 1 exit /b 1
