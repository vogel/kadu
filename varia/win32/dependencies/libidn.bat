if exist libidn (
	echo libidn directory already exists, skipping...
	exit /b
)

if not exist libidn-%IDNVER%.tar.gz (
	%WGET% http://ftp.gnu.org/gnu/libidn/libidn-%IDNVER%.tar.gz
	if errorlevel 1 exit /b 1
)

if exist libidn-%IDNVER%.tar %RM% libidn-%IDNVER%.tar
%SEVENZ% x libidn-%IDNVER%.tar.gz
if errorlevel 1 exit /b 1
%SEVENZ% x libidn-%IDNVER%.tar
if errorlevel 1 exit /b 1
%RM% libidn-%IDNVER%.tar

rename libidn-%IDNVER% libidn
if errorlevel 1 exit /b 1

pushd libidn\windows
if errorlevel 1 exit /b 1
VCUpgrade libidn.vcproj
if errorlevel 1 exit /b 1
%MSBUILD% libidn.vcxproj
if errorlevel 1 exit /b 1
popd

%CP% "%INSTALLPREFIX%"\libidn\windows\lib\libidn.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
