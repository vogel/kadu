#!/bin/sh
# destination directory, Kadu.app will be created here
# katalog docelowy, w nim zostanie utworzony katalog Kadu.app
DEST=~/Desktop/

QTDIR=/usr/local/Trolltech/Qt-4.4.2
LQTDIR=~/Desktop/kadu/qt

# prefix of compiled libsndfile and openssl
# prefiks skompilowanych bibliotek libsndfile i openssl
SNDFILEPATH=~/Compilation/libsndfile
LIBGADU=~/Compilation/libgadu

#OPENSSLPATH=~/Desktop/root/openssl
OPENSSLPATH=~/Compilation/openssl
# version of openssl, without letters
# wersja openssla, bez liter
SSLVER=0.9.8

#libao nie radzi sobie z mackiem jak trzeba
#AOPATH=~/Desktop/root/libao
#AOVER=2.1.3

INSTALLED_DIR=/usr/local/

ICONS_PKG=ng_big_icons.tar.bz2
ICONS_LINK=http://www.kadu.net/~neeo/kadu/macosx/$ICONS_PKG

PLAYSOUND_LINK=http://f.steike.com/playsound

echo "Set paths in this file and remove this lines (including exit)"
echo "Ustaw sciezki w tym pliku i usun te linie (lacznie z exit)"
echo
#exit

if [ ! -f VERSION ]; then
	echo "run this script from main Kadu directory"
	echo "uruchom ten skrypt z glownego katalogu Kadu"
	exit
fi

#if [ ! -f ./kadu-core/kadu ]; then
#	echo "compile Kadu first!"
#	echo "wpierw skompiluj Kadu!"
#	exit
#fi

if [ ! -f ${INSTALLED_DIR}/bin/kadu ]; then
	echo "do make install first!"
	echo "wpierw zrob make install!"
	exit
fi

if [ ! -f ${QTDIR}/include/Qt/qwidget.h ]; then
	echo "wrong QTDIR"
	echo "QTDIR zle ustawione"
	#exit
fi

if [ ! -f ./varia/themes/icons/default/kadu/big_message.png ]; then
	curl -O ${ICONS_LINK}
	tar xjf ${ICONS_PKG}
	patch -p0 < icons.diff
	cd varia
	make install
	cd ..
fi

if [ ! -f ./playsound ]; then
	curl -O ${PLAYSOUND_LINK}
	chmod a+x playsound
fi

KADUVERSION=`cat VERSION`
echo "log: removing old Kadu.app"
rm -rf ${DEST}/Kadu.app/
mkdir ${DEST}/Kadu.app/

echo "log: copying files"
cp -r ${INSTALLED_DIR}/share/kadu ${DEST}/Kadu.app
cp -r ${INSTALLED_DIR}/lib/kadu ${DEST}/Kadu.app

CNT_DIR=${DEST}/Kadu.app/Contents
mkdir ${CNT_DIR};

echo "log: creating PkgInfo"
echo "APPLKadu" > ${CNT_DIR}/PkgInfo

echo "log: creating Info.plist"
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
	<string>Copyright 2001-2007 Kadu Team</string>
</dict>
</plist>" >> ${CNT_DIR}/Info.plist


MACOS_DIR=${CNT_DIR}/MacOS;
mkdir ${MACOS_DIR}
echo "log: copying kadu binary"
cp ${INSTALLED_DIR}/bin/kadu ${MACOS_DIR}/kadu
if [ -f ./playsound ]; then
	echo "log: copying playsound binary"
	cp playsound ${MACOS_DIR}/
fi

RSC_DIR=${CNT_DIR}/Resources
mkdir ${RSC_DIR}
cp kadu.icns $RSC_DIR/
cp hi48-app-kadu.png ${DEST}/Kadu.app/kadu/kadu.png

FM_DIR=${CNT_DIR}/Frameworks
mkdir ${FM_DIR}

if [ -f ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ]; then
	echo "log: copying ssl libraries"
	cp ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ${FM_DIR}
	cp ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
	install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
	install_name_tool -id @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libcrypto.${SSLVER}.dylib
fi

#if [ -f ${QTDIR}/lib/libQtCLucene.4.4.1.dylib ]; then
#	echo "log: copying qtCLucene library"
#	cp ${QTDIR}/lib/libQtCLucene.4.4.1.dylib ${FM_DIR}
#	install_name_tool -id @executable_path/../Frameworks/libQtCLucene.4.4.1.dylib ${FM_DIR}/libQtCLucene.4.4.1.dylib
#fi

if [ -f ${QTDIR}/lib/Qt3Support.framework/versions/4/Qt3Support ]; then
	echo "log: copying qt3Support library"
	cp ${QTDIR}/lib/Qt3Support.framework/versions/4/Qt3Support ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/Qt3Support ${FM_DIR}/Qt3Support
fi

if [ -f ${QTDIR}/lib/QtGui.framework/versions/4/QtGui ]; then
	echo "log: copying qtGui library"
	cp ${QTDIR}/lib/QtGui.framework/versions/4/QtGui ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtGui ${FM_DIR}/QtGui
fi

if [ -f ${QTDIR}/lib/QtWebKit.framework/versions/4/QtWebKit ]; then
	echo "log: copying qtWebKit library"
	cp ${QTDIR}/lib/QtWebKit.framework/versions/4/QtWebKit ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtWebKit ${FM_DIR}/QtWebKit
fi

if [ -f ${QTDIR}/lib/QtXml.framework/versions/4/QtXml ]; then
	echo "log: copying qtXml library"
	cp ${QTDIR}/lib/QtXml.framework/versions/4/QtXml ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtXml ${FM_DIR}/QtXml
fi

if [ -f ${QTDIR}/lib/QtSql.framework/versions/4/QtSql ]; then
	echo "log: copying qtSql library"
	cp ${QTDIR}/lib/QtSql.framework/versions/4/QtSql ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtSql ${FM_DIR}/QtSql
fi

if [ -f ${QTDIR}/lib/QtNetwork.framework/versions/4/QtNetwork ]; then
	echo "log: copying qtNetwork library"
	cp ${QTDIR}/lib/QtNetwork.framework/versions/4/QtNetwork ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtNetwork ${FM_DIR}/QtNetwork	
fi

if [ -f ${QTDIR}/lib/QtCore.framework/versions/4/QtCore ]; then
	echo "log: copying qtCore library"
	cp ${QTDIR}/lib/QtCore.framework/versions/4/QtCore ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtCore ${FM_DIR}/QtCore
fi

if [ -f ${SNDFILEPATH}/lib/libsndfile.1.dylib ]; then
	echo "log: copying sndfile library"
	cp ${SNDFILEPATH}/lib/libsndfile.1.dylib ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/libsndfile.1.dylib ${FM_DIR}/libsndfile.1.dylib
fi

if [ -f ${LIBGADU}/lib/libgadu.3.dylib ]; then
	echo "log: copying libgadu library"
	cp ${LIBGADU}/lib/libgadu.3.dylib ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/libgadu.3.dylib ${FM_DIR}/libgadu.3.dylib
fi

cd ${FM_DIR}
install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./QtCore
install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./Qt3Support
install_name_tool -change ${QTDIR}/lib/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml ./Qt3Support
echo 1
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui ./Qt3Support
echo 2
install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork ./Qt3Support
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./Qt3Support
echo 3
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtSql
echo 4
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtXml
echo 5
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtGui
echo 6
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtWebKit
echo 7
install_name_tool -change ${LQTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtWebKit
echo 8
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtNetwork
echo 9
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui ./QtWebKit
echo 10
install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork ./QtWebKit
echo 11

cd ${MACOS_DIR}
echo "log: changing library bindings"
install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./kadu
install_name_tool -change ${LQTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./kadu
install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork ./kadu
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui ./kadu
install_name_tool -change ${QTDIR}/lib/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml ./kadu
install_name_tool -change ${QTDIR}/lib/Qt3Support.framework/Versions/4/Qt3Support @executable_path/../Frameworks/Qt3Support ./kadu
install_name_tool -change ${LQTDIR}/lib/QtWebKit.framework/Versions/4/QtWebKit @executable_path/../Frameworks/QtWebKit ./kadu
install_name_tool -change ${LQTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./kadu

#install_name_tool -change libQtCLucene.4.4.1.dylib @executable_path/../Frameworks/libQtCLucene.4.4.1.dylib ./kadu
install_name_tool -change ${LIBGADU}/lib/libgadu.3.dylib @executable_path/../Frameworks/libgadu.3.dylib ./kadu
install_name_tool -change ${SNDFILEPATH}/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib ./kadu
install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib ./kadu
install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ./kadu

if [ -f ${DEST}/Kadu.app/kadu/modules/sound.dylib ]; then
	install_name_tool -change ${SNDFILEPATH}/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib ${DEST}/Kadu.app/kadu/modules/sound.dylib
fi
if [ -f ${DEST}/Kadu.app/kadu/modules/encryption.dylib ]; then
	install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${DEST}/Kadu.app/kadu/modules/encryption.dylib
	install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${DEST}/Kadu.app/kadu/modules/encryption.dylib
fi

#if [ -f ${AOPATH}/lib/libao.2.dylib ]; then
#	cp ${AOPATH}/lib/libao.2.dylib ${FM_DIR}
#	cp ${AOPATH}/lib/libao.${AOVER}.dylib ${FM_DIR}
#	mkdir -p ${FM_DIR}/ao/plugins-2
#	cp ${AOPATH}/lib/ao/plugins-2/libmacosx.so ${FM_DIR}/ao/plugins-2/libmacosx.dylib
#fi
#if [ -f ${DEST}/Kadu.app/kadu/modules/ao_sound.dylib ]; then
#	install_name_tool -id @executable_path/../Frameworks/libao.2.dylib ${FM_DIR}/libao.2.dylib
#fi
#if [ -f ${DEST}/Kadu.app/kadu/modules/ao_sound.dylib ]; then
#	install_name_tool -change ${AOPATH}/lib/libao.2.dylib @executable_path/../Frameworks/libao.2.dylib ${DEST}/Kadu.app/kadu/modules/ao_sound.dylib
#fi

echo "everything done"
