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
%PERL% Tools\Scripts\build-webkit --qt --release --no-webkit2 --no-force-sse2 --minimal --netscape-plugin-api
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
