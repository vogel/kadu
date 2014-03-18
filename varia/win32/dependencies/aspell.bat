@echo off

call "%~dp0\..\build-config.bat"
if errorlevel 1 exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

if exist aspell-install (
	echo aspell-install directory already exists, skipping...
	goto end
)

call strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 goto fail

if exist aspell-%ASPELLVER% %RMDIR% aspell-%ASPELLVER%

if not exist aspell-%ASPELLVER%.tar.gz (
	%WGET% ftp://ftp.gnu.org/gnu/aspell/aspell-%ASPELLVER%.tar.gz
	if errorlevel 1 goto fail
)

if exist aspell-%ASPELLVER%.tar %RM% aspell-%ASPELLVER%.tar
%SEVENZ% x aspell-%ASPELLVER%.tar.gz
if errorlevel 1 goto fail
%SEVENZ% x aspell-%ASPELLVER%.tar
if errorlevel 1 goto fail
%RM% aspell-%ASPELLVER%.tar

pushd aspell-%ASPELLVER%
if errorlevel 1 goto fail

%PATCH% -p1 < "%~dp0"\patches\aspell-0.60.6-20100726.diff
if errorlevel 1 goto fail2

mkdir build
if errorlevel 1 goto fail2
pushd build
if errorlevel 1 goto fail2

%CMAKE% -DCMAKE_INSTALL_PREFIX:PATH="%INSTALLPREFIX%"\aspell-install ..
if errorlevel 1 goto fail3

%CMAKE_MAKE_INSTALL%
if errorlevel 1 goto fail3

popd
popd

if not exist aspell-data-bin.7z (
	%WGET% http://download.kadu.im/win32-devel/aspell-data-bin.7z
	if errorlevel 1 goto fail
)

%CP% "%INSTALLPREFIX%"\aspell-install\bin\aspell.dll "%INSTALLBASE%"
if errorlevel 1 goto fail

if exist "%INSTALLBASE%"\aspell %RMDIR% "%INSTALLBASE%"\aspell
if errorlevel 1 goto fail

%CPDIR% "%INSTALLPREFIX%"\aspell-install\lib\aspell-0.60 "%INSTALLBASE%"\aspell
if errorlevel 1 goto fail

%SEVENZ% x -o"%INSTALLBASE%"\aspell aspell-data-bin.7z
if errorlevel 1 goto fail

echo.
echo apell build: Success
echo.
goto end

:fail3
popd

:fail2
popd

:fail
echo.
echo apell: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
exit /b 1
goto end

:end
popd
call "%~dp0\..\post-build.bat"
