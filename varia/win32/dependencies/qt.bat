@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist qt (
	echo qt directory already exists, skipping...
	exit /b
)

rem if not exist qt-everywhere-opensource-src-%QTVER%.tar.gz (
rem 	%WGET% http://releases.qt-project.org/qt4/source/qt-everywhere-opensource-src-%QTVER%.tar.gz
rem 	if errorlevel 1 exit /b 1
rem )

if not exist qt.git-bare (
	%GIT% clone --bare git://gitorious.org/qt/qt.git qt.git-bare
	if errorlevel 1 goto fail
) else (
	pushd qt.git-bare
	if errorlevel 1 goto fail
	%GIT% fetch --all
	if errorlevel 1 goto fail2
	popd
)

if not exist sqlite-amalgamation-%SQLITEVER%.zip (
	%WGET% http://www.sqlite.org/%SQLITEYEAR%/sqlite-amalgamation-%SQLITEVER%.zip
	if errorlevel 1 goto fail
)

rem if exist qt-everywhere-opensource-src-%QTVER%.tar %RM% qt-everywhere-opensource-src-%QTVER%.tar
rem %SEVENZ% x qt-everywhere-opensource-src-%QTVER%.tar.gz
rem if errorlevel 1 exit /b 1
rem %SEVENZ% x qt-everywhere-opensource-src-%QTVER%.tar
rem if errorlevel 1 exit /b 1
rem %RM% qt-everywhere-opensource-src-%QTVER%.tar
rem 
rem rename qt-everywhere-opensource-src-%QTVER% qt
rem if errorlevel 1 exit /b 1

if not exist qt (
	%GIT% clone .\qt.git-bare qt
	if errorlevel 1 goto fail
)

pushd qt
if errorlevel 1 goto fail

%GIT% checkout %QTVER%
%GIT% cherry-pick 158caa355f459480e56dec0dc6aadfd792084a00
if errorlevel 1 goto fail2

rem TODO: check thread-safety, 2156f7057df5c748b51a7fd16a044f39c60b872 in qt.git
rem %SEVENZ% e ..\sqlite-amalgamation-%SQLITEVER%.zip -osrc\3rdparty\sqlite -y
rem if errorlevel 1 goto fail2

rem %PATCH% -p1 < "%~dp0"\patches\qt-stylesheet.patch
rem if errorlevel 1 goto fail2

%SED% -i -e "s/zdll.lib/zlib.lib/g" src/3rdparty/zlib_dependency.pri src/tools/bootstrap/bootstrap.pri
if errorlevel 1 goto fail2

%SED% -i -e "s/SUBSYSTEM:CONSOLE$/SUBSYSTEM:CONSOLE,5.01/" -e "s/SUBSYSTEM:WINDOWS$/SUBSYSTEM:WINDOWS,5.01/" mkspecs\win32-msvc2012\qmake.conf
if errorlevel 1 goto fail2

%SED% -i -e "s/"""v110"""/"""%PlatformToolset%"""/" qmake\generators\win32\msbuild_objectmodel.cpp
if errorlevel 1 goto fail2

rem qmake.exe will need zlib1.dll in PATH
set OLDPATH=%PATH%
set PATH=%INSTALLPREFIX%\zlib-install\bin;%PATH%

configure -release -opensource -confirm-license -shared -ltcg -no-accessibility -plugin-sql-sqlite -no-qt3support -no-opengl -no-openvg -platform win32-msvc2012 -I "%INSTALLPREFIX%"\zlib-install\include -L "%INSTALLPREFIX%"\zlib-install\lib -l zlib -I "%INSTALLPREFIX%"\openssl-install\include -L "%INSTALLPREFIX%"\openssl-install\lib OPENSSL_LIBS="-lssleay32 -llibeay32" -system-zlib -qt-libpng -no-libmng -no-libtiff -qt-libjpeg -openssl-linked -no-dbus -phonon -phonon-backend -no-multimedia -no-webkit -script -scripttools -declarative -arch windows -no-style-plastique -no-style-cleanlooks -no-style-motif -no-style-cde -nomake demos -nomake examples -nomake tests -nomake tools -mp
if errorlevel 1 goto fail2

nmake
if errorlevel 1 goto fail2

set QTDIR=%CD%

popd
if errorlevel 1 goto fail

set PATH=%OLDPATH%

%CP% "%QTDIR%"\lib\phonon4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtCore4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtDeclarative4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtGui4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtNetwork4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtScript4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtSql4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtSvg4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtXml4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\QtXmlPatterns4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%QTDIR%"\lib\phonon4.dll "%INSTALLBASE%"
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
%CP% "%QTDIR%"\plugins\bearer\qgenericbearer4.dll "%INSTALLBASE%"\qt-plugins\bearer
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\bearer\qnativewifibearer4.dll "%INSTALLBASE%"\qt-plugins\bearer
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\iconengines mkdir "%INSTALLBASE%"\qt-plugins\iconengines
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\iconengines\qsvgicon4.dll "%INSTALLBASE%"\qt-plugins\iconengines
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\imageformats mkdir "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\imageformats\qgif4.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\imageformats\qjpeg4.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\imageformats\qsvg4.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\phonon_backend mkdir "%INSTALLBASE%"\qt-plugins\phonon_backend
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\phonon_backend\phonon_ds94.dll "%INSTALLBASE%"\qt-plugins\phonon_backend
if errorlevel 1 goto fail

if not exist "%INSTALLBASE%"\qt-plugins\sqldrivers mkdir "%INSTALLBASE%"\qt-plugins\sqldrivers
if errorlevel 1 goto fail
%CP% "%QTDIR%"\plugins\sqldrivers\qsqlite4.dll "%INSTALLBASE%"\qt-plugins\sqldrivers
if errorlevel 1 goto fail

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
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
