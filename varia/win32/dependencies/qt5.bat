if exist qt5 (
	echo qt5 directory already exists, skipping...
	exit /b
)

set OLDPATH=%PATH%
set OLDINCLUDE=%INCLUDE%
set OLDLIB=%LIB%

rem Download dependencies.

if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32\python.exe goto pastpython
	if not exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z (
		%WGET% http://python-common-portable.googlecode.com/files/python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
		if errorlevel 1 exit /b 1
	)

	if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32 %MY_RMDIR% python-%PYTHONCOMMONPORTABLEVER%-bin-win32
	mkdir python-%PYTHONCOMMONPORTABLEVER%-bin-win32

	%SEVENZ% x -opython-%PYTHONCOMMONPORTABLEVER%-bin-win32 python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
	if errorlevel 1 exit /b 1
:pastpython

set PATH=%CD%\python-%PYTHONCOMMONPORTABLEVER%-bin-win32;%PATH%

if exist ruby-%RUBYVER%-i386-mingw32\bin\ruby.exe goto pastruby
	if not exist ruby-%RUBYVER%-i386-mingw32.7z (
		%WGET% -O ruby-%RUBYVER%-i386-mingw32.7z http://dl.bintray.com/oneclick/rubyinstaller/ruby-%RUBYVER%-i386-mingw32.7z?direct
		if errorlevel 1 exit /b 1
	)

	if exist ruby-%RUBYVER%-i386-mingw32 %MY_RMDIR% ruby-%RUBYVER%-i386-mingw32

	%SEVENZ% x ruby-%RUBYVER%-i386-mingw32.7z
	if errorlevel 1 exit /b 1
:pastruby

set PATH=%CD%\ruby-%RUBYVER%-i386-mingw32\bin;%PATH%

rem Now Qt itself.

%GIT% clone git://gitorious.org/qt/qt5.git qt5
if errorlevel 1 exit /b 1

pushd qt5
if errorlevel 1 exit /b 1

%GIT% checkout %QTVER%
if errorlevel 1 exit /b 1

%PERL% init-repository
if errorlevel 1 exit /b 1

%SED% -i -e "s/SUBSYSTEM:CONSOLE$/SUBSYSTEM:CONSOLE,5.01/" -e "s/SUBSYSTEM:WINDOWS$/SUBSYSTEM:WINDOWS,5.01/" qtbase\mkspecs\win32-msvc2013\qmake.conf
if errorlevel 1 exit /b 1

%SED% -i -e "s/_MSC_VER >= 1700/0/" -e "s/_MSC_VER < 1700/1/" qtbase\src\corelib\io\qfilesystemengine_win.cpp qtbase\src\corelib\kernel\qeventdispatcher_win.cpp
if errorlevel 1 exit /b 1

%SED% -i -e "s/"""v120"""/"""%PlatformToolset%"""/" qtbase\qmake\generators\win32\msbuild_objectmodel.cpp
if errorlevel 1 exit /b 1

rem Provide ICU paths
set PATH=%PATH%;%INSTALLPREFIX%\icu-install\lib
set INCLUDE=%INCLUDE%;%INSTALLPREFIX%\icu-install\include
set LIB=%LIB%;%INSTALLPREFIX%\icu-install\lib

rem qmake.exe will need zlib1.dll in PATH
set PATH=%INSTALLPREFIX%\zlib-install\bin;%PATH%

configure -release -force-debug-info -opensource -confirm-license -c++11 -shared -ltcg -nomake examples -nomake tests -skip qtdeclarative -skip qtquickcontrols -skip qtlocation -skip qtsensors -skip qtconnectivity -skip qtgraphicaleffects -skip qtserialport -no-compile-examples -plugin-sql-sqlite -platform win32-msvc2013 -no-warnings-are-errors -I "%INSTALLPREFIX%"\zlib-install\include -L "%INSTALLPREFIX%"\zlib-install\lib -l zlib -I "%INSTALLPREFIX%"\openssl-install\include -L "%INSTALLPREFIX%"\openssl-install\lib OPENSSL_LIBS="-lssleay32 -llibeay32" -system-zlib -icu -qt-harfbuzz -openssl-linked -no-qml-debug -no-style-fusion -mp
if errorlevel 1 exit /b 1

nmake
if errorlevel 1 exit /b 1

pushd qtwebkit
if errorlevel 1 exit /b 1

%GIT% reset --hard %QTWEBKITVER%
if errorlevel 1 exit /b 1

..\qtbase\bin\qmake
if errorlevel 1 exit /b 1

nmake
if errorlevel 1 exit /b 1

popd
popd

set LIB=%OLDLIB%
set INCLUDE=%OLDINCLUDE%
set PATH=%OLDPATH%

%MY_CP% "%QTDIR%"\lib\libEGL.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\libGLESv2.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Concurrent.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Core.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Declarative.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Gui.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Network.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Script.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Sql.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Svg.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5WebKit.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5WebKitWidgets.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Widgets.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5Xml.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%QTDIR%"\lib\Qt5XmlPatterns.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\translations mkdir "%INSTALLBASE%"\translations
rem TODO: for each %KADUGIT%\translations\*.language
%MY_CP% "%QTDIR%"\translations\qt_cs.qm "%INSTALLBASE%"\translations
if errorlevel 1 exit /b 1
%MY_CP% "%QTDIR%"\translations\qt_de.qm "%INSTALLBASE%"\translations
if errorlevel 1 exit /b 1
%MY_CP% "%QTDIR%"\translations\qt_pl.qm "%INSTALLBASE%"\translations
if errorlevel 1 exit /b 1
%MY_CP% "%QTDIR%"\translations\qt_ru.qm "%INSTALLBASE%"\translations
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\qt-plugins\bearer mkdir "%INSTALLBASE%"\qt-plugins\bearer
%MY_CP% "%QTDIR%"\plugins\bearer\qgenericbearer.dll "%INSTALLBASE%"\qt-plugins\bearer
if errorlevel 1 exit /b 1
%MY_CP% "%QTDIR%"\plugins\bearer\qnativewifibearer.dll "%INSTALLBASE%"\qt-plugins\bearer
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\qt-plugins\iconengines mkdir "%INSTALLBASE%"\qt-plugins\iconengines
%MY_CP% "%QTDIR%"\plugins\iconengines\qsvgicon.dll "%INSTALLBASE%"\qt-plugins\iconengines
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\qt-plugins\imageformats mkdir "%INSTALLBASE%"\qt-plugins\imageformats
%MY_CP% "%QTDIR%"\plugins\imageformats\qgif.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 exit /b 1
%MY_CP% "%QTDIR%"\plugins\imageformats\qjpeg.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 exit /b 1
%MY_CP% "%QTDIR%"\plugins\imageformats\qsvg.dll "%INSTALLBASE%"\qt-plugins\imageformats
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\qt-plugins\platforms mkdir "%INSTALLBASE%"\qt-plugins\platforms
%MY_CP% "%QTDIR%"\plugins\platforms\qwindows.dll "%INSTALLBASE%"\qt-plugins\platforms
if errorlevel 1 exit /b 1

if not exist "%INSTALLBASE%"\qt-plugins\sqldrivers mkdir "%INSTALLBASE%"\qt-plugins\sqldrivers
%MY_CP% "%QTDIR%"\plugins\sqldrivers\qsqlite.dll "%INSTALLBASE%"\qt-plugins\sqldrivers
if errorlevel 1 exit /b 1
