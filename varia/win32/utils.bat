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
