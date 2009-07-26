@ECHO OFF
SETLOCAL EnableDelayedExpansion 
SETLOCAL ENABLEEXTENSIONS

echo #include "kadu-config.h" > %2

FOR /F "eol=# tokens=1,2* delims==" %%i IN (%1) DO (
rem	echo "%%i -> %%j"
	if "%%i" == "Description" echo #define DESCRIPTION "%%j" >> %2
	if "%%i" == "Description[pl]" echo #define DESCRIPTION_PL "%%j" >> %2
	if "%%i" == "Author" echo #define AUTHOR "%%j" >> %2
	if "%%i" == "Version" if not "%%j" == "core" echo #define VERSION "%%j" >> %2
		
)

echo #include "module.rc" >> %2
