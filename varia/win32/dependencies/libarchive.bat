@echo off

echo.
echo Building libarchive
echo.

set ret=0

call "%~dp0\..\utils.bat" load-config
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" enable-xz
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" load-result archive ARCHIVE
if errorlevel 1 goto fail

if %ARCHIVE_RESULT% EQU 1 goto downloaded
if %ARCHIVE_RESULT% EQU 2 goto unpacked
if %ARCHIVE_RESULT% EQU 3 goto ready

if exist libarchive-%ARCHIVEVER%.tar.gz %RM% libarchive-%ARCHIVEVER%.tar.gz
if errorlevel 1 goto fail

%WGET% http://libarchive.org/downloads/libarchive-%ARCHIVEVER%.tar.gz
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result archive 1
if errorlevel 1 goto fail

:downloaded

if exist libarchive-%ARCHIVEVER% %RMDIR% libarchive-%ARCHIVEVER%
if errorlevel 1 goto fail

if exist libarchive-%ARCHIVEVER%.tar %RM% libarchive-%ARCHIVEVER%.tar
if errorlevel 1 goto fail

%SEVENZ% x libarchive-%ARCHIVEVER%.tar.gz
if errorlevel 1 goto fail

%SEVENZ% x libarchive-%ARCHIVEVER%.tar
if errorlevel 1 goto fail

%RM% libarchive-%ARCHIVEVER%.tar
if errorlevel 1 goto fail

mkdir libarchive-%ARCHIVEVER%\build-cmake
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result archive 2
if errorlevel 1 goto fail

:unpacked

pushd libarchive-%ARCHIVEVER%\build-cmake
if errorlevel 1 goto fail2

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\libarchive-install -DCMAKE_PREFIX_PATH:PATH="%INSTALLPREFIX%"\xz -DENABLE_CPIO:BOOL=OFF -DENABLE_TAR:BOOL=OFF -DENABLE_TEST:BOOL=OFF ..
if errorlevel 1 goto fail2

%CMAKE_MAKE%
if errorlevel 1 goto fail2

rem The following tests FAILED:
rem          92 - libarchive_test_compat_zip (SEGFAULT)
rem         106 - libarchive_test_fuzz_zip (SEGFAULT)
rem         116 - libarchive_test_read_disk_directory_traversals (Failed)
rem         190 - libarchive_test_read_format_rar_binary (SEGFAULT)
rem         211 - libarchive_test_read_format_zip (SEGFAULT)
rem         213 - libarchive_test_read_format_zip_filename (SEGFAULT)
rem         214 - libarchive_test_read_format_zip_mac_metadata (Failed)
rem         221 - libarchive_test_read_append_filter (Failed)
rem         305 - libarchive_test_write_format_zip (SEGFAULT)
rem %CTEST%
rem if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail

%CP% "%INSTALLPREFIX%"\libarchive-install\bin\archive.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

call "%~dp0\..\utils.bat" store-result archive 3
if errorlevel 1 goto fail

:ready

echo.
echo libarchive build: Success
echo.
goto end

:fail2
popd

:fail
echo.
echo libarchive: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
pause
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
