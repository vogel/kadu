@ECHO OFF
SETLOCAL EnableDelayedExpansion 
SETLOCAL ENABLEEXTENSIONS

SET TYPE=%1
SET AVAILABLE_MODULES=

FOR /F "eol=# tokens=2,3,4,* delims=_=" %%i IN (..\.config) DO (
	SET LINE=%%i
	SET SETTING=%%j

	IF NOT "%%k" EQU "" (
		SET LINE=%%i_%%j
		SET SETTING=%%k
	)

	IF NOT "%%l" EQU "" (
		SET LINE=%%i_%%j_%%k
		SET SETTING=%%l
	)
	
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
