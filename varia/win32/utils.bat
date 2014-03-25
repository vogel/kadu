@echo off

call :%1 ret %2 %3 %4 %5 %6 %7 %8 %9
exit /b %ret%

:load-global-config
echo Loading global configuration...
call "%~dp0\global-config.bat"
set "%~1=%errorlevel%"
goto :eof

:load-user-config
if exist "%~dp0\user-config.bat" (
	echo Loading user configuration...
	call "%~dp0\user-config.bat"
	set %~1=%ret%
) else (
	echo User configuration not found...
)
goto :eof

:load-config
call :load-global-config ret
set %~1=%ret%
if %ret% GTR 1 goto :eof

call :load-user-config ret
set %~1=%ret%
if %ret% GTR 1 goto :eof

goto :eof

:load-result
if not exist %~2.status (
	set %~3_RESULT=nothing
	goto :eof
)

set /P %~3_RESULT=<%INSTALLPREFIX%\%~2.status
goto :eof

:store-result
echo %~3 > %INSTALLPREFIX%\%~2.status
goto :eof

:build-dependency
%COMSPEC% /c "%~dp0\dependencies\%~2.bat"
set "%~1=%errorlevel%"
goto :eof

:enable-msvc
pushd "%VS120COMNTOOLS%..\..\VC\"
call ".\vcvarsall.bat" x86
set "%~1=%errorlevel%"
echo.
echo MSVC enabled
echo.
popd
goto :eof

:enable-perl
call :build-dependency ret perl
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

call strawberry-perl-%PERLVER%-32bit-portable\portableshell.bat
if errorlevel 1 goto fail2

popd
if errorlevel 1 goto fail2

goto :eof

:enable-python
call :build-dependency ret python
if errorlevel 1 go

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set PATH=%CD%\python-%PYTHONCOMMONPORTABLEVER%-bin-win32;%PATH%

popd
if errorlevel 1 goto fail2

goto :eof

:enable-ruby
call :build-dependency ret ruby
if errorlevel 1 go

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set PATH=%CD%\ruby-%RUBYVER%-i386-mingw32\bin;%PATH%

popd
if errorlevel 1 goto fail2

goto :eof

:enable-nasm
call :build-dependency ret nasm
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set PATH=%CD%\nasm-%NASMVER%;%PATH%
goto :eof

:enable-winflexbison
call :build-dependency ret winflexbison
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set PATH=%CD%\winflexbison;%PATH%
goto :eof

:enable-gperf
call :build-dependency ret gperf
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set PATH=%CD%\gnuwin32\bin;%PATH%
goto :eof

:enable-grep
call :build-dependency ret grep
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set PATH=%CD%\gnuwin32\bin;%PATH%
goto :eof


:enable-xz
call :build-dependency ret xz
if errorlevel 1 goto fail

pushd "%INSTALLPREFIX%"
if errorlevel 1 goto fail

set PATH=%INSTALLPREFIX%\xz\bin_i486;%PATH%
goto :eof

:fail2
popd

:fail
exit /b %ret%
