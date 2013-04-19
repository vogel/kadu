if exist xz (
	echo xz directory already exists, skipping...
	exit /b
)

if not exist xz-%XZVER%-windows.7z (
	%WGET% http://tukaani.org/xz/xz-%XZVER%-windows.7z
	if errorlevel 1 exit /b 1
)

mkdir xz
if errorlevel 1 exit /b 1

pushd xz
if errorlevel 1 exit /b 1

%SEVENZ% x ..\xz-%XZVER%-windows.7z
if errorlevel 1 exit /b 1

mkdir lib
if errorlevel 1 exit /b 1

lib /def:doc\liblzma.def /out:lib\liblzma.lib /machine:ix86
if errorlevel 1 exit /b 1

popd
if errorlevel 1 exit /b 1

%CP% "%INSTALLPREFIX%"\xz\bin_i486\liblzma.dll "%INSTALLBASE%"
if errorlevel 1 exit /b 1
