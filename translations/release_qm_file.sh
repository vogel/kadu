#!/bin/bash
NAME=kadu
echo "*****************************************************"
echo "Polskie -->"
$QTDIR/bin/lrelease -verbose "$NAME"_pl.ts -qm "$NAME"_pl.qm
echo "*****************************************************"
echo "Wloskie -->"
$QTDIR/bin/lrelease -verbose "$NAME"_it.ts -qm "$NAME"_it.qm
echo "*****************************************************"
echo "Niemieckie -->"
$QTDIR/bin/lrelease -verbose "$NAME"_de.ts -qm "$NAME"_de.qm
echo "*****************************************************"
echo "Francuskie -->"
$QTDIR/bin/lrelease -verbose "$NAME"_fr.ts -qm "$NAME"_fr.qm
echo "*****************************************************"
