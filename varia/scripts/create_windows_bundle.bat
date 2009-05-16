rem @echo off

set DESTINATION="C:\Program Files\Kadu-0.6.5.2"
set QT_DIR="C:\Qt\4.5.1\bin\"
set LIBGADU_DIR="C:\Qt\libgadu-win32\"
set LIBSNDFILE_DIR="C:\Qt\libsndfile\"

xcopy kadu-core\kadu*.exe %DESTINATION%\ /R /Y
xcopy kadu-core\kadu*.dll %DESTINATION%\ /R /Y
xcopy AUTHORS   %DESTINATION%\ /R /Y
xcopy ChangeLog %DESTINATION%\ /R /Y
xcopy COPYING   %DESTINATION%\ /R /Y
xcopy HISTORY   %DESTINATION%\ /R /Y
xcopy README    %DESTINATION%\ /R /Y
xcopy THANKS    %DESTINATION%\ /R /Y

xcopy varia\configuration\*.ui %DESTINATION%\configuration\ /C /Q /H /R /Y 
xcopy varia\syntax\chat\*.syntax %DESTINATION%\syntax\chat\ /C /Q /H /R /Y 
xcopy varia\syntax\infopanel\*.syntax %DESTINATION%\syntax\infopanel\ /C /Q /H /R /Y 
xcopy translations\*.qm %DESTINATION%\translations\ /C /Q /H /R /Y

xcopy varia\themes\emoticons\penguins\* %DESTINATION%\themes\emoticons\penguins\ /C /Q /H /R /Y
xcopy varia\themes\icons\default\* %DESTINATION%\themes\icons\default\ /E /C /Q /H /R /Y
xcopy varia\themes\sounds\default\* %DESTINATION%\themes\sounds\default\ /C /Q /H /R /Y

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

xcopy %QT_DIR%Qt3Support4.dll  %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtCore4.dll      %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtGui4.dll       %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtNetwork4.dll   %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtSql4.dll       %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtWebKit4.dll    %DESTINATION%\ /C /Q /H /R /Y
xcopy %QT_DIR%QtXml4.dll       %DESTINATION%\ /C /Q /H /R /Y
xcopy %LIBGADU_DIR%libgadu.dll %DESTINATION%\ /C /Q /H /R /Y
xcopy %LIBSNDFILE_DIR%libsndfile-1.dll %DESTINATION%\ /C /Q /H /R /Y
