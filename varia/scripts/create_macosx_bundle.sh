#!/bin/sh
# destination directory, Kadu.app will be created here
# katalog docelowy, w nim zostanie utworzony katalog Kadu.app
DEST=~/Desktop/kadu

# prefix of copiled Qt and QCA libraries
# prefiks skompilowanego Qt i QCA
QTDIR=/Users/tomek/Desktop/kadu/qt
QCADIR=/Users/tomek/Desktop/kadu/qca2/

# prefix of compiled libraries
# prefiks skompilowanych bibliotek
LIBGADU=/Users/tomek/Desktop/kadu/libgadu
LIBIDN=/Users/tomek/Desktop/kadu/libidn

#OPENSSLPATH=/Users/tomek/Desktop/kadu/openssl
OPENSSLPATH=/usr
# version of openssl, without letters
# wersja openssla, bez liter
SSLVER=0.9.7

INSTALLED_DIR=/usr/local/

echo "Set paths in this file and remove this lines (including exit)"
echo "Ustaw sciezki w tym pliku i usun te linie (lacznie z exit)"
echo
#exit

if [ ! -f VERSION ]; then
	echo "run this script from main Kadu directory"
	echo "uruchom ten skrypt z glownego katalogu Kadu"
	exit
fi

if [ ! -f ./kadu-core/kadu ]; then
	echo "compile Kadu first!"
	echo "wpierw skompiluj Kadu!"
	exit
fi

if [ ! -f ${INSTALLED_DIR}/bin/kadu ]; then
	echo "do make install first!"
	echo "wpierw zrob make install!"
	exit
fi

if [ ! -f ${QTDIR}/bin/qmake ]; then
	echo "wrong QTDIR"
	echo "QTDIR zle ustawione"
	exit
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
	<string>Copyright 2001-2014 Kadu Team</string>
</dict>
</plist>" >> ${CNT_DIR}/Info.plist

MACOS_DIR=${CNT_DIR}/MacOS;
mkdir ${MACOS_DIR}
echo "log: copying kadu binary"

RSC_DIR=${CNT_DIR}/Resources
mkdir ${RSC_DIR}
cp kadu-core/kadu.icns $RSC_DIR/
cp kadu-core/hi48-app-kadu.png ${DEST}/Kadu.app/kadu/kadu.png
cp ${INSTALLED_DIR}/bin/kadu ${MACOS_DIR}

echo "[Paths]
Plugins = plugins" > ${RSC_DIR}/qt.conf

mkdir -p ${CNT_DIR}/plugins/imageformats
mkdir -p ${CNT_DIR}/plugins/sqldrivers
mkdir -p ${CNT_DIR}/plugins/crypto
mkdir -p ${CNT_DIR}/plugins/iconengines

FM_DIR=${CNT_DIR}/Frameworks
mkdir ${FM_DIR}

if [ -f ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ]; then
	echo "log: copying ssl libraries"
	cp ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ${FM_DIR}
	cp ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
	install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
	install_name_tool -id @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libcrypto.${SSLVER}.dylib
else
	echo "log: OpenSSL not found"
	exit
fi

function copy_framework {
	NAME=$1
	if [ -f ${QTDIR}/lib/${NAME}.framework/versions/4/${NAME} ]; then
		echo "log: copying $1"
		cp ${QTDIR}/lib/${NAME}.framework/versions/4/${NAME} ${FM_DIR}
		install_name_tool -id @executable_path/../Frameworks/${NAME} ${FM_DIR}/${NAME}
	else
		echo "log: ${NAME} not found"
		exit
	fi
}

function copy_lib {
	NAME=$1
	SRC=$2
	DST=$3

	if [ -f ${SRC}/${NAME} ]; then
		echo "log: copying library ${NAME}"
		cp -f ${SRC}/${NAME} ${DST}/
		install_name_tool -id @executable_path/../${NAME} ${DST}/${NAME}
	else
		echo "log: library ${NAME} not found"
		exit
	fi
}

function update_bindings {
	BIN=$1
	echo "Updating bindings for ${BIN}"
	install_name_tool -change ${QTDIR}/lib/phonon.framework/Versions/4/phonon               @executable_path/../Frameworks/phonon                    ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore               @executable_path/../Frameworks/QtCore                    ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtDBus.framework/Versions/4/QtDBus               @executable_path/../Frameworks/QtDBus                    ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui                 @executable_path/../Frameworks/QtGui                     ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork         @executable_path/../Frameworks/QtNetwork                 ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtScript.framework/Versions/4/QtScript           @executable_path/../Frameworks/QtScript                  ${BIN}
#	install_name_tool -change ${QTDIR}/lib/QtScriptTools.framework/Versions/4/QtScriptTools @executable_path/../Frameworks/QtScriptTools             ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql                 @executable_path/../Frameworks/QtSql                     ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtSvg.framework/Versions/4/QtSvg                 @executable_path/../Frameworks/QtSvg                     ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtWebKit.framework/Versions/4/QtWebKit           @executable_path/../Frameworks/QtWebKit                  ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtXml.framework/Versions/4/QtXml                 @executable_path/../Frameworks/QtXml                     ${BIN}
	install_name_tool -change ${QTDIR}/lib/QtXmlPatterns.framework/Versions/4/QtXmlPatterns @executable_path/../Frameworks/QtXmlPatterns             ${BIN}
	install_name_tool -change ${QTDIR}/lib/qca.framework/versions/2/qca                     @executable_path/../Frameworks/qca                       ${BIN}
	install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib                  @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${BIN}
	install_name_tool -change ${LIBGADU}/lib/libgadu.3.dylib                                @executable_path/../Frameworks/libgadu.3.dylib           ${BIN}
	install_name_tool -change ${LIBIDN}/lib/libidn.11.dylib                                 @executable_path/../Frameworks/libidn.11.dylib           ${BIN}
	install_name_tool -change ${QCADIR}/lib/libqca.2.dylib                                  @executable_path/../Frameworks/libqca.2.dylib            ${BIN}
	install_name_tool -change ${QCADIR}/lib//libqca.2.dylib                                 @executable_path/../Frameworks/libqca.2.dylib            ${BIN}
	install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib                     @executable_path/../Frameworks/libssl.${SSLVER}.dylib    ${BIN}
	install_name_tool -change /Library/Frameworks/Growl.framework/Versions/A/Growl          @executable_path/../Frameworks/Growl                     ${BIN}
	install_name_tool -change @executable_path/../Frameworks/Growl.framework/Versions/A/Growl @executable_path/../Frameworks/Growl                   ${BIN}
}

copy_framework QtGui
copy_framework QtWebKit
copy_framework QtXmlPatterns
copy_framework QtXml
copy_framework QtSql
copy_framework QtSvg
copy_framework QtScript
#copy_framework QtScriptTools
copy_framework QtNetwork
copy_framework QtCore
#copy_framework QtDBus
copy_framework phonon

copy_lib libqjpeg.dylib     ${QTDIR}/plugins/imageformats ${CNT_DIR}/plugins/imageformats
copy_lib libqgif.dylib      ${QTDIR}/plugins/imageformats ${CNT_DIR}/plugins/imageformats
copy_lib libqmng.dylib      ${QTDIR}/plugins/imageformats ${CNT_DIR}/plugins/imageformats
copy_lib libqsvg.dylib      ${QTDIR}/plugins/imageformats ${CNT_DIR}/plugins/imageformats
copy_lib libqtiff.dylib     ${QTDIR}/plugins/imageformats ${CNT_DIR}/plugins/imageformats
copy_lib libqico.dylib      ${QTDIR}/plugins/imageformats ${CNT_DIR}/plugins/imageformats
copy_lib libqsqlite.dylib   ${QTDIR}/plugins/sqldrivers   ${CNT_DIR}/plugins/sqldrivers
copy_lib libqsvgicon.dylib  ${QTDIR}/plugins/iconengines  ${CNT_DIR}/plugins/iconengines
copy_lib libqca-ossl.dylib  ${QTDIR}/plugins/crypto       ${CNT_DIR}/plugins/crypto
copy_lib libidn.11.dylib    ${LIBIDN}/lib                 ${FM_DIR}
copy_lib libgadu.3.dylib    ${LIBGADU}/lib                ${FM_DIR}
copy_lib libqca.2.dylib     ${QCADIR}/lib                 ${FM_DIR}

if [ -f  /Library/Frameworks/Growl.framework/Versions/A/Growl ]; then
	echo "log: copying Growl framework"
	cp /Library/Frameworks/Growl.framework/Versions/A/Growl ${FM_DIR}
	install_name_tool -change /Library/Frameworks/Growl.framework/Versions/A/Growl @executable_path/../Frameworks/Growl ${FM_DIR}/Growl
else
	echo "log: Growl framework not found"
	exit
fi

echo "log: Changing Qt Framework bindings"
for FILE in `ls ${FM_DIR}/*`; do
	update_bindings ${FILE}
done

echo "log: Changing Crypto Plugins bindings"
for FILE in `ls ${CNT_DIR}/plugins/crypto/*.dylib`; do
	update_bindings ${FILE}
done

echo "log: Changing Imageformats Plugins bindings"
for FILE in `ls ${CNT_DIR}/plugins/imageformats/*.dylib`; do
	update_bindings ${FILE}
done

echo "log: Changing Iconengines Plugins bindings"
for FILE in `ls ${CNT_DIR}/plugins/iconengines/*.dylib`; do
	update_bindings ${FILE}
done

echo "log: Changing SQL Drivers Plugins bindings"
for FILE in `ls ${CNT_DIR}/plugins/sqldrivers/*.dylib`; do
	update_bindings ${FILE}
done

echo "log: changing Kadu bindings"
update_bindings ${MACOS_DIR}/kadu

echo "log: Changing Kadu Plugins bindings"
for FILE in `ls ${DEST}/Kadu.app/kadu/plugins/*.dylib`; do
	update_bindings ${FILE}
done

echo "log: stripping binaries"
strip -x ${FM_DIR}/Qt*  ${CNT_DIR}/plugins/imageformats/* ${MACOS_DIR}/kadu

echo "log: everything done"
