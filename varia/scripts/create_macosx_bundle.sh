#!/bin/sh
# destination directory, kadu.app will be created here
# katalog docelowy, w nim zostanie utworzony katalog kadu.app
DEST=~/Desktop

QTDIR=~/Desktop/root/qt

# prefix of compiled libsndfile and openssl
# prefiks skompilowanych bibliotek libsndfile i openssl
SNDFILEPATH=~/Desktop/root/libsndfile
#OPENSSLPATH=/Users/kadu/Desktop/root/openssl
OPENSSLPATH=/usr

# version of openssl, without letters
# wersja openssla, bez liter
SSLVER=0.9.7

INSTALLED_DIR=~/Desktop/root/kadu

echo "Set paths in this file and remove this lines (including exit)"
echo "Ustaw sciezki w tym pliku i usun te linie (lacznie z exit)"
echo
exit

if [ ! -f VERSION ]; then
	echo "run this script from main Kadu directory"
	echo "uruchom ten skrypt z glownego katalogu Kadu"
	exit
fi

if [ ! -f ./kadu/kadu ]; then
	echo "compile Kadu first!"
	echo "wpierw skompiluj Kadu!"
	exit
fi

if [ ! -f ${INSTALLED_DIR}/bin/kadu ]; then
	echo "do make install first!"
	echo "wpierw zrob make install!"
	exit
fi

KADUVERSION=`cat VERSION`
rm -rf ${DEST}/kadu.app/
mkdir ${DEST}/kadu.app/

cp -r ${INSTALLED_DIR}/share/kadu ${DEST}/kadu.app/kadu

CNT_DIR=${DEST}/kadu.app/Contents
mkdir ${CNT_DIR};

echo "APPL∂çQt" > ${CNT_DIR}/PkgInfo

echo "<?xml version='1.0' encoding='UTF-8'?>
<plist version='1.0'>
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>English</string>
	<key>CFBundleDisplayName</key>
	<string>Kadu</string>
	<key>CFBundleExecutable</key>
	<string>kadu</string>
	<key>CFBundleIconFile</key>
	<string>kadu.icns</string>
	<key>CFBundleIdentifier</key>
	<string>net.kadu</string>
	<key>CFBundleLongVersionString</key>
	<string>${KADUVERSION}</string>
	<key>CFBundleName</key>
	<string>Kadu</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>${KADUVERSION}</string>
	<key>CFBundleSignature</key>
	<string>Kadu</string>
	<key>CFBundleVersion</key>
	<string>${KADUVERSION}</string>
	<key>NSAppleScriptEnabled</key>
	<false/>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright 2001-2005 Kadu Team</string>
</dict>
</plist>" >> ${CNT_DIR}/Info.plist


MACOS_DIR=${CNT_DIR}/MacOS;
mkdir ${MACOS_DIR}
cp ${INSTALLED_DIR}/bin/kadu ${MACOS_DIR}/kadu

RSC_DIR=${CNT_DIR}/Resources
mkdir ${RSC_DIR}
cp kadu/kadu.icns $RSC_DIR/

FM_DIR=${CNT_DIR}/Frameworks
mkdir ${FM_DIR}

cp ${SNDFILEPATH}/lib/libsndfile.1.dylib ${FM_DIR}
cp ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib ${FM_DIR}
cp ${QTDIR}/lib/libqt-mt.3.dylib ${FM_DIR}

install_name_tool -id @executable_path/../Frameworks/libqt-mt.3.dylib ${FM_DIR}/libqt-mt.3.dylib
install_name_tool -id @executable_path/../Frameworks/libsndfile.1.dylib ${FM_DIR}/libsndfile.1.dylib
install_name_tool -id @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
install_name_tool -id @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libcrypto.${SSLVER}.dylib

cd ${MACOS_DIR}
install_name_tool -change libqt-mt.3.dylib @executable_path/../Frameworks/libqt-mt.3.dylib kadu
install_name_tool -change libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib kadu
install_name_tool -change libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib kadu
install_name_tool -change libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib kadu
