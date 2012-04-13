@ECHO OFF
SETLOCAL EnableDelayedExpansion 
SETLOCAL ENABLEEXTENSIONS

echo #include "kadu-config.h" > %2

FOR /F "eol=# tokens=1,2* delims==" %%i IN (%1) DO (
	if "%%i" == "Description" (
		set str=%%j
		set str=!str:"=""!
		echo #define DESCRIPTION "!str!" >> %2
	)
	if "%%i" == "Author" (
		set str=%%j
		set str=!str:"=""!
		echo #define AUTHOR "!str!" >> %2
	)
	if "%%i" == "Version" (
		if not "%%j" == "core" (
			echo #define PLUGIN_VERSION "%%j" >> %2
		FOR /F "tokens=1,2,3 delims=.-" %%l in ("%%j") DO (
			:: cmd sucks..
			set l=0
			set m=0
			set n=0
			if not "%%l" == "" set l=%%l
			if not "%%m" == "" set m=%%m
			if not "%%n" == "" set n=%%n
			echo #define PLUGIN_NUMERIC_VERSION !l!, !m!, !n! >> %2
			)
		) else (
			echo #define PLUGIN_VERSION KADU_VERSION >> %2
			echo #define PLUGIN_NUMERIC_VERSION KADU_NUMERIC_VERSION >> %2
		)
	)		
)

echo #include "pluginbase.rc" >> %2
