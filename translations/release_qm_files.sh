#!/usr/bin/env bash
LRELEASE=LRELEASE=$(which lrelease) || LRELEASE=$QTDIR/bin/lrelease
NAME=kadu
echo "*****************************************************"
echo "Angielskie -->"
$LRELEASE -verbose "$NAME"_en.ts -qm "$NAME"_en.qm
echo "*****************************************************"
echo "Polskie -->"
$LRELEASE -verbose "$NAME"_pl.ts -qm "$NAME"_pl.qm
echo "*****************************************************"
echo "Wloskie -->"
$LRELEASE -verbose "$NAME"_it.ts -qm "$NAME"_it.qm
echo "*****************************************************"
echo "Niemieckie -->"
$LRELEASE -verbose "$NAME"_de.ts -qm "$NAME"_de.qm
echo "*****************************************************"
echo "Francuskie -->"
$LRELEASE -verbose "$NAME"_fr.ts -qm "$NAME"_fr.qm
echo "*****************************************************"
