if exist icu-install (
	echo icu-install directory already exists, skipping...
	exit /b
)

set OLDPATH=%PATH%	

if exist icu %MY_RMDIR% icu
if errorlevel 1 exit /b 1

if exist C:\icu-install %MY_RMDIR% C:\icu-install
if errorlevel 1 exit /b 1

if not exist icu4c-%ICUVER_%-src.zip (
	%WGET% http://download.icu-project.org/files/icu4c/%ICUVER%/icu4c-%ICUVER_%-src.zip
	if errorlevel 1 exit /b 1
)

%SEVENZ% x icu4c-%ICUVER_%-src.zip
if errorlevel 1 exit /b 1

pushd icu\source
if errorlevel 1 exit /b 1

rem This requires Cygwin with dos2unix and make installed
set PATH=C:\cygwin64\bin;%PATH%

rem Rerun to gain VC's link back
call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
if errorlevel 1 exit /b 1

dos2unix *
if errorlevel 1 exit /b 1

dos2unix -f configure
if errorlevel 1 exit /b 1

bash runConfigureICU Cygwin/MSVC --prefix=/cygdrive/c/icu-install
if errorlevel 1 exit /b 1

rem Multi-process build fails, so no -j
make install
if errorlevel 1 exit /b 1

%MY_MV% C:\icu-install "%INSTALLPREFIX%"\icu-install
if errorlevel 1 exit /b 1

popd

set PATH=%OLDPATH%

%MY_CP% "%INSTALLPREFIX%"\icu-install\lib\icudt*.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%INSTALLPREFIX%"\icu-install\lib\icuin*.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

%MY_CP% "%INSTALLPREFIX%"\icu-install\lib\icuuc*.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
