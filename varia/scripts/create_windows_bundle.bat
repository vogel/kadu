@ECHO OFF

set DESTINATION="C:\Program Files\Kadu-0.6.5.2"
set QT_DIR="C:\Qt\4.5.1\bin\"
set LIBGADU_DIR="C:\Qt\libgadu-win32\"
set LIBSNDFILE_DIR="C:\Qt\libsndfile\"
set OPENSSL_DIR="C:\Qt\mingw32-openssl-0.9.8j\"
set ZLIB_DIR="C:\Qt\zlib\"

ECHO Removing existing installation
rd %DESTINATION% /S /Q

ECHO Copying kadu files
xcopy kadu-core\kadu*.exe %DESTINATION%\ /R /Y
xcopy kadu-core\kadu*.dll %DESTINATION%\ /R /Y
xcopy kadu-core\kadu.ico  %DESTINATION%\ /R /Y
xcopy AUTHORS   %DESTINATION%\ /R /Y
xcopy ChangeLog %DESTINATION%\ /R /Y
xcopy COPYING   %DESTINATION%\ /R /Y
xcopy HISTORY   %DESTINATION%\ /R /Y
xcopy README    %DESTINATION%\ /R /Y
xcopy THANKS    %DESTINATION%\ /R /Y

xcopy varia\configuration\*.ui        %DESTINATION%\configuration\ /C /Q /H /R /Y 
xcopy varia\syntax\chat\*.syntax      %DESTINATION%\syntax\chat\ /C /Q /H /R /Y 
xcopy varia\syntax\infopanel\*.syntax %DESTINATION%\syntax\infopanel\ /C /Q /H /R /Y 
xcopy translations\*.qm               %DESTINATION%\translations\ /C /Q /H /R /Y

xcopy varia\themes\emoticons\penguins\* %DESTINATION%\themes\emoticons\penguins\ /C /Q /H /R /Y
xcopy varia\themes\icons\default\*      %DESTINATION%\themes\icons\default\ /E /C /Q /H /R /Y
xcopy varia\themes\sounds\default\*     %DESTINATION%\themes\sounds\default\ /C /Q /H /R /Y

xcopy modules\gg_avatars\data\chat\*      %DESTINATION%\syntax\chat\ /C /Q /H /R /Y 
xcopy modules\gg_avatars\data\infopanel\* %DESTINATION%\syntax\infopanel\ /C /Q /H /R /Y 

ECHO Copying modules
cd modules
for /D %%F in (*) do (
	IF EXIST %%F\*.dll (
		xcopy %%F\*.dll  %DESTINATION%\modules\ /C /Q /H /R /Y
		xcopy %%F\*.desc %DESTINATION%\modules\ /C /Q /H /R /Y
		xcopy %%F\configuration\*.ui %DESTINATION%\modules\configuration\ /C /Q /H /R /Y
		xcopy %%F\*.qm   %DESTINATION%\modules\translations\ /C /Q /H /R /Y
		xcopy %%F\data\* %DESTINATION%\modules\data\%%F\ /C /Q /H /R /Y /E
	)
)
cd ..

ECHO Copying Qt
xcopy %QT_DIR%Qt3Support4.dll  %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtCore4.dll      %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtGui4.dll       %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtNetwork4.dll   %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtSql4.dll       %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtWebKit4.dll    %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtXml4.dll       %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%mingwm10.dll     %DESTINATION%\ /C /Q /H /R /Y

ECHO Copying Qt plugins
xcopy %QT_DIR%..\plugins\imageformats\qgif4.dll  %DESTINATION%\plugins\imageformats\ /C /Q /H /R /Y
xcopy %QT_DIR%..\plugins\imageformats\qjpeg4.dll %DESTINATION%\plugins\imageformats\ /C /Q /H /R /Y
xcopy %QT_DIR%..\plugins\imageformats\qmng4.dll  %DESTINATION%\plugins\imageformats\ /C /Q /H /R /Y
xcopy %QT_DIR%..\plugins\imageformats\qsvg4.dll  %DESTINATION%\plugins\imageformats\ /C /Q /H /R /Y
xcopy %QT_DIR%..\plugins\imageformats\qtiff4.dll %DESTINATION%\plugins\imageformats\ /C /Q /H /R /Y
xcopy %QT_DIR%..\plugins\imageformats\qico4.dll  %DESTINATION%\plugins\imageformats\ /C /Q /H /R /Y

ECHO [Paths] > %DESTINATION%\qt.conf
ECHO Plugins = plugins >> %DESTINATION%\qt.conf

ECHO Copying libgadu
xcopy %LIBGADU_DIR%libgadu.dll %DESTINATION%\ /C /Q /H /R /Y

ECHO Copying libsndfile
xcopy %LIBSNDFILE_DIR%libsndfile-1.dll %DESTINATION%\ /C /Q /H /R /Y

ECHO Copying libcrypto
xcopy %OPENSSL_DIR%bin\libcrypto-8.dll %DESTINATION%\ /C /Q /H /R /Y

ECHO Copying zlib
xcopy %ZLIB_DIR%zlib1.dll              %DESTINATION%\ /C /Q /H /R /Y

ECHO Done
