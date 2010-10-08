@ECHO OFF
SETLOCAL EnableDelayedExpansion 
SETLOCAL ENABLEEXTENSIONS

echo #include "kadu-config.h" > %2

FOR /F "eol=# tokens=1,2* delims==" %%i IN (%1) DO (
	if "%%i" == "Description" echo #define DESCRIPTION "%%j" >> %2
	if "%%i" == "Description[pl]" echo #define DESCRIPTION_PL "%%j" >> %2
	if "%%i" == "Author" echo #define AUTHOR "%%j" >> %2
	if "%%i" == "Version" (
		if not "%%j" == "core" (
			echo #define VERSION "%%j" >> %2
		FOR /F "tokens=1,2,3 delims=.-" %%l in ("%%j") DO (
			:: cmd sucks..
			set l=0
			set m=0
			set n=0
			if not "%%l" == "" set l=%%l
			if not "%%m" == "" set m=%%m
			if not "%%n" == "" set n=%%n
			echo #define NUMERIC_VERSION !l!, !m!, !n!
			)
		)
	)		
)

echo #include "module.rc" >> %2
