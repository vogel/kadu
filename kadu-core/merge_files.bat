@ECHO OFF

REM ECHO "type %1 >> %2"

set A=%1
set A=%A:/=\%

set B=%2
set B=%B:/=\%

type %A% >> %B%

REM ECHO %0 %A% %B% >> regerate_final.bat
