if exist aspell-install (
	echo aspell-install directory already exists, skipping...
	exit /b
)

if exist aspell-%ASPELLVER% %RMDIR% aspell-%ASPELLVER%

if not exist aspell-%ASPELLVER%.tar.gz (
	%WGET% ftp://ftp.gnu.org/gnu/aspell/aspell-%ASPELLVER%.tar.gz
	if errorlevel 1 exit /b 1
)

if exist aspell-%ASPELLVER%.tar %RM% aspell-%ASPELLVER%.tar
%SEVENZ% x aspell-%ASPELLVER%.tar.gz
if errorlevel 1 exit /b 1
%SEVENZ% x aspell-%ASPELLVER%.tar
if errorlevel 1 exit /b 1
%RM% aspell-%ASPELLVER%.tar

pushd aspell-%ASPELLVER%
if errorlevel 1 exit /b 1

%PATCH% -p1 < "%~dp0"\patches\aspell-0.60.6-20100726.diff
if errorlevel 1 exit /b 1

mkdir build
if errorlevel 1 exit /b 1
pushd build
if errorlevel 1 exit /b 1

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\aspell-install ..
if errorlevel 1 exit /b 1

%CMAKE_MAKE_INSTALL%
if errorlevel 1 exit /b 1

popd
popd

if not exist aspell-data-bin.7z (
	%WGET% http://download.kadu.im/win32-devel/aspell-data-bin.7z
	if errorlevel 1 exit /b 1
)

%CP% "%INSTALLPREFIX%"\aspell-install\bin\aspell.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1

if exist "%INSTALLBASE%"\aspell %RMDIR% "%INSTALLBASE%"\aspell
if errorlevel 1 exit /b 1

%CPDIR% "%INSTALLPREFIX%"\aspell-install\lib\aspell-0.60 "%INSTALLBASE%"\aspell
if errorlevel 1 exit /b 1

%SEVENZ% x -o"%INSTALLBASE%"\aspell aspell-data-bin.7z
if errorlevel 1 exit /b 1
