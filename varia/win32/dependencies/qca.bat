@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist qca (
	echo qca directory already exists, skipping...
	goto end
)

set OLDPATH=%PATH%
set QTDIR=%INSTALLPREFIX%\qt
set PATH=%QTDIR%\bin;%INSTALLPREFIX%\zlib-install\bin;%PATH%
set QMAKEFEATURES=%INSTALLPREFIX%\openssl-install\delta-winlocal

if not exist "%QMAKEFEATURES%" (
	mkdir "%QMAKEFEATURES%"
	if errorlevel 1 goto fail
)

echo WINLOCAL_PREFIX = %INSTALLPREFIX%\openssl-install > "%QMAKEFEATURES%"\winlocal.prf

%GIT% clone git://anongit.kde.org/qca.git qca
if errorlevel 1 goto fail

pushd qca
if errorlevel 1 goto fail

%GIT% checkout %QCAVER%

echo CONFIG += release > conf.pri
echo QCA_NO_TESTS = 1 >> conf.pri
if errorlevel 1 goto fail2

echo CONFIG += release > confapp.pri
if errorlevel 1 goto fail2

rem It only creates and installs qmake crypto feature, crypto.prf and is
rem actually needed so early.
call .\installwin.bat
if errorlevel 1 goto fail2

qmake
if errorlevel 1 goto fail2

nmake
if errorlevel 1 goto fail2

pushd plugins\qca-ossl
if errorlevel 1 goto fail2

call .\configwin.bat r
if errorlevel 1 goto fail3

qmake
if errorlevel 1 goto fail3

nmake
if errorlevel 1 goto fail3

popd
if errorlevel 1 goto fail3

popd
if errorlevel 1 goto fail2

set PATH=%OLDPATH%

%CP% "%INSTALLPREFIX%"\qca\lib\qca2.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\crypto mkdir "%INSTALLBASE%"\qt-plugins\crypto
%CP% "%INSTALLPREFIX%"\qca\plugins\qca-ossl\lib\qca-ossl2.dll "%INSTALLBASE%"\qt-plugins\crypto
if errorlevel 1 goto fail

echo.
echo qca build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo qca: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
