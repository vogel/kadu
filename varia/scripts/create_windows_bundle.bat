@ECHO OFF

set DESTINATION="C:\kadu\git-install"
set QT_DIR="C:\Qt\2010.05\qt\bin"
set QT_PLUGINS_DIR="C:\Qt\2010.05\qt\plugins"
set QT_TRANSLATIONS_DIR="C:\Qt\2010.05\qt\translations"
set QT_DEBUG_SUFFIX=""
set LIBGADU_DIR="C:\kadu\deps\libgadu-1.10.0-win32"
set LIBICONV_DIR="C:\kadu\deps\libiconv-1.13.1"
set LIBIDN_DIR="C:\kadu\deps\libidn-1.19"
set LIBINTL_DIR="C:\kadu\deps\libintl-0.17"
set LIBSNDFILE_DIR="C:\kadu\deps\libsndfile-1.0.23"
set OPENSSL_DIR="C:\kadu\deps\openssl-0.9.8q"
set QCA_DIR="C:\kadu\deps\qca-2.0.3\bin"
set QCA_OSSL_DIR="C:\kadu\deps\qca-ossl-2.0.0_beta3"
set ZLIB_DIR="C:\kadu\deps\zlib-1.2.5"

ECHO Set proper paths and uncomment this line
REM EXIT

ECHO Removing existing installation
rd %DESTINATION% /S /Q

ECHO Copying Kadu core
xcopy build\kadu-core\kadu*.exe %DESTINATION%\ /R /Y /Q
xcopy build\kadu-core\kadu*.dll %DESTINATION%\ /R /Y /Q
xcopy kadu-core\kadu.ico        %DESTINATION%\ /R /Y /Q
xcopy AUTHORS                   %DESTINATION%\ /R /Y /Q
xcopy ChangeLog                 %DESTINATION%\ /R /Y /Q
xcopy ChangeLog.OLD-PL          %DESTINATION%\ /R /Y /Q
xcopy COPYING                   %DESTINATION%\ /R /Y /Q
xcopy HISTORY                   %DESTINATION%\ /R /Y /Q
xcopy README                    %DESTINATION%\ /R /Y /Q
xcopy THANKS                    %DESTINATION%\ /R /Y /Q

xcopy kadu-core\chat\style-engines\chat-scripts.js %DESTINATION%\scripts\ /C /H /R /Y /Q

xcopy varia\configuration\*.ui        %DESTINATION%\configuration\ /C /H /R /Y /Q
xcopy varia\syntax\chat\*.syntax      %DESTINATION%\syntax\chat\ /C /H /R /Y /Q

cd varia\syntax\chat
for /D %%F in (*) do (
	IF EXIST %%F\Contents (
		xcopy "%%F" %DESTINATION%\syntax\chat\"%%F" /C /H /R /Y /S /I /Q
	)
)
xcopy Default %DESTINATION%\syntax\chat\Default /C /H /R /Y /S /I /Q
cd ..\..\..

xcopy varia\syntax\infopanel\*.syntax %DESTINATION%\syntax\infopanel\ /C /H /R /Y /Q
xcopy build\translations\*.qm         %DESTINATION%\translations\ /C /H /R /Y /Q

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
cd build\modules
for /D %%F in (*) do (
	IF EXIST %%F\*.a (
		IF EXIST %%F\*.dll (
		  xcopy %%F\*.dll  %DESTINATION%\modules\ /C /H /R /Y /Q
		)
		xcopy ..\..\modules\%%F\*.desc %DESTINATION%\modules\ /C /H /R /Y /Q
		IF EXIST ..\..\modules\%%F\configuration\*.ui (
			xcopy ..\..\modules\%%F\configuration\*.ui %DESTINATION%\modules\configuration\ /C /H /R /Y /Q
		)
		IF EXIST ..\..\modules\%%F\data\configuration\*.ui (
			xcopy ..\..\modules\%%F\data\configuration\*.ui %DESTINATION%\modules\configuration\ /C /H /R /Y /Q
		)
		IF EXIST ..\..\modules\%%F\*.qm (
			xcopy ..\..\modules\%%F\*.qm   %DESTINATION%\modules\translations\ /C /H /R /Y /Q
		)
		IF EXIST ..\..\modules\%%F\data\* (
			xcopy ..\..\modules\%%F\data\* %DESTINATION%\modules\data\%%F\ /C /H /R /Y /E /Q
		)
	)
)
echo LoadByDefault=true >> %DESTINATION%\modules\qt4_docking_notify.desc
echo LoadByDefault=true >> %DESTINATION%\modules\qt4_sound.desc
cd ..\..

ECHO Copying Qt
xcopy %QT_DIR%\libgcc_s_dw2-1.dll                  %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\mingwm10.dll                        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\phonon%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtCore%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtGui%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtNetwork%QT_DEBUG_SUFFIX%4.dll     %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtScript%QT_DEBUG_SUFFIX%4.dll      %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtScriptTools%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtSql%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtSvg%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtWebKit%QT_DEBUG_SUFFIX%4.dll      %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtXml%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtXmlPatterns%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying Qt plugins
xcopy %QT_PLUGINS_DIR%\iconengines\qsvgicon%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\plugins\iconengines\  /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qgif%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qico%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qjpeg%QT_DEBUG_SUFFIX%4.dll   %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qmng%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qsvg%QT_DEBUG_SUFFIX%4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qtiff%QT_DEBUG_SUFFIX%4.dll   %DESTINATION%\plugins\imageformats\ /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\sqldrivers\qsqlite%QT_DEBUG_SUFFIX%4.dll   %DESTINATION%\plugins\sqldrivers\   /C /H /R /Y /Q

ECHO Copying Qt translations
xcopy %QT_TRANSLATIONS_DIR%\qt_cs.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%\qt_de.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%\qt_fr.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%\qt_it.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%\qt_pl.qm %DESTINATION%\translations\  /C /H /R /Y /Q

ECHO [Paths] > %DESTINATION%\qt.conf
ECHO Plugins = plugins >> %DESTINATION%\qt.conf
ECHO Translations = translations >> %DESTINATION%\qt.conf

ECHO Copying QCA
xcopy %QCA_DIR%\qca2.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying QCA OSSL Plugin
xcopy %QCA_OSSL_DIR%\qca-ossl2.dll  %DESTINATION%\plugins\crypto\ /C /H /R /Y /Q

ECHO Copying OpenSSL
xcopy %OPENSSL_DIR%\libeay32.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %OPENSSL_DIR%\libssl32.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libsndfile
xcopy %LIBSNDFILE_DIR%\libsndfile-1.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libgadu
xcopy %LIBGADU_DIR%\libgadu.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libiconv
xcopy %LIBICONV_DIR%\libiconv-2.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libidn
xcopy %LIBIDN_DIR%\libidn-11.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libintl
xcopy %LIBINTL_DIR%\libintl-8.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying zlib
xcopy %ZLIB_DIR%\zlib1.dll %DESTINATION%\ /C /H /R /Y /Q

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

ECHO Done
