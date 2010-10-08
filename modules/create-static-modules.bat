@ECHO OFF

SETLOCAL ENABLEEXTENSIONS

ECHO //This file is automatically created by create-static-modules.bat > static_modules.cpp

FOR %%I IN (%*) DO (
	ECHO extern "C" int %%I_init(bool^); >> static_modules.cpp
	ECHO extern "C" void %%I_close(^); >> static_modules.cpp
)

ECHO void ModulesManager::registerStaticModules() >> static_modules.cpp
ECHO { >> static_modules.cpp

FOR %%I IN (%*) DO (
	ECHO 	registerStaticModule("%%I", %%I_init, %%I_close^); >> static_modules.cpp
)

ECHO } >> static_modules.cpp
