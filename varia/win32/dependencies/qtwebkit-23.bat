if exist qtwebkit-23 (
	echo qtwebkit-23 directory already exists, skipping...
	exit /b
)

set OLDPATH=%PATH%

rem Download dependencies.

if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32\python.exe goto pastpython
	if not exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z (
		%WGET% http://python-common-portable.googlecode.com/files/python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
		if errorlevel 1 exit /b 1
	)

	if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32 %RMDIR% python-%PYTHONCOMMONPORTABLEVER%-bin-win32
	mkdir python-%PYTHONCOMMONPORTABLEVER%-bin-win32

	%SEVENZ% x -opython-%PYTHONCOMMONPORTABLEVER%-bin-win32 python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
	if errorlevel 1 exit /b 1
:pastpython

set PATH=%CD%\python-%PYTHONCOMMONPORTABLEVER%-bin-win32;%PATH%

if exist ruby-%RUBYVER%-i386-mingw32\bin\ruby.exe goto pastruby
	if not exist ruby-%RUBYVER%-i386-mingw32.7z (
		%WGET% http://rubyforge.org/frs/download.php/76807/ruby-%RUBYVER%-i386-mingw32.7z
		if errorlevel 1 exit /b 1
	)

	if exist ruby-%RUBYVER%-i386-mingw32 %RMDIR% ruby-%RUBYVER%-i386-mingw32

	%SEVENZ% x ruby-%RUBYVER%-i386-mingw32.7z
	if errorlevel 1 exit /b 1
:pastruby

set PATH=%CD%\ruby-%RUBYVER%-i386-mingw32\bin;%PATH%

if exist winflexbison\bison.exe goto pastwinflexbison
	if not exist win_flex_bison-latest.zip (
		%WGET% http://downloads.sourceforge.net/project/winflexbison/win_flex_bison-latest.zip
		if errorlevel 1 exit /b 1
	)

	if exist winflexbison %RMDIR% winflexbison
	mkdir winflexbison

	%SEVENZ% x -owinflexbison win_flex_bison-latest.zip
	if errorlevel 1 exit /b 1

	rename winflexbison\win_bison.exe bison.exe
	if errorlevel 1 exit /b 1
:pastwinflexbison

set PATH=%CD%\winflexbison;%PATH%

if not exist gnuwin32 mkdir gnuwin32
set PATH=%CD%\gnuwin32\bin;%PATH%

if exist gnuwin32\bin\gperf.exe goto pastgperf
	if not exist gperf-%GNUWIN32GPERFVER%-bin.zip (
		%WGET% http://sourceforge.net/projects/gnuwin32/files/gperf/%GNUWIN32GPERFVER%/gperf-%GNUWIN32GPERFVER%-bin.zip/download
		if errorlevel 1 exit /b 1
	)

	%SEVENZ% x -ognuwin32 gperf-%GNUWIN32GPERFVER%-bin.zip
	if errorlevel 1 exit /b 1
:pastgperf

if exist gnuwin32\bin\grep.exe goto pastgrep
	if not exist grep-%GNUWIN32GREPVER%-dep.zip (
		%WGET% http://sourceforge.net/projects/gnuwin32/files/grep/%GNUWIN32GREPVER%/grep-%GNUWIN32GREPVER%-dep.zip/download
		if errorlevel 1 exit /b 1
	)

	if not exist grep-%GNUWIN32GREPVER%-bin.zip (
		%WGET% http://sourceforge.net/projects/gnuwin32/files/grep/%GNUWIN32GREPVER%/grep-%GNUWIN32GREPVER%-bin.zip/download
		if errorlevel 1 exit /b 1
	)

	%SEVENZ% x -ognuwin32 grep-%GNUWIN32GREPVER%-dep.zip
	if errorlevel 1 exit /b 1

	%SEVENZ% x -ognuwin32 grep-%GNUWIN32GREPVER%-bin.zip
	if errorlevel 1 exit /b 1
:pastgrep

rem Now QtWebKit itself.

set TARBALL=qtwebkit-%QTWEBKIT23VER%.tar.gz
rem FOR does not seem to work inside IF, so I use GOTO.
if exist %TARBALL% goto pastdownload
	rem Beware of HTTP 202 Accept: Gitorious will be preparing the tarball and
	rem send small text instead of it meanwhile.
	
:download
	%WGET% -O %TARBALL% http://gitorious.org/webkit/qtwebkit-23/archive-tarball/qtwebkit-%QTWEBKIT23VER%
	if errorlevel 1 exit /b 1
	
	for /F "usebackq" %%A in ('%TARBALL%') do set SIZE=%%~zA
	if %SIZE% LSS 1000000 (
		%RM% %TARBALL%
		if errorlevel 1 exit /b 1
		goto download
	)
:pastdownload

if exist pax_global_header %RM% pax_global_header
if exist webkit-qtwebkit-23 %RMDIR% webkit-qtwebkit-23
if exist qtwebkit-%QTWEBKIT23VER%.tar %RM% qtwebkit-%QTWEBKIT23VER%.tar
%SEVENZ% x qtwebkit-%QTWEBKIT23VER%.tar.gz
if errorlevel 1 exit /b 1
%SEVENZ% x qtwebkit-%QTWEBKIT23VER%.tar
if errorlevel 1 exit /b 1
%RM% qtwebkit-%QTWEBKIT23VER%.tar
%RM% pax_global_header

rename webkit-qtwebkit-23 qtwebkit-23
if errorlevel 1 exit /b 1

pushd qtwebkit-23
if errorlevel 1 exit /b 1

rem         "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\BIN\nmake.exe" -f Makefile.QtWebKit.api
rem 
rem Microsoft (R) Program Maintenance Utility Version 11.00.60315.1
rem Copyright (C) Microsoft Corporation.  All rights reserved.
rem 
rem Project MESSAGE: Running perl C:\Users\beevvy\kadu\dependencies\qtwebkit-23\Tools\qmake\syncqt-4.8 -windows -outdir C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release -separate-module QtWebKit;C:\Users\beevvy\kadu\dependencies\qtwebkit-23\Source;WebKit\qt\Api
rem         (set QMAKEPATH=C:\Users\beevvy\kadu\dependencies\qtwebkit-23\Tools\qmake) && c:\Users\beevvy\kadu\dependencies\qt\bin\qmake.exe CONFIG+=production_build CONFIG+=no_webkit2 CONFIG+=no_force_sse2 DEFINES+=ENABLE_3D_RENDERING=0 DEFINES+=ENABLE_ACCELERATED_2D_CANVAS=0 DEFINES+=ENABLE_ANIMATION_API=0 DEFINES+=ENABLE_BATTERY_STATUS=0 DEFINES+=ENABLE_BLOB=0 DEFINES+=ENABLE_CHANNEL_MESSAGING=0 DEFINES+=ENABLE_CSP_NEXT=0 DEFINES+=ENABLE_CSS_DEVICE_ADAPTATION=0 DEFINES+=ENABLE_CSS_EXCLUSIONS=0 DEFINES+=ENABLE_CSS_FILTERS=0 DEFINES+=ENABLE_CSS3_BACKGROUND=0 DEFINES+=ENABLE_CSS3_CONDITIONAL_RULES=0 DEFINES+=ENABLE_CSS3_TEXT=0 DEFINES+=ENABLE_CSS_BOX_DECORATION_BREAK=0 DEFINES+=ENABLE_CSS_IMAGE_ORIENTATION=0 DEFINES+=ENABLE_CSS_IMAGE_RESOLUTION=0 DEFINES+=ENABLE_CSS_REGIONS=0 DEFINES+=ENABLE_CSS_SHADERS=0 DEFINES+=ENABLE_CSS_COMPOSITING=0 DEFINES+=ENABLE_CSS_VARIABLES=0 DEFINES+=ENABLE_CUSTOM_SCHEME_HANDLER=0 DEFINES+=ENABLE_DATALIST_ELEMENT=0 DEFINES+=ENABLE_DATA_TRANSFER_ITEMS=0 DEFINES+=ENABLE_DETAILS_ELEMENT=0 DEFINES+=ENABLE_DEVICE_ORIENTATION=0 DEFINES+=ENABLE_DIALOG_ELEMENT=0 DEFINES+=ENABLE_DIRECTORY_UPLOAD=0 DEFINES+=ENABLE_DOWNLOAD_ATTRIBUTE=0 DEFINES+=ENABLE_FILE_SYSTEM=0 DEFINES+=ENABLE_FILTERS=0 DEFINES+=ENABLE_FTPDIR=0 DEFINES+=ENABLE_FULLSCREEN_API=0 DEFINES+=ENABLE_GAMEPAD=0 DEFINES+=ENABLE_GEOLOCATION=0 DEFINES+=ENABLE_HIGH_DPI_CANVAS=0 DEFINES+=ENABLE_ICONDATABASE=0 DEFINES+=ENABLE_IFRAME_SEAMLESS=0 DEFINES+=ENABLE_INDEXED_DATABASE=0 DEFINES+=ENABLE_INPUT_SPEECH=0 DEFINES+=ENABLE_INPUT_TYPE_COLOR=0 DEFINES+=ENABLE_INPUT_TYPE_DATE=0 DEFINES+=ENABLE_INPUT_TYPE_DATETIME=0 DEFINES+=ENABLE_INPUT_TYPE_DATETIMELOCAL=0 DEFINES+=ENABLE_INPUT_TYPE_MONTH=0 DEFINES+=ENABLE_INPUT_TYPE_TIME=0 DEFINES+=ENABLE_INPUT_TYPE_WEEK=0 DEFINES+=ENABLE_INSPECTOR=0 DEFINES+=ENABLE_JAVASCRIPT_DEBUGGER=0 DEFINES+=ENABLE_LEGACY_NOTIFICATIONS=0 DEFINES+=ENABLE_LEGACY_VENDOR_PREFIXES=0 DEFINES+=ENABLE_LEGACY_WEB_AUDIO=0 DEFINES+=ENABLE_LINK_PREFETCH=0 DEFINES+=ENABLE_LINK_PRERENDER=0 DEFINES+=ENABLE_MATHML=0 DEFINES+=ENABLE_MEDIA_CAPTURE=0 DEFINES+=ENABLE_MEDIA_SOURCE=0 DEFINES+=ENABLE_MEDIA_STATISTICS=0 DEFINES+=ENABLE_MEDIA_STREAM=0 DEFINES+=ENABLE_METER_ELEMENT=0 DEFINES+=ENABLE_MHTML=0 DEFINES+=ENABLE_MICRODATA=0 DEFINES+=ENABLE_MUTATION_OBSERVERS=0 DEFINES+=ENABLE_NAVIGATOR_CONTENT_UTILS=0 DEFINES+=ENABLE_NETSCAPE_PLUGIN_API=1 DEFINES+=ENABLE_NETWORK_INFO=0 DEFINES+=ENABLE_NOTIFICATIONS=0 DEFINES+=ENABLE_ORIENTATION_EVENTS=0 DEFINES+=ENABLE_PAGE_VISIBILITY_API=0 DEFINES+=ENABLE_PROGRESS_ELEMENT=0 DEFINES+=ENABLE_PROXIMITY_EVENTS=0 DEFINES+=ENABLE_QUOTA=0 DEFINES+=ENABLE_RESOLUTION_MEDIA_QUERY=0 DEFINES+=ENABLE_REQUEST_ANIMATION_FRAME=0 DEFINES+=ENABLE_SCRIPTED_SPEECH=0 DEFINES+=ENABLE_SHADOW_DOM=0 DEFINES+=ENABLE_SHARED_WORKERS=0 DEFINES+=ENABLE_SQL_DATABASE=0 DEFINES+=ENABLE_STYLE_SCOPED=0 DEFINES+=ENABLE_SVG=0 DEFINES+=ENABLE_SVG_DOM_OBJC_BINDINGS=0 DEFINES+=ENABLE_SVG_FONTS=0 DEFINES+=USE_SYSTEM_MALLOC=0 DEFINES+=ENABLE_TEMPLATE_ELEMENT=0 DEFINES+=ENABLE_TEXT_AUTOSIZING=0 DEFINES+=WTF_USE_TILED_BACKING_STORE=0 DEFINES+=ENABLE_TOUCH_EVENTS=0 DEFINES+=ENABLE_TOUCH_SLIDER=0 DEFINES+=ENABLE_TOUCH_ICON_LOADING=0 DEFINES+=ENABLE_VIBRATION=0 DEFINES+=ENABLE_VIDEO=0 DEFINES+=ENABLE_VIDEO_TRACK=0 DEFINES+=ENABLE_WEBGL=0 DEFINES+=ENABLE_WEB_AUDIO=0 DEFINES+=ENABLE_WEB_INTENTS=0 DEFINES+=ENABLE_WEB_INTENTS_TAG=0 DEFINES+=ENABLE_WEB_SOCKETS=0 DEFINES+=ENABLE_WEB_TIMING=0 DEFINES+=ENABLE_WORKERS=0 DEFINES+=ENABLE_XHR_RESPONSE_BLOB=0 DEFINES+=ENABLE_XHR_TIMEOUT=0 DEFINES+=ENABLE_XSLT=0 CONFIG+=release CONFIG-=debug CONFIG+=production_build -o Makefile.QtWebKit.api c:\Users\beevvy\kadu\dependencies\qtwebkit-23\Source\api.pri
rem WARNING: Failure to find: \InspectorBackendCommands.qrc
rem WARNING: Failure to find: \InspectorBackendCommands.qrc
rem c:\Users\beevvy\kadu\dependencies\qt\bin\rcc.exe: File does not exist '\InspectorBackendCommands.qrc'
rem c:\Users\beevvy\kadu\dependencies\qt\bin\rcc.exe: File does not exist '\InspectorBackendCommands.qrc'
rem         "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\BIN\nmake.exe" -f Makefile.QtWebKit.api.Release
rem 
rem Microsoft (R) Program Maintenance Utility Version 11.00.60315.1
rem Copyright (C) Microsoft Corporation.  All rights reserved.
rem 
rem         python C:/Users/beevvy/kadu/dependencies/qtwebkit-23\Tools\Scripts\generate-win32-export-forwards C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/WebKit/release\WebKit1.lib C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/WebCore/release\WebCore.lib C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/JavaScriptCore/release\JavaScriptCore.lib C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/WTF/release\WTF.lib forwarded-exports.cpp
rem Forwarding 812 symbols from C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/WebKit/release\WebKit1.lib C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/WebCore/release\WebCore.lib C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/JavaScriptCore/release\JavaScriptCore.lib C:\Users\beevvy\kadu\dependencies\qtwebkit-23\WebKitBuild\Release/Source/WTF/release\WTF.lib
rem NMAKE : fatal error U1073: don't know how to make '\InspectorBackendCommands.qrc'
rem Stop.
rem NMAKE : fatal error U1077: '"C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\BIN\nmake.exe"' : return code '0x2'
rem Stop.
rem NMAKE : fatal error U1077: '"C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\BIN\nmake.exe"' : return code '0x2'
rem Stop.
rem NMAKE : fatal error U1077: 'cd' : return code '0x2'
rem Stop.
%PATCH% -p1 -R < "%~dp0"\patches\qtwebkit-7349c1125423555d908d89a0fd1548cf5cee2de3.patch
if errorlevel 1 exit /b 1

rem This is needed to disable LTCG also on MSVC2012. Otherwise linking fails due to too big objects.
%SED% -i -e "s/win32-msvc2005|win32-msvc2008|win32-msvc2010|wince/win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|wince/" Source\WebCore\WebCore.pri
if errorlevel 1 exit /b 1

rem Don't build DumpRenderTree, linking fails.
%SED% -i -e "/DumpRenderTree\//d" Tools\Tools.pro
if errorlevel 1 exit /b 1

set QTDIR=%INSTALLPREFIX%\qt
set SQLITE3SRCDIR=%QTDIR%\src\3rdparty\sqlite
set PATH=%QTDIR%\bin;%INSTALLPREFIX%\zlib-install\bin;%PATH%

rem debug: --inspector --javascript-debugger?
rem TODO all: --svg?
%PERL% Tools\Scripts\build-webkit --qt --release --no-webkit2 --no-force-sse2 --qmakearg="CONFIG+=production_build" --minimal --netscape-plugin-api
if errorlevel 1 exit /b 1

pushd WebKitBuild\Release
if errorlevel 1 exit /b 1

nmake install
if errorlevel 1 exit /b 1

popd
if errorlevel 1 exit /b 1

popd
if errorlevel 1 exit /b 1

set PATH=%OLDPATH%

%CP% "%QTDIR%"\lib\QtWebKit4.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
