#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Name=Kadu
Name[pl]=Kadu
GenericName=Instant Messenger
GenericName[pl]=Komunikator internetowy
Comment=Instant Messenger
Comment[pl]=Komunikator internetowy
Type=Application
Icon=kadu
Exec=$1/bin/kadu
Categories=Network;InstantMessaging;Qt;
Terminal=false
StartupNotify=true
EOF

