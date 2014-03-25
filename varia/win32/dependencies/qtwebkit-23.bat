@echo off

echo.
echo Building qt
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-python
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-perl
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-ruby
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-winflexbison
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-gperf
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-grep
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result qtwebkit QTWEBKIT
if errorlevel 1 goto fail2

if %QTWEBKIT_RESULT% EQU 1 goto cloned
if %QTWEBKIT_RESULT% EQU 2 (
	pushd qtwebkit-23
	if errorlevel 1 goto fail2
	goto version-selected
)
if %QTWEBKIT_RESULT% EQU 3 (
	pushd qtwebkit-23
	if errorlevel 1 goto fail2
	goto patched
)
if %QTWEBKIT_RESULT% EQU 4 (
	pushd qtwebkit-23
	if errorlevel 1 goto fail2
	goto perl-finished
)
if %QTWEBKIT_RESULT% EQU 5 goto make-passed
if %QTWEBKIT_RESULT% EQU 6 goto ready

rem if exist qtwebkit-23 %RMDIR% qtwebkit-23
rem if errorlevel 1 goto fail2

%GIT% clone git://gitorious.org/webkit/qtwebkit-23.git qtwebkit-23
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result qtwebkit 1
if errorlevel 1 goto fail2

:cloned

rem if exist pax_global_header %RM% pax_global_header
rem if exist webkit-qtwebkit-23 %RMDIR% webkit-qtwebkit-23
rem if exist qtwebkit-%QTWEBKIT23VER%.tar %RM% qtwebkit-%QTWEBKIT23VER%.tar
rem %SEVENZ% x qtwebkit-%QTWEBKIT23VER%.tar.gz
rem if errorlevel 1 exit /b 1
rem %SEVENZ% x qtwebkit-%QTWEBKIT23VER%.tar
rem if errorlevel 1 exit /b 1
rem %RM% qtwebkit-%QTWEBKIT23VER%.tar
rem %RM% pax_global_header

rem rename webkit-qtwebkit-23 qtwebkit-23
rem if errorlevel 1 exit /b 1

pushd qtwebkit-23
if errorlevel 1 goto fail2

%GIT% checkout qtwebkit-%QTWEBKIT23VER%
if errorlevel 1 goto fail3

%GIT% reset --hard
if errorlevel 1 goto fail3

call "%~dp0\..\utils.bat" store-result qtwebkit 2
if errorlevel 1 goto fail3

:version-selected

%PATCH% -p1 < "%~dp0"\patches\qtwebkit-msvc2013.patch
if errorlevel 1 goto fail3

call "%~dp0\..\utils.bat" store-result qtwebkit 3
if errorlevel 1 goto fail3

:patched

rem This is needed to disable LTCG also on MSVC2012. Otherwise linking fails due to too big objects.
%SED% -i -e "s/win32-msvc2005|win32-msvc2008|win32-msvc2010|wince/win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|wince/" Source\WebCore\WebCore.pri
if errorlevel 1 goto fail2

rem Don't build DumpRenderTree, linking fails.
%SED% -i -e "/DumpRenderTree\//d" Tools\Tools.pro
if errorlevel 1 goto fail2

set QTDIR=%INSTALLPREFIX%\qt
set SQLITE3SRCDIR=%QTDIR%\src\3rdparty\sqlite
set PATH=%QTDIR%\bin;%INSTALLPREFIX%\zlib-install\bin;%PATH%

rem debug: --inspector --javascript-debugger?
rem TODO all: --svg?
echo Calling perl

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

%PERL% Tools\Scripts\build-webkit --qt --release --no-webkit2 --no-force-sse2 --minimal --netscape-plugin-api
if errorlevel 1 goto fail3

call "%~dp0\..\utils.bat" store-result qtwebkit 4
if errorlevel 1 goto fail3

:perl-finished

pushd WebKitBuild\Release
if errorlevel 1 goto fail3

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

nmake install
if errorlevel 1 goto fail4

popd
if errorlevel 1 goto fail4

popd
if errorlevel 1 goto fail3

call "%~dp0\..\utils.bat" store-result qtwebkit 5
if errorlevel 1 goto fail2

:make-passed

%CP% "%QTDIR%"\lib\QtWebKit4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result qtwebkit 6
if errorlevel 1 goto fail2

:ready

echo.
echo qtwebkit build: Success
echo.
goto end

:fail4
popd

:fail3
popd

:fail2
popd

:fail
echo.
echo qtwebkit: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"

:utils-not-found
echo.
echo utils.bat file not found
echo.
pause
exit
