@echo off

set PRE_BUILD_PATH=%PATH%

rem pushd "%VS120COMNTOOLS%..\..\VC\"
rem call ".\vcvarsall.bat" x86
rem if errorlevel 1 goto fail
rem popd

goto end

:fail
popd
exit /b 1

:end
