@ECHO OFF
SETLOCAL EnableDelayedExpansion
SETLOCAL ENABLEEXTENSIONS

set TYPE=%1
set NAME=%2
set SCRIPT_DIR=%~p0
set PWD=%CD%

echo Downloading %TYPE% %NAME%

IF EXIST ..\VERSION       set /p KADU_VERSION=<"..\VERSION"
IF EXIST ..\..\VERSION    set /p KADU_VERSION=<"..\..\VERSION"
IF EXIST ..\..\..\VERSION set /p KADU_VERSION=<"..\..\..\VERSION"

set KADU_VERSION=%KADU_VERSION:~0,5%

set ITEM_WEBSITE=
IF EXIST %NAME%.web (
	CALL SET /p ITEM_WEBSITE=<"%NAME%.web"

	IF "!ITEM_WEBSITE!"=="" (
		echo ERROR: Homepage for %TYPE% %NAME% is unknown
		GOTO END
	)
	set URL=!ITEM_WEBSITE!/kadu-%KADU_VERSION%-%NAME%.href

	echo Downloading href-file !URL! to kadu-%KADU_VERSION%-%NAME%.href
	%SCRIPT_DIR%\download.vbs !URL! kadu-%KADU_VERSION%-%NAME%.href

	IF EXIST kadu-%KADU_VERSION%-%NAME%.href (
		set /P ITEM_URL=<"kadu-%KADU_VERSION%-%NAME%.href"

		set FILENAME=
		set TMP_FILE=!ITEM_URL:/= !
		FOR %%I IN (!TMP_FILE!) DO SET FILENAME=%%I
		if "!FILENAME!"=="" (
			set FILENAME=downloaded.out
		)
		
		echo Downloading !ITEM_URL! to !FILENAME!
		%SCRIPT_DIR%\download.vbs !ITEM_URL! !FILENAME!

		IF NOT EXIST %SCRIPT_DIR%\gzip.exe (
			echo Downloading decompressor
			%SCRIPT_DIR%\download.vbs http://stahlworks.com/dev/gzip.exe  %SCRIPT_DIR%\gzip.exe
			%SCRIPT_DIR%\download.vbs http://stahlworks.com/dev/bzip2.exe %SCRIPT_DIR%\bzip2.exe
			%SCRIPT_DIR%\download.vbs http://stahlworks.com/dev/tar.exe   %SCRIPT_DIR%\tar.exe
		)

		echo Decompressing !FILENAME!
		if "!FILENAME:~-2!"=="gz" (
			%SCRIPT_DIR%\gzip.exe  -d !FILENAME!
			set FILENAME=!FILENAME:~0,-3!
		) ELSE (
			%SCRIPT_DIR%\bzip2.exe -d !FILENAME!
			set FILENAME=!FILENAME:~0,-4!
		)
		%SCRIPT_DIR%\tar.exe xvf !FILENAME!

	) ELSE (
		echo ERROR: Downloading of %TYPE% %NAME% href-file kadu-%KADU_VERSION%-%NAME%.href failed
		GOTO END
	)

)

:END
