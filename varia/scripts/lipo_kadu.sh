#!/usr/bin/env bash
# skrypt ma za zadanie stworzenie wersji universal binary z wszystkich binarnych plików u¿ywanych przez Kadu
# autorzy: max & joi
# licencja: gpl v2 lub wy¿sza

KINTEL=~/Desktop/kaduintel/Kadu.app
KPPC=~/Desktop/kaduppc/Kadu.app
KUB=~/Desktop/Kadu.app

lipo $KINTEL/Contents/MacOS/kadu $KPPC/Contents/MacOS/kadu -create -output $KUB/Contents/MacOS/kadu
lipo $KINTEL/Contents/MacOS/playsound $KPPC/Contents/MacOS/playsound -create -output $KUB/Contents/MacOS/playsound

#na makintelu te biblioteki s± UB, wiêc nie trzeba nic lipowaæ
cp $KINTEL/Contents/Frameworks/libssl.*.dylib $KUB/Contents/Frameworks/
cp $KINTEL/Contents/Frameworks/libcrypto.*.dylib $KUB/Contents/Frameworks/

for i in $KINTEL/kadu/modules/*.dylib; do
	MODNAME=`basename $i`
	lipo $KINTEL/kadu/modules/$MODNAME $KPPC/kadu/modules/$MODNAME -create -output $KUB/kadu/modules/$MODNAME
done
