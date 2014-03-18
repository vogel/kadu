@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist qtwebkit-23 (
	echo qtwebkit-23 directory already exists, skipping...
	goto end
)

set OLDPATH=%PATH%

call strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 goto fail

rem set PROGRAM_FILES_X86=%ProgramFiles%
rem if defined ProgramFiles(x86) set PROGRAM_FILES_X86=%ProgramFiles(x86)%
rem set RUBYVER=2.0.0-p0
rem set WGET="%PROGRAM_FILES_X86%"\GnuWin32\bin\wget.exe
rem set PYTHONCOMMONPORTABLEVER=2.7
rem set QTWEBKIT23VER=2.3.1b
rem set SEVENZ="%ProgramFiles%"\7-Zip\7z.exe
rem set GNUWIN32GPERFVER=3.0.1
rem set GNUWIN32GREPVER=2.5.4
rem set GIT=git

rem Download dependencies.

if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32\python.exe goto pastpython
	if not exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z (
		%WGET% http://python-common-portable.googlecode.com/files/python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
		if errorlevel 1 goto fail
	)

	if exist python-%PYTHONCOMMONPORTABLEVER%-bin-win32 %RMDIR% python-%PYTHONCOMMONPORTABLEVER%-bin-win32
	mkdir python-%PYTHONCOMMONPORTABLEVER%-bin-win32

	%SEVENZ% x -opython-%PYTHONCOMMONPORTABLEVER%-bin-win32 python-%PYTHONCOMMONPORTABLEVER%-bin-win32.7z
	if errorlevel 1 goto fail
:pastpython

set PATH=%CD%\python-%PYTHONCOMMONPORTABLEVER%-bin-win32;%PATH%

if exist ruby-%RUBYVER%-i386-mingw32\bin\ruby.exe goto pastruby
	if not exist ruby-%RUBYVER%-i386-mingw32.7z (
		%WGET% http://rubyforge.org/frs/download.php/76807/ruby-%RUBYVER%-i386-mingw32.7z
		if errorlevel 1 goto fail
	)

	if exist ruby-%RUBYVER%-i386-mingw32 %RMDIR% ruby-%RUBYVER%-i386-mingw32

	%SEVENZ% x ruby-%RUBYVER%-i386-mingw32.7z
	if errorlevel 1 goto fail
:pastruby

set PATH=%CD%\ruby-%RUBYVER%-i386-mingw32\bin;%PATH%

if exist winflexbison\bison.exe goto pastwinflexbison
	if not exist win_flex_bison-latest.zip (
		%WGET% http://downloads.sourceforge.net/project/winflexbison/win_flex_bison-latest.zip
		if errorlevel 1 goto fail
	)

	if exist winflexbison %RMDIR% winflexbison
	mkdir winflexbison

	%SEVENZ% x -owinflexbison win_flex_bison-latest.zip
	if errorlevel 1 goto fail

	rename winflexbison\win_bison.exe bison.exe
	if errorlevel 1 goto fail
:pastwinflexbison

set PATH=%CD%\winflexbison;%PATH%

if not exist gnuwin32 mkdir gnuwin32
if errorlevel 1 goto fail

set PATH=%CD%\gnuwin32\bin;%PATH%

if exist gnuwin32\bin\gperf.exe goto pastgperf
	if not exist gperf-%GNUWIN32GPERFVER%-bin.zip (
		%WGET% http://sourceforge.net/projects/gnuwin32/files/gperf/%GNUWIN32GPERFVER%/gperf-%GNUWIN32GPERFVER%-bin.zip/download
		if errorlevel 1 goto fail
	)

	%SEVENZ% x -ognuwin32 gperf-%GNUWIN32GPERFVER%-bin.zip
	if errorlevel 1 goto fail
:pastgperf

if exist gnuwin32\bin\grep.exe goto pastgrep
	if not exist grep-%GNUWIN32GREPVER%-dep.zip (
		%WGET% http://sourceforge.net/projects/gnuwin32/files/grep/%GNUWIN32GREPVER%/grep-%GNUWIN32GREPVER%-dep.zip/download
		if errorlevel 1 goto fail
	)

	if not exist grep-%GNUWIN32GREPVER%-bin.zip (
		%WGET% http://sourceforge.net/projects/gnuwin32/files/grep/%GNUWIN32GREPVER%/grep-%GNUWIN32GREPVER%-bin.zip/download
		if errorlevel 1 goto fail
	)

	%SEVENZ% x -ognuwin32 grep-%GNUWIN32GREPVER%-dep.zip
	if errorlevel 1 goto fail

	%SEVENZ% x -ognuwin32 grep-%GNUWIN32GREPVER%-bin.zip
	if errorlevel 1 goto fail
:pastgrep

rem Now QtWebKit itself.

rem set TARBALL=qtwebkit-%QTWEBKIT23VER%.tar.gz
rem FOR does not seem to work inside IF, so I use GOTO.
rem if exist %TARBALL% goto pastdownload
	rem Beware of HTTP 202 Accept: Gitorious will be preparing the tarball and
	rem send small text instead of it meanwhile.
	
rem :download
rem 	%WGET% --no-check-certificate -O %TARBALL% http://gitorious.org/webkit/qtwebkit-23/archive-tarball/qtwebkit-%QTWEBKIT23VER%
rem 	if errorlevel 1 exit /b 1
	
rem 	for /F "usebackq" %%A in ('%TARBALL%') do set SIZE=%%~zA
rem 	if %SIZE% LSS 1000000 (
rem 		%RM% %TARBALL%
rem 		if errorlevel 1 exit /b 1
rem 		goto download
rem 	)
rem :pastdownload

rem if exist qtwebkit-23 (
rem 	echo qtwebkit-23 directory already exists, skipping...
rem 	exit /b
rem )

if not exist qtwebkit-23.git-bare (
	%GIT% clone --bare git://gitorious.org/webkit/qtwebkit-23.git qtwebkit-23.git-bare
	if errorlevel 1 goto fail
) else (
	pushd qtwebkit-23.git-bare
	%GIT% fetch --all
	if errorlevel 1 goto fail
	popd
)

%GIT% clone .\qtwebkit-23.git-bare qtwebkit-23
if errorlevel 1 goto fail

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

:ready

pushd qtwebkit-23
if errorlevel 1 goto fail

%GIT% checkout qtwebkit-%QTWEBKIT23VER%
if errorlevel 1 goto fail2

%PATCH% -p1 < "%~dp0"\patches\qtwebkit-msvc2013.patch
if errorlevel 1 goto fail2

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
%PERL% Tools\Scripts\build-webkit --qt --release --no-webkit2 --no-force-sse2 --minimal --netscape-plugin-api
if errorlevel 1 goto fail2

pushd WebKitBuild\Release
if errorlevel 1 goto fail2

nmake install
if errorlevel 1 goto fail3

popd
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail

set PATH=%OLDPATH%

%CP% "%QTDIR%"\lib\QtWebKit4.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

echo.
echo qtwebkit build: Success
echo.
goto end

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
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
