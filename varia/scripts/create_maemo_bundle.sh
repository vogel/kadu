#!/bin/sh

################################################################################
#
# Create Kadu deb package for Maemo 5
#
# Libgadu, QCA2 and qca-ossl plugin are required.
# It is assumed that those libraries are compiled and installed with PREFIX set
# to /home/opt/kadu and the qca-ossl plugin is installed in Qt plugins directory
# (/usr/lib/qt4/plugins/crypto/libqca-ossl.so).
#
################################################################################

echo "Check script configuration and remove this lines (including exit)"
echo "Sprawdz konfiguracje skryptu i usun te linie (lacznie z exit)"
echo
exit


################################################################################
#
# Definitions:
# * Kadu version number 
# * build number
# * prefix where kadu and libraries are installed
# * temporary directory
#
################################################################################

VERSION=0.6.6-beta2
RELEASE=t4
PREFIX=/home/opt/kadu
TMP_DIR=tmp


################################################################################
#
# Functions for generation of DEBIAN/control and DEBIAN/postinst files
#
################################################################################

create_debian_control()
{
cat > $TMP_DIR/DEBIAN/control << "END"
Package: kadu
Version: 0.6.6-beta2
Section: net
Priority: extra
Architecture: armel
Depends: libqt4-core (>= 4.6), libqt4-dbus, libqt4-gui, libqt4-network, libqt4-sql, libqt4-webkit
Maintainer: Tomasz Rostanski <rozteck@interia.pl>
Description: Kadu is a Gadu-Gadu client for online messaging written in Qt
END
}

create_debian_postinst()
{
cat > $TMP_DIR/DEBIAN/postinst << "END"
#! /bin/sh
ln -sf /home/opt/kadu/bin/kadu /usr/bin/kadu || true
ln -sf /home/opt/kadu/lib/libgadu.so.3.10.0 /usr/lib/libgadu.so || true
ln -sf /home/opt/kadu/lib/libgadu.so.3.10.0 /usr/lib/libgadu.so.3 || true
ln -sf /home/opt/kadu/lib/libgadu.so.3.10.0 /usr/lib/libgadu.so.3.10.0 || true
ln -sf /home/opt/kadu/lib/libqca.so.2.0.2   /usr/lib/libqca.so || true
ln -sf /home/opt/kadu/lib/libqca.so.2.0.2   /usr/lib/libqca.so.2  || true
ln -sf /home/opt/kadu/lib/libqca.so.2.0.2   /usr/lib/libqca.so.2.0.2 || true
mkdir -p /usr/lib/qt4/plugins/crypto || true
ln -sf /home/opt/kadu/lib/libqca-ossl.so /usr/lib/qt4/plugins/crypto/libqca-ossl.so || true
END
chmod +x $TMP_DIR/DEBIAN/postinst
}

update_kadu_desktop()
{
cat >> $TMP_DIR/usr/share/applications/hildon/kadu.desktop << "END"
X-Window-Icon=kadu
X-Window-Icon-Dimmed=kadu
X-HildonDesk-ShowInToolbar=true
X-Osso-Type=application/x-executable
MimeType=application/x-executable
END
}

################################################################################
#
# Create directories
#
################################################################################
mkdir  -p $TMP_DIR/DEBIAN
mkdir  -p $TMP_DIR/home/opt
mkdir  -p $TMP_DIR/usr/share/applications/hildon
mkdir  -p $TMP_DIR/usr/share/icons/hicolor/64x64/apps


################################################################################
#
# Copy files to temporary directory
#
################################################################################

cp -af $PREFIX $TMP_DIR/home/opt
cp -af kadu-core/kadu.desktop $TMP_DIR/usr/share/applications/hildon/
cp -af $PREFIX/share/icons/hicolor/64x64/apps/kadu.png $TMP_DIR/usr/share/icons/hicolor/64x64/apps
cp -af /usr/lib/qt4/plugins/crypto/libqca-ossl.so $TMP_DIR/home/opt/kadu/lib/

rm -f  $TMP_DIR/home/opt/kadu/bin/qca*
rm -fr $TMP_DIR/home/opt/kadu/include
rm -fr $TMP_DIR/home/opt/kadu/lib/pkgconfig
rm -f  $TMP_DIR/home/opt/kadu/lib/*.*a
rm -f  $TMP_DIR/home/opt/kadu/lib/*.debug
rm -f  $TMP_DIR/home/opt/kadu/lib/*.prl
rm -fr $TMP_DIR/home/opt/kadu/share/qca
rm -fr $TMP_DIR/home/opt/kadu/share/man


################################################################################
#
# Create package from temporary directory
#
################################################################################

update_kadu_desktop
create_debian_control
create_debian_postinst

dpkg -b $TMP_DIR kadu-$VERSION-$RELEASE.n900.deb


################################################################################
#
# Cleanup and were done
#
################################################################################

rm -fr $TMP_DIR || true

echo "Done: kadu-$VERSION-$RELEASE.n900.deb"

