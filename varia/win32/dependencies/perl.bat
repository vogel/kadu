echo on

call "%~dp0\..\build-config.bat"
if errorlevel 1 goto exit /b 1

call "%~dp0\..\pre-build.bat"
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"

if exist strawberry-perl-%PERLVER%-32bit-portable\perl\bin\perl.exe (
	echo perl already exists, skipping...
	popd
	exit /b
)

if not exist strawberry-perl-%PERLVER%-32bit-portable.zip (
	%WGET% http://strawberryperl.com/download/%PERLVER%/strawberry-perl-%PERLVER%-32bit-portable.zip
	if errorlevel 1 goto fail
)

if exist strawberry-perl-%PERLVER%-32bit-portable %RMDIR% strawberry-perl-%PERLVER%-32bit-portable 
mkdir strawberry-perl-%PERLVER%-32bit-portable

%SEVENZ% x -ostrawberry-perl-%PERLVER%-32bit-portable strawberry-perl-%PERLVER%-32bit-portable.zip
if errorlevel 1 goto fail

%SED% -i -e "/cmd \/K/d" -e "/@echo off/d" strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 goto fail

echo.
echo perl download: Success
echo.
goto end

:fail
echo.
echo perl: Error encountered
echo.
popd
call "%~dp0\..\post-build.bat"
exit /b 1

:end
popd
call "%~dp0\..\post-build.bat"
