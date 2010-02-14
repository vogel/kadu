@ECHO OFF
SETLOCAL EnableDelayedExpansion 

SET AVAILABLE_MODULES=

FOR /D %%I IN (*) DO (
	IF EXIST %%I/emots.txt (
		CALL SET AVAILABLE_MODULES=%%AVAILABLE_MODULES%%;%%I
	)
)

REM When echo called without parameter (or with empty one) it prints its status
REM on screen so we have to avoid that simply adding ; to the end
CALL SET AVAILABLE_MODULES=%%AVAILABLE_MODULES%%;

ECHO %AVAILABLE_MODULES%
