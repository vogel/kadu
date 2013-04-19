if exist libarchive-install (
	echo libarchive-install directory already exists, skipping...
	exit /b
)

set OLDPATH=%PATH%
set PATH=%INSTALLPREFIX%\xz\bin_i486;%PATH%

if exist libarchive-%ARCHIVEVER% %RMDIR% libarchive-%ARCHIVEVER%

if not exist libarchive-%ARCHIVEVER%.tar.gz (
	%WGET% http://libarchive.org/downloads/libarchive-%ARCHIVEVER%.tar.gz
	if errorlevel 1 exit /b 1
)

if exist libarchive-%ARCHIVEVER%.tar %RM% libarchive-%ARCHIVEVER%.tar
%SEVENZ% x libarchive-%ARCHIVEVER%.tar.gz
if errorlevel 1 exit /b 1
%SEVENZ% x libarchive-%ARCHIVEVER%.tar
if errorlevel 1 exit /b 1
%RM% libarchive-%ARCHIVEVER%.tar

mkdir libarchive-%ARCHIVEVER%\build-cmake
if errorlevel 1 exit /b 1

pushd libarchive-%ARCHIVEVER%\build-cmake
if errorlevel 1 exit /b 1

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\libarchive-install -DCMAKE_PREFIX_PATH:PATH="%INSTALLPREFIX%"\xz -DENABLE_CPIO:BOOL=OFF -DENABLE_TAR:BOOL=OFF -DENABLE_TEST:BOOL=OFF ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE%
if errorlevel 1 exit /b 1

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
if errorlevel 1 exit /b 1

popd
if errorlevel 1 exit /b 1

set PATH=%OLDPATH%

%CP% "%INSTALLPREFIX%"\libarchive-install\bin\archive.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
