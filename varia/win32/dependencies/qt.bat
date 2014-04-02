@echo off

echo.
echo Building qt
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-perl
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result qt QT
if errorlevel 1 goto fail

if %QT_RESULT% EQU 1 goto downloaded
if %QT_RESULT% EQU 2 goto patch-downloaded
if %QT_RESULT% EQU 3 (
	pushd qt
	if errorlevel 1 goto fail
	goto version-checked-out
)
if %QT_RESULT% EQU 4 (
	pushd qt
	if errorlevel 1 goto fail
	goto patched
)
if %QT_RESULT% EQU 5 (
	pushd qt
	if errorlevel 1 goto fail
	goto configured
)
if %QT_RESULT% EQU 6 goto compiled
if %QT_RESULT% EQU 7 goto ready

if exist qt %RMDIR% qt
if errorlevel 1 goto fail

:reclone
%GIT% clone git://gitorious.org/qt/qt.git qt
if errorlevel 1 goto reclone

call "%~dp0\..\utils.bat" store-result qt 1
if errorlevel 1 goto fail

:downloaded

if exist sqlite-amalgamation-%SQLITEVER%.zip %RM% sqlite-amalgamation-%SQLITEVER%.zip
if errorlevel 1 goto fail

%WGET% http://www.sqlite.org/%SQLITEYEAR%/sqlite-amalgamation-%SQLITEVER%.zip
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result qt 2
if errorlevel 1 goto fail

:patch-downloaded

pushd qt
if errorlevel 1 goto fail

%GIT% checkout %QTVER%
if errorlevel 1 goto fail2

rem javascript and msvc bug
%GIT% cherry-pick 158caa355f459480e56dec0dc6aadfd792084a00
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result qt 3
if errorlevel 1 goto fail

:version-checked-out

rem TODO: check thread-safety, 2156f7057df5c748b51a7fd16a044f39c60b872 in qt.git
%SEVENZ% e ..\sqlite-amalgamation-%SQLITEVER%.zip -osrc\3rdparty\sqlite -y
if errorlevel 1 goto fail2

%PATCH% -p1 < "%~dp0"\patches\qt-stylesheet.patch
if errorlevel 1 goto fail2

%SED% -i -e "s/zdll.lib/zlib%LIBSUFFIX%.lib/g" src/3rdparty/zlib_dependency.pri src/tools/bootstrap/bootstrap.pri
if errorlevel 1 goto fail2

%SED% -i -e "s/SUBSYSTEM:CONSOLE$/SUBSYSTEM:CONSOLE,5.01/" -e "s/SUBSYSTEM:WINDOWS$/SUBSYSTEM:WINDOWS,5.01/" mkspecs\%QMAKESPEC%\qmake.conf
if errorlevel 1 goto fail2

%SED% -i -e "s/"""v110"""/"""%PlatformToolset%"""/" qmake\generators\win32\msbuild_objectmodel.cpp
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result qt 4
if errorlevel 1 goto fail

:patched

rem qmake.exe will need zlib1.dll in PATH
set PATH=%INSTALLPREFIX%\zlib-install\bin;%PATH%

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

configure -%QTMODE% -opensource -confirm-license -shared -ltcg -no-accessibility -plugin-sql-sqlite -no-qt3support -no-opengl -no-openvg -platform %QMAKESPEC% -I "%INSTALLPREFIX%"\zlib-install\include -L "%INSTALLPREFIX%"\zlib-install\lib -l zlib%LIBSUFFIX% -I "%INSTALLPREFIX%"\openssl-install\include -L "%INSTALLPREFIX%"\openssl-install\lib OPENSSL_LIBS="-lssleay32 -llibeay32" -system-zlib -qt-libpng -no-libmng -no-libtiff -qt-libjpeg -openssl-linked -no-dbus -phonon -phonon-backend -no-multimedia -no-webkit -script -scripttools -declarative -arch windows -no-style-plastique -no-style-cleanlooks -no-style-motif -no-style-cde -nomake demos -nomake examples -nomake tests -nomake tools -mp
if errorlevel 1 goto fail2

call "%~dp0\..\utils.bat" store-result qt 5
if errorlevel 1 goto fail

:configured

call "%~dp0\..\utils.bat" enable-msvc
if errorlevel 1 goto fail

nmake
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result qt 6
if errorlevel 1 goto fail

:compiled

set QTDIR=%CD%\qt

echo %CP% "%QTDIR%"\lib\phonon%LIBSUFFIX%4.dll "%INSTALLBASE%"
%CP% "%QTDIR%"\lib\phonon%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtCore%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtDeclarative%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtGui%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtNetwork%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtScript%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtSql%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtSvg%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtXml%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtXmlPatterns%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\phonon%LIBSUFFIX%4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\translations mkdir "%INSTALLBASE%"\translations
if errorlevel 1 goto fail

rem TODO: for each %KADUGIT%\translations\*.language
%CP% "%QTDIR%"\translations\qt_cs.qm "%INSTALLBASE%"\translations
if errorlevel 1 goto fail
%CP% "%QTDIR%"\translations\qt_de.qm "%INSTALLBASE%"\translations
if errorlevel 1 goto fail
%CP% "%QTDIR%"\translations\qt_pl.qm "%INSTALLBASE%"\translations
if errorlevel 1 goto fail
%CP% "%QTDIR%"\translations\qt_ru.qm "%INSTALLBASE%"\translations
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\bearer mkdir "%INSTALLBASE%"\qt-plugins\bearer
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\bearer\qgenericbearer%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\bearer
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\bearer\qnativewifibearer%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\bearer
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\iconengines mkdir "%INSTALLBASE%"\qt-plugins\iconengines
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\iconengines\qsvgicon%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\iconengines
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\imageformats mkdir "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\imageformats\qgif%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\imageformats\qjpeg%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\imageformats\qsvg%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\phonon_backend mkdir "%INSTALLBASE%"\qt-plugins\phonon_backend
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\phonon_backend\phonon_ds9%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\phonon_backend
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\sqldrivers mkdir "%INSTALLBASE%"\qt-plugins\sqldrivers
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\sqldrivers\qsqlite%LIBSUFFIX%4.dll "%INSTALLBASE%"\qt-plugins\sqldrivers
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result qt 7
if errorlevel 1 goto fail

:ready

echo.
echo qt build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo qt: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
