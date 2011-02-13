@ECHO OFF

set DESTINATION="C:\Program Files\Kadu-0.6.6"
set QT_DIR="C:\Qt\4.7.0\bin\"
set QT_TRANSLATIONS_DIR="C:\Qt\2010.05\qt\translations\"
set QT_DEBUG_SUFFIX=""
set LIBGADU_DIR="c:\Qt\libgadu-win32\"
set LIBSNDFILE_DIR="C:\Qt\libsndfile\"
set OPENSSL_DIR="C:\Qt\mingw32-openssl-0.9.8j\bin\"
set ZLIB_DIR="C:\Qt\zlib\"
set IDN_DIR="C:\Qt\libidn\"
set QCA_DIR="C:\Qt\qca-2.0.1-mingw\bin\"
set QCA_OSSL_DIR="%QT_DIR%..\plugins\crypto\"
REM set QCA_OSSL_DIR="c:\Qt\qca-ossl-2.0.0-beta3\lib\"

ECHO Set proper paths and uncomment this line
REM EXIT

ECHO Removing existing installation
rd %DESTINATION% /S /Q

ECHO Copying Kadu core
xcopy kadu-core\kadu*.exe %DESTINATION%\ /R /Y /Q
xcopy kadu-core\kadu*.dll %DESTINATION%\ /R /Y /Q
xcopy kadu-core\kadu.ico  %DESTINATION%\ /R /Y /Q
xcopy AUTHORS   %DESTINATION%\ /R /Y /Q
xcopy ChangeLog %DESTINATION%\ /R /Y /Q
xcopy COPYING   %DESTINATION%\ /R /Y /Q
xcopy HISTORY   %DESTINATION%\ /R /Y /Q
xcopy README    %DESTINATION%\ /R /Y /Q
xcopy THANKS    %DESTINATION%\ /R /Y /Q

xcopy kadu-core\chat\style-engines\chat-scripts.js %DESTINATION%\scripts\ /C /H /R /Y /Q

xcopy varia\configuration\*.ui        %DESTINATION%\configuration\ /C /H /R /Y /Q
xcopy varia\syntax\chat\*.syntax      %DESTINATION%\syntax\chat\ /C /H /R /Y /Q

cd varia\syntax\chat\
for /D %%F in (*) do (
	IF EXIST %%F\Contents (
		xcopy "%%F" %DESTINATION%\syntax\chat\"%%F" /C /H /R /Y /S /I /Q
	)
)
xcopy Default %DESTINATION%\syntax\chat\Default /C /H /R /Y /S /I /Q
cd ..\..\..

xcopy varia\syntax\infopanel\*.syntax %DESTINATION%\syntax\infopanel\ /C /H /R /Y /Q
xcopy translations\*.qm               %DESTINATION%\translations\ /C /H /R /Y /Q

cd varia\themes\emoticons
for /D %%D in (*) do (
	IF NOT "%%D"=="CMakeFiles" (
		xcopy %%D %DESTINATION%\themes\emoticons\%%D /C /H /R /Y /S /I /Q
	)
)

cd ..\icons
for /D %%D in (*) do (
	IF NOT "%%D"=="CMakeFiles" (
		xcopy %%D %DESTINATION%\themes\icons\%%D /C /H /R /Y /S /I /Q
	)
)

cd ..\sounds
for /D %%D in (*) do (
	IF NOT "%%D"=="CMakeFiles" (
		xcopy %%D %DESTINATION%\themes\sounds\%%D /C /H /R /Y /S /I /Q
	)
)
cd ..\..\..

ECHO Copying Kadu modules
cd modules
for /D %%F in (*) do (
	IF EXIST %%F\*.a (
		IF EXIST %%F\*.dll (
		  xcopy %%F\*.dll  %DESTINATION%\modules\ /C /H /R /Y /Q
		)
		xcopy %%F\*.desc %DESTINATION%\modules\ /C /H /R /Y /Q
		IF EXIST %%F\configuration\*.ui (
		  xcopy %%F\configuration\*.ui %DESTINATION%\modules\configuration\ /C /H /R /Y /Q
		)
		IF EXIST %%F\data\configuration\*.ui (
		  xcopy %%F\data\configuration\*.ui %DESTINATION%\modules\configuration\ /C /H /R /Y /Q
		)
		IF EXIST %%F\*.qm (
		  xcopy %%F\*.qm   %DESTINATION%\modules\translations\ /C /H /R /Y /Q
		)
		IF EXIST %%F\data\* (
		  xcopy %%F\data\* %DESTINATION%\modules\data\%%F\ /C /H /R /Y /E /Q
		)
	)
)
echo LoadByDefault=true >> %DESTINATION%\modules\qt4_sound.desc
cd ..

ECHO Copying Qt
xcopy %QT_DIR%libgcc_s_dw2-1.dll                  %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%mingwm10.dll                        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtCore%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtGui%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtNetwork%QT_DEBUG_SUFFIX%4.dll     %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtScript%QT_DEBUG_SUFFIX%4.dll      %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtScriptTools%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtSql%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtSvg%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtWebKit%QT_DEBUG_SUFFIX%4.dll      %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtXml%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%QtXmlPatterns%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%phonon%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying Qt plugins
xcopy %QT_DIR%..\plugins\iconengines\qsvgicon%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\plugins\iconengines\  /C /H /R /Y /Q
xcopy %QT_DIR%..\plugins\imageformats\qgif%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_DIR%..\plugins\imageformats\qico%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_DIR%..\plugins\imageformats\qjpeg%QT_DEBUG_SUFFIX%4.dll   %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_DIR%..\plugins\imageformats\qmng%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_DIR%..\plugins\imageformats\qsvg%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_DIR%..\plugins\imageformats\qtiff%QT_DEBUG_SUFFIX%4.dll   %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_DIR%..\plugins\sqldrivers\qsqlite%QT_DEBUG_SUFFIX%4.dll   %DESTINATION%\plugins\sqldrivers\   /C /H /R /Y /Q

ECHO Copying Qt translations
xcopy %QT_TRANSLATIONS_DIR%qt_de.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%qt_fr.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%qt_it.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%qt_pl.qm %DESTINATION%\translations\  /C /H /R /Y /Q

ECHO [Paths] > %DESTINATION%\qt.conf
ECHO Plugins = plugins >> %DESTINATION%\qt.conf
ECHO Translations = translations >> %DESTINATION%\qt.conf

ECHO Copying libgadu
xcopy %LIBGADU_DIR%libgadu.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libsndfile
xcopy %LIBSNDFILE_DIR%libsndfile-1.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying zlib
xcopy %ZLIB_DIR%zlib1.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libidn
xcopy %IDN_DIR%libidn.dll %DESTINATION%\ /C /H /R /Y /Q

rem ECHO Stripping
rem cd %DESTINATION%
rem for /R %%F in (*.dll) do (
rem   strip -x "%%F" -o "%%F.stripped"
rem   MOVE /Y "%%F.stripped" "%%F"
rem )
rem for /R %%F in (*.exe) do (
rem   strip "%%F" -o "%%F.stripped"
rem   MOVE /Y "%%F.stripped" "%%F"
rem )

ECHO Copying libcrypto
xcopy %OPENSSL_DIR%libeay32.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %OPENSSL_DIR%libssl32.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying QCA
xcopy %QCA_DIR%qca2.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying QCA Plugins
xcopy %QCA_OSSL_DIR%qca-ossl2.dll  %DESTINATION%\plugins\crypto\ /C /H /R /Y /Q
xcopy %QCA_OSSL_DIR%qca-gnupg2.dll %DESTINATION%\plugins\crypto\ /C /H /R /Y /Q

ECHO Done
