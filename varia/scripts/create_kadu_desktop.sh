#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Name=Kadu
Type=Application
Icon=kadu
Exec=$1/bin/kadu
Categories=Network;InstantMessaging;Qt;
Terminal=false
StartupNotify=true
GenericName=Gadu-Gadu Client
GenericName[pl]=Klient Gadu-Gadu
Comment=Gadu-Gadu protocol client
Comment[pl]=Klient protokołu Gadu-Gadu
EOF

