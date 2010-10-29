@ECHO OFF

set QT_VERSION="4.7.0"
set DESTINATION="C:\Program Files\Kadu-0.6.6"
set QT_DIR="C:\Qt\%QT_VERSION%\bin\"
set LIBGADU_DIR="c:\Qt\libgadu-win32\"
set LIBSNDFILE_DIR="C:\Qt\libsndfile\"
set OPENSSL_DIR="C:\Qt\mingw32-openssl-0.9.8j\"
set ZLIB_DIR="C:\Qt\zlib\"
set IDN_DIR="C:\Qt\libidn\"
set QCA_DIR="C:\Qt\qca-2.0.1-mingw\bin\"
set QCA_OSSL_DIR="%QT_DIR%..\plugins\crypto\"
REM set QCA_OSSL_DIR="c:\Qt\qca-ossl-2.0.0-beta3\lib\"

ECHO Set proper paths and uncomment this line
REM EXIT

ECHO Removing existing installation
rd %DESTINATION% /S /Q

ECHO Copying kadu files
xcopy kadu-core\kadu*.exe %DESTINATION%\ /R /Y > install.log
xcopy kadu-core\kadu*.dll %DESTINATION%\ /R /Y >> install.log
xcopy kadu-core\kadu.ico  %DESTINATION%\ /R /Y >> install.log
xcopy AUTHORS   %DESTINATION%\ /R /Y >> install.log
xcopy ChangeLog %DESTINATION%\ /R /Y >> install.log
xcopy COPYING   %DESTINATION%\ /R /Y >> install.log
xcopy HISTORY   %DESTINATION%\ /R /Y >> install.log
xcopy README    %DESTINATION%\ /R /Y >> install.log
xcopy THANKS    %DESTINATION%\ /R /Y >> install.log

xcopy  kadu-core\chat\style-engines\chat-scripts.js %DESTINATION%\scripts\ /C /H /R /Y  >> install.log

xcopy varia\configuration\*.ui        %DESTINATION%\configuration\ /C /H /R /Y  >> install.log
xcopy varia\syntax\chat\*.syntax      %DESTINATION%\syntax\chat\ /C /H /R /Y  >> install.log
xcopy varia\syntax\infopanel\*.syntax %DESTINATION%\syntax\infopanel\ /C /H /R /Y  >> install.log
xcopy translations\*.qm               %DESTINATION%\translations\ /C /H /R /Y >> install.log

xcopy varia\themes\emoticons\penguins\* %DESTINATION%\themes\emoticons\penguins\ /C /H /R /Y >> install.log
xcopy varia\themes\icons\default\*      %DESTINATION%\themes\icons\default\ /E /C /H /R /Y >> install.log
xcopy varia\themes\sounds\default\*     %DESTINATION%\themes\sounds\default\ /C /H /R /Y >> install.log

ECHO Copying modules
cd modules
for /D %%F in (*) do (
	IF EXIST %%F\*.a (
		IF EXIST %%F\*.dll (
		  xcopy %%F\*.dll  %DESTINATION%\modules\ /C /H /R /Y  >> install.log
		)
		xcopy %%F\*.desc %DESTINATION%\modules\ /C /H /R /Y  >> install.log
		IF EXIST %%F\configuration\*.ui (
		  xcopy %%F\configuration\*.ui %DESTINATION%\modules\configuration\ /C /H /R /Y  >> install.log
		)
		IF EXIST %%F\data\configuration\*.ui (
		  xcopy %%F\data\configuration\*.ui %DESTINATION%\modules\configuration\ /C /H /R /Y  >> install.log
		)
		IF EXIST %%F\*.qm (
		  xcopy %%F\*.qm   %DESTINATION%\modules\translations\ /C /H /R /Y  >> install.log
		)
		IF EXIST %%F\data\* (
		  xcopy %%F\data\* %DESTINATION%\modules\data\%%F\ /C /H /R /Y /E  >> install.log
		)
	)
)
echo LoadByDefault=true >> %DESTINATION%\modules\qt4_sound.desc
cd .. 

ECHO Copying Qt
REM xcopy %QT_DIR%Qt3Support4.dll    %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtCore4.dll        %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtGui4.dll         %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtNetwork4.dll     %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtSql4.dll         %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtSvg4.dll         %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtWebKit4.dll      %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtXml4.dll         %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtScript4.dll      %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtScriptTools4.dll %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%mingwm10.dll       %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%QtXmlPatterns4.dll %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%phonon4.dll        %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%libgcc_s_dw2-1.dll %DESTINATION%\ /C /H /R /Y  >> install.log

ECHO Copying Qt plugins
xcopy %QT_DIR%..\plugins\imageformats\qgif4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%..\plugins\imageformats\qjpeg4.dll   %DESTINATION%\plugins\imageformats\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%..\plugins\imageformats\qmng4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%..\plugins\imageformats\qsvg4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%..\plugins\imageformats\qtiff4.dll   %DESTINATION%\plugins\imageformats\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%..\plugins\imageformats\qico4.dll    %DESTINATION%\plugins\imageformats\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%..\plugins\sqldrivers\qsqlite4.dll   %DESTINATION%\plugins\sqldrivers\ /C /H /R /Y  >> install.log
xcopy %QT_DIR%..\plugins\iconengines\qsvgicon4.dll %DESTINATION%\plugins\iconengines\ /C /H /R /Y  >> install.log


ECHO [Paths] > %DESTINATION%\qt.conf
ECHO Plugins = plugins >> %DESTINATION%\qt.conf

ECHO Copying libgadu
xcopy %LIBGADU_DIR%libgadu.dll %DESTINATION%\ /C /H /R /Y  >> install.log

ECHO Copying libsndfile
xcopy %LIBSNDFILE_DIR%libsndfile-1.dll %DESTINATION%\ /C /H /R /Y  >> install.log

ECHO Copying zlib
xcopy %ZLIB_DIR%zlib1.dll %DESTINATION%\ /C /H /R /Y  >> install.log

ECHO Copying libidn
xcopy %IDN_DIR%libidn.dll %DESTINATION%\ /C /H /R /Y  >> install.log

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
xcopy %OPENSSL_DIR%bin\libeay32.dll %DESTINATION%\ /C /H /R /Y  >> install.log
xcopy %OPENSSL_DIR%bin\libssl32.dll %DESTINATION%\ /C /H /R /Y  >> install.log

IF EXIST %QCA_DIR%qca2.dll (
ECHO Copying QCA
xcopy %QCA_DIR%qca2.dll %DESTINATION%\ /C /H /R /Y  >> install.log
)
IF EXIST %QCA_OSSL_DIR%qca-ossl2.dll (
ECHO Copying QCA Plugins
xcopy %QCA_OSSL_DIR%qca-ossl2.dll %DESTINATION%\plugins\crypto\ /C /H /R /Y  >> install.log
xcopy %QCA_OSSL_DIR%qca-gnupg2.dll %DESTINATION%\plugins\crypto\ /C /H /R /Y  >> install.log
)

ECHO Done
