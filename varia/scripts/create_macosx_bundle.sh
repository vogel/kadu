#!/bin/sh
# destination directory, kadu.app will be created here
# katalog docelowy, w nim zostanie utworzony katalog kadu.app
DEST=~/Desktop

QTDIR=~/Desktop/root/qt

# prefix of compiled libsndfile and openssl
# prefiks skompilowanych bibliotek libsndfile i openssl
SNDFILEPATH=~/Desktop/root/libsndfile
#OPENSSLPATH=~/Desktop/root/openssl
OPENSSLPATH=/usr

#libao nie radzi sobie z mackiem jak trzeba
#AOPATH=~/Desktop/root/libao

# version of openssl, without letters
# wersja openssla, bez liter
SSLVER=0.9.7
#AOVER=2.1.3

INSTALLED_DIR=~/Desktop/root/kadu

ICONS_PKG=ng_big_icons.tar.bz2
ICONS_LINK=http://www.kadu.net/~joi/macosx/$ICONS_PKG

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

if [ ! -f ./varia/themes/icons/default/big_message.png ]; then
	WGETNOTFOUND=`which wget | grep "^no" | wc -l`
	if [ ${WGETNOTFOUND} -eq 1 ]; then
		echo "wget not found, download this: ${ICONS_LINK} and copy it to the main directory of Kadu sources"
		echo "wget nie znalezione, sciagnij powyzszy plik i umiesc go w glownym katalogu zrodel Kadu"
		exit
	fi
	wget -c ${ICONS_LINK}
	tar xjf ${ICONS_PKG}
	patch -p0 < icons.diff
	cd varia
	make install
	cd ..
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

#if [ -f ${AOPATH}/lib/libao.2.dylib ]; then
#	cp ${AOPATH}/lib/libao.2.dylib ${FM_DIR}
#	cp ${AOPATH}/lib/libao.${AOVER}.dylib ${FM_DIR}
#	mkdir -p ${FM_DIR}/ao/plugins-2
#	cp ${AOPATH}/lib/ao/plugins-2/libmacosx.so ${FM_DIR}/ao/plugins-2/libmacosx.dylib
#fi

cp ${SNDFILEPATH}/lib/libsndfile.1.dylib ${FM_DIR}
cp ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib ${FM_DIR}
cp ${QTDIR}/lib/libqt-mt.3.dylib ${FM_DIR}

install_name_tool -id @executable_path/../Frameworks/libqt-mt.3.dylib ${FM_DIR}/libqt-mt.3.dylib
install_name_tool -id @executable_path/../Frameworks/libsndfile.1.dylib ${FM_DIR}/libsndfile.1.dylib
install_name_tool -id @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
#install_name_tool -id @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
install_name_tool -id @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libcrypto.${SSLVER}.dylib
#if [ -f ${DEST}/kadu.app/kadu/modules/ao_sound.dylib ]; then
#	install_name_tool -id @executable_path/../Frameworks/libao.2.dylib ${FM_DIR}/libao.2.dylib
#fi

cd ${MACOS_DIR}
install_name_tool -change libqt-mt.3.dylib @executable_path/../Frameworks/libqt-mt.3.dylib ./kadu

install_name_tool -change ${SNDFILEPATH}/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib ./kadu
install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib ./kadu
install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ./kadu

if [ -f ${DEST}/kadu.app/kadu/modules/sound.dylib ]; then
	install_name_tool -change ${SNDFILEPATH}/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib ${DEST}/kadu.app/kadu/modules/sound.dylib
fi
if [ -f ${DEST}/kadu.app/kadu/modules/encryption.dylib ]; then
	install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${DEST}/kadu.app/kadu/modules/encryption.dylib
	install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${DEST}/kadu.app/kadu/modules/encryption.dylib
fi
#if [ -f ${DEST}/kadu.app/kadu/modules/ao_sound.dylib ]; then
#	install_name_tool -change ${AOPATH}/lib/libao.2.dylib @executable_path/../Frameworks/libao.2.dylib ${DEST}/kadu.app/kadu/modules/ao_sound.dylib
#fi

echo "done"
