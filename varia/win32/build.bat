@echo off

set PROGRAM_FILES_X86=%ProgramFiles%
if defined ProgramFiles(x86) set PROGRAM_FILES_X86=%ProgramFiles(x86)%

call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
if errorlevel 1 goto fail

if defined CL (set CL=/MP %CL%) else (set CL=/MP)

rem This is needed for nmake-based projects like Qt or OpenSSL.
rem MSBuild (and thus CMake) can simply use v120_xp Platform Toolset.
set INCLUDE=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Include;%INCLUDE%
set PATH=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Bin;%PATH%
set LIB=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Lib;%LIB%
set CL=/D_USING_V110_SDK71_ %CL%

set ARCHIVEVER=3.1.2
set ASPELLVER=0.60.6.1
set GADUVER=4d642839e0ed54de9f8fab6a31e66094e8722f8a
set GCRYPTVER=1.5.3
set GNUWIN32GAWKVER=3.1.6-1
set GPGERRORVER=1.12
set ICUVER=52.1
set ICUVER_=52_1
set IDNVER=1.28
set NASMVER=2.11
set OSSLVER=1.0.1e
set OTRVER=4.0.0
set PERLVER=5.18.1.1
set PYTHONCOMMONPORTABLEVER=2.7
rem set QCAVER=d349189b84e53ee5f15b52e22281fe4582e40842 --pre-cmake
set QCAVER=2817b31e06b8033deea07d9f95b52a4a6f9d092e
rem set QTVER=ece0bd590a06d8d8f4f922395a8e68f7b0e678e6 --Qt4
set QTVER=06d0fa240f6a0f99770e51969543608dbaf5f7e0
set QTWEBKITVER=9215ead2e2237a735693eb60725001edf58ae93d
rem set QTWEBKIT23VER=2.3.3
set RUBYVER=2.0.0-p353
rem set SQLITEYEAR=2013
rem set SQLITEVER=3080200
set XZVER=5.0.5
set ZLIBVER=1.2.8

set Configuration=Release
set PlatformToolset=v120_xp

set KADUGIT=%~dp0\..\..
if not defined KADUROOT set KADUROOT=%KADUGIT%\..
set INSTALLPREFIX=%KADUROOT%\dependencies
set INSTALLBASE=%KADUROOT%\install-base

set MY_CP=copy /y
set MY_CPDIR=xcopy /s /q /y /i
set MY_MKDIR_P=md
set MY_MV=move
set MY_RM=del /f /q
set MY_RMDIR=rmdir /s /q

set CMAKE=cmake -G "Visual Studio 12" -T %PlatformToolset%
set CMAKE_MAKE=cmake --build . --config %Configuration%
set CMAKE_MAKE_INSTALL=cmake --build . --config %Configuration% --target install
set CTEST=ctest -C %Configuration%
set GIT=git
set MSBUILD=MSBuild /p:Configuration=%Configuration%;PlatformToolset=%PlatformToolset%
set PATCH="%PROGRAM_FILES_X86%"\Git\bin\patch.exe
set PERL=perl
set SED="%PROGRAM_FILES_X86%"\Git\bin\sed.exe
set SEVENZ="%ProgramFiles%"\7-Zip\7z.exe
set WGET="%PROGRAM_FILES_X86%"\GnuWin32\bin\wget.exe --no-check-certificate

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

	if exist strawberry-perl-%PERLVER%-32bit-portable %MY_RMDIR% strawberry-perl-%PERLVER%-32bit-portable 
	mkdir strawberry-perl-%PERLVER%-32bit-portable

	%SEVENZ% x -ostrawberry-perl-%PERLVER%-32bit-portable strawberry-perl-%PERLVER%-32bit-portable.zip
	if errorlevel 1 exit /b 1

	%SED% -i -e "/cmd \/K/d" -e "/@echo off/d" strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
	if errorlevel 1 exit /b 1
:pastperl

set PATH=%CD%\strawberry-perl-%PERLVER%-32bit-portable\perl\bin;%PATH%

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

call "%~dp0"\dependencies\libgpg-error.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\libgcrypt.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\libotr.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\icu.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\qt5.bat
if errorlevel 1 goto fail

set QTDIR=%INSTALLPREFIX%\qt5\qtbase

call "%~dp0"\dependencies\qca-qt5.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\xz.bat
if errorlevel 1 goto fail

call "%~dp0"\dependencies\libarchive.bat
if errorlevel 1 goto fail

popd
if errorlevel 1 goto fail

rem Prepare Kadu installation directory.

%MY_CP% "%VS120COMNTOOLS%"\..\..\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%MY_CP% "%VS120COMNTOOLS%"\..\..\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

%MY_CP% "%INSTALLPREFIX%"\libidn\COPYINGv3 "%INSTALLBASE%"\COPYING
if errorlevel 1 goto fail

rem Build and install Kadu.

pushd "%KADUROOT%"
if errorlevel 1 goto fail

if exist install goto pastkadu
	if not exist build (
		mkdir build
		if errorlevel 1 goto fail
	)

	pushd build
	if errorlevel 1 goto fail

	%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%KADUROOT%"\install -DCMAKE_PREFIX_PATH="%INSTALLPREFIX%"\qca-install;"%INSTALLPREFIX%"\libarchive-install -DZLIB_ROOT:PATH="%INSTALLPREFIX%"\zlib-install -DWIN_LIBOTR_DIR:PATH="%INSTALLPREFIX%"\libotr-install -DWIN_LIBGADU_DIR:PATH="%INSTALLPREFIX%"\libgadu-install -DWIN_IDN_DIR:PATH="%INSTALLPREFIX%"\libidn -DWIN_ASPELL_DIR:PATH="%INSTALLPREFIX%"\aspell-install "%KADUGIT%"
	if errorlevel 1 goto fail
	cmake --build . --config RelWithDebInfo
	if errorlevel 1 goto fail
	cmake --build . --config RelWithDebInfo --target install
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
