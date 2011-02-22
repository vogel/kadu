#!/usr/bin/env bash

PWD=`pwd`
LOG=$PWD/update_ts_file.log

LUPDATE=$(which lupdate-qt4) || LUPDATE=$(which lupdate) || LUPDATE=$QTDIR/bin/lupdate

echo "Updating kadu-core translations"
echo > $LOG

# empty fake file
echo > ../kadu-core/.configuration-ui-translations.cpp
for i in `ls ../varia/configuration/*.ui`; do
	saxon-xslt $i configuration-ui.xsl >> ../kadu-core/.configuration-ui-translations.cpp 2>> $LOG
done

# all .cpp files in kadu_core subdirectories
SRC_FILES=`find ../kadu-core/ -type f -name *.cpp`
$LUPDATE -noobsolete -verbose $SRC_FILES -ts kadu_cs.ts >> $LOG 2>&1
$LUPDATE -noobsolete -verbose $SRC_FILES -ts kadu_it.ts >> $LOG 2>&1
$LUPDATE -noobsolete -verbose $SRC_FILES -ts kadu_pl.ts >> $LOG 2>&1
$LUPDATE -noobsolete -verbose $SRC_FILES -ts kadu_en.ts >> $LOG 2>&1
$LUPDATE -noobsolete -verbose $SRC_FILES -ts kadu_de.ts >> $LOG 2>&1
$LUPDATE -noobsolete -verbose $SRC_FILES -ts kadu_fr.ts >> $LOG 2>&1

pushd ../modules/ >> $LOG
for module in *; do
	if [ ! -d $module ]; then
		continue
	fi

	if [ ! -f $module/$module.desc ]; then
		continue;
	fi

	echo "Updating module $module translations"

	UI_TRANS=

	pushd $module >> $LOG 2>&1

	if [ -d configuration ]; then
		UI_TRANS=.configuration-ui-translations.cpp

		pushd configuration >> $LOG 2>&1
		echo > ../.configuration-ui-translations.cpp
		for i in *.ui; do
			saxon-xslt $i ../../../translations/configuration-ui.xsl >> ../.configuration-ui-translations.cpp 2>> $LOG
		done
		popd >> $LOG 2>&1
	fi

	if [ -d data/configuration ]; then
		UI_TRANS=.configuration-ui-translations.cpp

		pushd data/configuration >> $LOG 2>&1
		echo > ../.configuration-ui-translations.cpp
		for i in *.ui; do
			saxon-xslt $i ../../../../translations/configuration-ui.xsl >> ../../.configuration-ui-translations.cpp 2>> $LOG
		done
		popd >> $LOG 2>&1
	fi

	if [ ! -d translations ]; then
		mkdir translations;
	fi

	SRC_FILES=`find . -type f -name "*.cpp"`
	$LUPDATE -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${module}_it.ts >> $LOG 2>&1
	$LUPDATE -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${module}_pl.ts >> $LOG 2>&1
	$LUPDATE -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${module}_en.ts >> $LOG 2>&1
	$LUPDATE -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${module}_de.ts >> $LOG 2>&1
	$LUPDATE -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${module}_fr.ts >> $LOG 2>&1

	popd >> $LOG 2>&1
done
popd >> $LOG 2>&1
