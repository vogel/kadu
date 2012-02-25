@ECHO OFF

set DESTINATION="C:\kadu\git-install"
set MINGW_DIR="C:\QtSDK\mingw\bin"
set QT_DIR="C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin"
set QT_PLUGINS_DIR="C:\QtSDK\Desktop\Qt\4.8.0\mingw\plugins"
set QT_TRANSLATIONS_DIR="C:\QtSDK\Desktop\Qt\4.8.0\mingw\translations"
set QT_DEBUG_SUFFIX=""
set ASPELL_DIR="C:\kadu\deps\aspell-0.60.6-3kde"
set ASPELL_DICT_DIR="C:\kadu\deps\aspell-0.60.6-3kde\lib\aspell-0.60"
set LIBGADU_DIR="C:\kadu\deps\libgadu-1.12.0-pre"
set LIBIDN_DIR="C:\kadu\deps\libidn-1.24-win32"
set OPENSSL_DIR="C:\kadu\deps\openssl-0.9.8s"
set QCA_DIR="C:\kadu\deps\qca\bin"
set QCA_OSSL_DIR="C:\kadu\deps\qca\plugins\qca-ossl\lib"
set ZLIB_DIR="C:\kadu\deps\zlib-1.2.5"

ECHO ! Make sure to set proper paths !

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
xcopy LICENSE.OpenSSL           %DESTINATION%\ /R /Y /Q
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
xcopy translations\cs.language        %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy translations\de.language        %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy translations\en.language        %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy translations\pl.language        %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy translations\ru.language        %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy build\translations\kadu_cs.qm   %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy build\translations\kadu_de.qm   %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy build\translations\kadu_en.qm   %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy build\translations\kadu_pl.qm   %DESTINATION%\translations\ /C /H /R /Y /Q
xcopy build\translations\kadu_ru.qm   %DESTINATION%\translations\ /C /H /R /Y /Q

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

ECHO Copying Kadu plugins
cd build\plugins
for /D %%F in (*) do (
	IF EXIST %%F\*.a (
		IF EXIST %%F\*.dll (
		  xcopy %%F\*.dll  %DESTINATION%\plugins\ /C /H /R /Y /Q
		)
		xcopy ..\..\plugins\%%F\*.desc %DESTINATION%\plugins\ /C /H /R /Y /Q
		IF EXIST ..\..\plugins\%%F\configuration\*.ui (
			xcopy ..\..\plugins\%%F\configuration\*.ui %DESTINATION%\plugins\configuration\ /C /H /R /Y /Q
		)
		IF EXIST ..\..\plugins\%%F\data\configuration\*.ui (
			xcopy ..\..\plugins\%%F\data\configuration\*.ui %DESTINATION%\plugins\configuration\ /C /H /R /Y /Q
		)
		IF EXIST %%F\%%F_cs.qm (
			xcopy %%F\%%F_cs.qm   %DESTINATION%\plugins\translations\ /C /H /R /Y /Q
		)
		IF EXIST %%F\%%F_de.qm (
			xcopy %%F\%%F_de.qm   %DESTINATION%\plugins\translations\ /C /H /R /Y /Q
		)
		IF EXIST %%F\%%F_en.qm (
			xcopy %%F\%%F_en.qm   %DESTINATION%\plugins\translations\ /C /H /R /Y /Q
		)
		IF EXIST %%F\%%F_pl.qm (
			xcopy %%F\%%F_pl.qm   %DESTINATION%\plugins\translations\ /C /H /R /Y /Q
		)
		IF EXIST %%F\%%F_ru.qm (
			xcopy %%F\%%F_ru.qm   %DESTINATION%\plugins\translations\ /C /H /R /Y /Q
		)
		IF EXIST ..\..\plugins\%%F\data\* (
			xcopy ..\..\plugins\%%F\data\* %DESTINATION%\plugins\data\%%F\ /C /H /R /Y /E /Q
		)
	)
)
cd ..\..

ECHO Copying MinGW runtime
xcopy %MINGW_DIR%\libgcc_s_dw2-1.dll               %DESTINATION%\ /C /H /R /Y /Q
xcopy %MINGW_DIR%\mingwm10.dll                     %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying Qt
xcopy %QT_DIR%\phonon%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtCore%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtGui%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtNetwork%QT_DEBUG_SUFFIX%4.dll     %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtScript%QT_DEBUG_SUFFIX%4.dll      %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtSql%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtSvg%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtWebKit%QT_DEBUG_SUFFIX%4.dll      %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtXml%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\QtXmlPatterns%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying Qt plugins
xcopy %QT_PLUGINS_DIR%\bearer\qgenericbearer%QT_DEBUG_SUFFIX%4.dll     %DESTINATION%\qt-plugins\bearer\         /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\bearer\qnativewifibearer%QT_DEBUG_SUFFIX%4.dll  %DESTINATION%\qt-plugins\bearer\         /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\iconengines\qsvgicon%QT_DEBUG_SUFFIX%4.dll      %DESTINATION%\qt-plugins\iconengines\    /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qgif%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\qt-plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qico%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\qt-plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qjpeg%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\qt-plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qmng%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\qt-plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qsvg%QT_DEBUG_SUFFIX%4.dll         %DESTINATION%\qt-plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\imageformats\qtiff%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\qt-plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\phonon_backend\phonon_ds9%QT_DEBUG_SUFFIX%4.dll %DESTINATION%\qt-plugins\phonon_backend\ /C /H /R /Y /Q
xcopy %QT_PLUGINS_DIR%\sqldrivers\qsqlite%QT_DEBUG_SUFFIX%4.dll        %DESTINATION%\qt-plugins\sqldrivers\     /C /H /R /Y /Q

ECHO Copying Qt translations (cs, de, pl, ru)
xcopy %QT_TRANSLATIONS_DIR%\qt_cs.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%\qt_de.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%\qt_pl.qm %DESTINATION%\translations\  /C /H /R /Y /Q
xcopy %QT_TRANSLATIONS_DIR%\qt_ru.qm %DESTINATION%\translations\  /C /H /R /Y /Q

ECHO [Paths] > %DESTINATION%\qt.conf
ECHO Plugins = qt-plugins >> %DESTINATION%\qt.conf
ECHO Translations = translations >> %DESTINATION%\qt.conf

ECHO Copying QCA
xcopy %QCA_DIR%\qca2.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying QCA OSSL Plugin
xcopy %QCA_OSSL_DIR%\qca-ossl2.dll  %DESTINATION%\qt-plugins\crypto\ /C /H /R /Y /Q

ECHO Copying OpenSSL
xcopy %OPENSSL_DIR%\libeay32.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %OPENSSL_DIR%\libssl32.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying aspell
xcopy %ASPELL_DIR%\bin\libaspell.dll        %DESTINATION%\             /C /H /R /Y /Q
xcopy %ASPELL_DIR%\bin\libgcc_s_sjlj-1.dll  %DESTINATION%\             /C /H /R /Y /Q
xcopy %ASPELL_DIR%\bin\libiconv.dll         %DESTINATION%\             /C /H /R /Y /Q
xcopy %ASPELL_DICT_DIR%\*                   %DESTINATION%\aspell\      /C /H /R /Y /Q

ECHO Copying libgadu
xcopy %LIBGADU_DIR%\libgadu.dll %DESTINATION%\ /C /H /R /Y /Q

ECHO Copying libidn
xcopy %LIBIDN_DIR%\bin\libiconv-2.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %LIBIDN_DIR%\bin\libidn-11.dll  %DESTINATION%\ /C /H /R /Y /Q

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
