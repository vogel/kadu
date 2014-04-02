@echo off

call "%~dp0\utils.bat" load-config
if errorlevel 1 goto fail

call "%~dp0\global-config.bat"
if errorlevel 1 goto fail

call "%~dp0\utils.bat" build-dependency kadu
if errorlevel 1 goto fail

rem Prepare Kadu installation directory.

%CP% "%VS120COMNTOOLS%"\..\..\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%VS120COMNTOOLS%"\..\..\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%INSTALLPREFIX%"\libidn\COPYINGv3 "%INSTALLBASE%"\COPYING
if errorlevel 1 goto fail

rem Build and install Kadu.

set QTDIR=%INSTALLPREFIX%\qt

pushd "%KADUROOT%"
if errorlevel 1 goto fail

if exist install goto pastkadu
	if not exist build (
		mkdir build
		if errorlevel 1 goto fail2
	)

	pushd build
	if errorlevel 1 goto fail2

	%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%KADUROOT%\install" -DKADU_SDK_DIR:PATH="%KADUROOT%\sdk" -DCMAKE_PREFIX_PATH="%INSTALLPREFIX%\qca";"%INSTALLPREFIX%\libarchive-install";"%INSTALLPREFIX%\openssl-install" -DZLIB_ROOT:PATH="%INSTALLPREFIX%\zlib-install" -DWIN_LIBGADU_DIR:PATH="%INSTALLPREFIX%\libgadu-install" -DWIN_IDN_DIR:PATH="%INSTALLPREFIX%\libidn" -DWIN_ASPELL_DIR:PATH="%INSTALLPREFIX%\aspell-install" "%KADUGIT%" -DCOMPILE_PLUGINS="%KADU_PLUGINS%"
	if errorlevel 1 goto fail3
	%CMAKE_MAKE%
	if errorlevel 1 goto fail3
	%CMAKE_MAKE_INSTALL%
	if errorlevel 1 goto fail3

	popd
	if errorlevel 1 goto fail3
:pastkadu

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

popd
if errorlevel 1 goto fail2

echo.
echo Kadu build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo Kadu build: Error encountered
echo.

:end
%COMSPEC% /k
exit
