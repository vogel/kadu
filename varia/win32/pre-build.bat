@echo off

set PRE_BUILD_PATH=%PATH%

pushd "%VS120COMNTOOLS%..\..\VC\"
call ".\vcvarsall.bat" x86
if errorlevel 1 goto fail
popd

goto end

:fail1
popd
exit /b 1

:end
