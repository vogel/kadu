@echo off

set PROGRAM_FILES_X86=%ProgramFiles%
if defined ProgramFiles(x86) set PROGRAM_FILES_X86=%ProgramFiles(x86)%

rem This is needed for nmake-based projects like Qt or OpenSSL.
rem MSBuild (and thus CMake) can simply use v120_xp Platform Toolset.
set INCLUDE=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Include;%INCLUDE%
set PATH=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Bin;%PATH%
set LIB=%PROGRAM_FILES_X86%\Microsoft SDKs\Windows\v7.1A\Lib;%LIB%
set CL=/MP /D_USING_V120_SDK71_

set ARCHIVEVER=3.1.2
set ASPELLVER=0.60.6.1
set GADUVER=1.12.0rc2
set GADUPACKAGEVER=1.1
set GNUWIN32GPERFVER=3.0.1
set GNUWIN32GREPVER=2.5.4
set IDNVER=1.26
set NASMVER=2.10.07
set OSSLVER=1.0.1f
set PERLVER=5.16.3.1
set PYTHONCOMMONPORTABLEVER=2.7
set QCAVER=v2.0.3
set QTVER=v4.8.5
set QTWEBKIT23VER=2.3.3
set RUBYVER=2.0.0-p0
set SQLITEYEAR=2013
set SQLITEVER=3071602
set XZVER=5.0.4
set ZLIBVER=1.2.8

set Configuration=Release
set PlatformToolset=v120_xp

set KADUGIT=%~dp0\..\..
if not defined KADUROOT set KADUROOT=%KADUGIT%\..
set INSTALLPREFIX=%KADUROOT%\dependencies
set INSTALLBASE=%KADUROOT%\install-base
set KADU_PLUGINS=antistring;auto_hide;autoaway;autoresponder;autostatus;cenzor;chat_notify;config_wizard;desktop_docking;docking;emoticons;encryption_ng;encryption_ng_simlite;exec_notify;ext_sound;filedesc;firewall;gadu_protocol;hints;history;history_migration;idle;imagelink;jabber_protocol;last_seen;mediaplayer;pcspeaker;phonon_sound;profiles_import;qt4_docking;qt4_docking_notify;qt4_sound;screenshot;simpleview;single_window;sms;sound;speech;spellchecker;sql_history;tabs;winamp_mediaplayer;word_fix

set CP=copy /y
set CPDIR=xcopy /s /q /y /i
set RM=del /f /q
set RMDIR=rmdir /s /q

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
set SVN=svn
set WGET="%PROGRAM_FILES_X86%"\GnuWin32\bin\wget.exe

if not exist "%INSTALLPREFIX%" mkdir "%INSTALLPREFIX%"
if not exist "%INSTALLBASE%" mkdir "%INSTALLBASE%"
