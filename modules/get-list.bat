@ECHO OFF
SETLOCAL EnableDelayedExpansion 
SETLOCAL ENABLEEXTENSIONS

SET TYPE=%1
SET AVAILABLE_MODULES=

FOR /F "eol=# tokens=2,3 delims=_=" %%i IN (..\.config.win32) DO (
	SET LINE=%%i
	SET SETTING=%%j
	
	FOR %%M IN (%*) DO (
		SET MODULE=%%M
		IF !LINE! == !MODULE! ( 
			IF !TYPE! == !SETTING! (
				CALL SET AVAILABLE_MODULES=%%AVAILABLE_MODULES%%;!MODULE!
			)
		)
	)
)
REM When echo called without parameter (or with empty one) it prints its status
REM on screen so we have to avoid that simply adding ; to the end
CALL SET AVAILABLE_MODULES=%AVAILABLE_MODULES%;

ECHO %AVAILABLE_MODULES%
