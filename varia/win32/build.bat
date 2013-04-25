@echo off

set PROGRAM_FILES_X86=%ProgramFiles%
if defined ProgramFiles(x86) set PROGRAM_FILES_X86=%ProgramFiles(x86)%

call "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
if errorlevel 1 goto fail

if defined CL (set CL=/MP %CL%) else (set CL=/MP)

rem This is needed for nmake-based projects like Qt or OpenSSL.
rem MSBuild (and thus CMake) can simply use v110_xp Platform Toolset.
set INCLUDE=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Include;%INCLUDE%
set PATH=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Bin;%PATH%
set LIB=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Lib;%LIB%
set CL=/D_USING_V110_SDK71_ %CL%

set ARCHIVEVER=3.1.2
set ASPELLVER=0.60.6.1
set GADUVER=1324
set GNUWIN32GPERFVER=3.0.1
set GNUWIN32GREPVER=2.5.4
set IDNVER=1.26
set NASMVER=2.10.07
set OSSLVER=1.0.1e
set PERLVER=5.16.3.1
set PYTHONCOMMONPORTABLEVER=2.7
set QCAVER=1311233
rem v4.8.4 tag does not compile on VS2012, and this version works just fine.
rem TODO: Please switch to official v4.8.5 when it gets released.
rem set QTVER=7f1197a689deae502129148be94b63c246962b8f
set QTVER=6c7b0b19da9bebbccf846f694eb6bf7891e5e883
set QTWEBKIT23VER=2.3.1b
set RUBYVER=2.0.0-p0
set SQLITEYEAR=2013
set SQLITEVER=3071602
set XZVER=5.0.4
set ZLIBVER=1.2.7

set Configuration=Release
set PlatformToolset=v110_xp

set KADUGIT=%~dp0\..\..
if not defined KADUROOT set KADUROOT=%KADUGIT%\..
set INSTALLPREFIX=%KADUROOT%\dependencies
set INSTALLBASE=%KADUROOT%\install-base

set CP=copy /y
set CPDIR=xcopy /s /q /y /i
set RM=del /f /q
set RMDIR=rmdir /s /q

set CMAKE=cmake -G "Visual Studio 11" -T %PlatformToolset%
set CMAKE_MAKE=cmake --build . --config %Configuration%
set CMAKE_MAKE_INSTALL=cmake --build . --config %Configuration% --target install
set CTEST=ctest -C %Configuration%
set GIT=git
set MSBUILD=MSBuild /p:Configuration=%Configuration%;PlatformToolset=%PlatformToolset%
set PATCH="%PROGRAM_FILES_X86%"\Git\bin\patch.exe
set PERL=perl
set SED="%PROGRAM_FILES_X86%"\Git\bin\sed.exe
set SEVENZ="%ProgramFiles%"\7-Zip\7z.exe
set SVN=svn
set WGET="%PROGRAM_FILES_X86%"\GnuWin32\bin\wget.exe

if not exist "%INSTALLPREFIX%" mkdir "%INSTALLPREFIX%"
if not exist "%INSTALLBASE%" mkdir "%INSTALLBASE%"

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

rem Perl is needed by OpenSSL, Qt, and QtWebKit.

if exist strawberry-perl-%PERLVER%-32bit-portable\perl\bin\perl.exe goto pastperl
	if not exist strawberry-perl-%PERLVER%-32bit-portable.zip (
		%WGET% http://strawberryperl.com/download/%PERLVER%/strawberry-perl-%PERLVER%-32bit-portable.zip
		if errorlevel 1 exit /b 1
	)

	if exist strawberry-perl-%PERLVER%-32bit-portable %RMDIR% strawberry-perl-%PERLVER%-32bit-portable 
	mkdir strawberry-perl-%PERLVER%-32bit-portable

	%SEVENZ% x -ostrawberry-perl-%PERLVER%-32bit-portable strawberry-perl-%PERLVER%-32bit-portable.zip
	if errorlevel 1 exit /b 1

	%SED% -i -e "/cmd \/K/d" -e "/@echo off/d" strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
	if errorlevel 1 exit /b 1
:pastperl

call strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 exit /b 1

rem Now the real Kadu dependencies.

call "%~dp0"\dependencies\zlib.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\libidn.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\openssl.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\libgadu.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\aspell.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\qt.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\qtwebkit-23.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\qca.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\xz.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\libarchive.bat
if errorlevel 1 goto fail

popd
if errorlevel 1 goto fail

rem Prepare Kadu installation directory.

%CP% "%VS110COMNTOOLS%"\..\..\VC\redist\x86\Microsoft.VC110.CRT\msvcp110.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%CP% "%VS110COMNTOOLS%"\..\..\VC\redist\x86\Microsoft.VC110.CRT\msvcr110.dll "%INSTALLBASE%"
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
		if errorlevel 1 goto fail
	)

	pushd build
	if errorlevel 1 goto fail

	%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%KADUROOT%"\install -DKADU_SDK_DIR:PATH="%KADUROOT%"\sdk -DCMAKE_PREFIX_PATH="%INSTALLPREFIX%"\qca;"%INSTALLPREFIX%"\libarchive-install -DZLIB_ROOT:PATH="%INSTALLPREFIX%"\zlib-install -DWIN_LIBGADU_DIR:PATH="%INSTALLPREFIX%"\libgadu-install -DWIN_IDN_DIR:PATH="%INSTALLPREFIX%"\libidn -DWIN_ASPELL_DIR:PATH="%INSTALLPREFIX%"\aspell-install "%KADUGIT%"
	if errorlevel 1 goto fail
	%CMAKE_MAKE%
	if errorlevel 1 goto fail
	%CMAKE_MAKE_INSTALL%
	if errorlevel 1 goto fail

	popd
	if errorlevel 1 goto fail
:pastkadu

popd
if errorlevel 1 goto fail

echo.
echo Kadu build: Success
echo.
goto end

:fail
echo.
echo Kadu build: Error encountered
echo.

:end
%COMSPEC% /k
exit
