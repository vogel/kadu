@echo off

echo.
echo Building qt
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" build-dependency openssl
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set QTDIR=%INSTALLPREFIX%\qt
set PATH=%QTDIR%\bin;%INSTALLPREFIX%\zlib-install\bin;%PATH%
set QMAKEFEATURES=%INSTALLPREFIX%\openssl-install\delta-winlocal

if not exist "%QMAKEFEATURES%" (
	mkdir "%QMAKEFEATURES%"
	if errorlevel 1 goto fail
)

echo WINLOCAL_PREFIX = %INSTALLPREFIX%\openssl-install > "%QMAKEFEATURES%"\winlocal.prf

call "%~dp0\..\utils.bat" load-result qca QCA
if errorlevel 1 goto fail

if %QCA_RESULT% EQU 1 goto downloaded
if %QCA_RESULT% EQU 2 (
	pushd qca
	if errorlevel 1 goto fail
	goto built
)
if %QCA_RESULT% EQU 3 goto ready

%GIT% clone git://anongit.kde.org/qca.git qca
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result qca 1
if errorlevel 1 goto fail

:downloaded

pushd qca
if errorlevel 1 goto fail

%GIT% checkout %QCAVER%

echo CONFIG += %QTMODE% > conf.pri
echo QCA_NO_TESTS = 1 >> conf.pri
if errorlevel 1 goto fail2

echo CONFIG += %QTMODE% > confapp.pri
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

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

echo CONFIG += %QTMODE% > conf_win.pri
if errorlevel 1 goto fail2

qmake
if errorlevel 1 goto fail3

nmake
if errorlevel 1 goto fail3

popd
if errorlevel 1 goto fail3

popd
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result qca 2
if errorlevel 1 goto fail

:built

%CP% "%INSTALLPREFIX%"\qca\lib\qca2.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\crypto mkdir "%INSTALLBASE%"\qt-plugins\crypto
%CP% "%INSTALLPREFIX%"\qca\plugins\qca-ossl\lib\qca-ossl2.dll "%INSTALLBASE%"\qt-plugins\crypto
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result qca 3
if errorlevel 1 goto fail

:ready

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
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
