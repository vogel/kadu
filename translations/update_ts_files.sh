#!/usr/bin/env bash

PWD=`pwd`
LOG=$PWD/update_ts_file.log

PROCESSONLY=""
if [ -n "$1" ]; then
	PROCESSONLY="$1"
	echo "Only processing \"$PROCESSONLY\"";
fi

XSLT_PROCESSOR=$(which saxon-xslt) || XSLT_PROCESSOR=$(which saxon8) || XSLT_PROCESSOR=$(which xt) || XSLT_PROCESSOR=saxon-xslt

LUPDATE=$(which lupdate-qt4) || LUPDATE=$(which lupdate) || LUPDATE=$QTDIR/bin/lupdate

if [ -z "$PROCESSONLY" ] || [ "$PROCESSONLY" = "kadu" ] || [ "$PROCESSONLY" = "kadu-core" ]; then
	echo "Updating kadu-core translations"
	echo > $LOG

	# empty fake file
	echo > ../kadu-core/.configuration-ui-translations.cpp
	for i in `ls ../varia/configuration/*.ui`; do
		$XSLT_PROCESSOR $i configuration-ui.xsl >> ../kadu-core/.configuration-ui-translations.cpp 2>> $LOG
	done

	# all .cpp files in kadu_core subdirectories
	SRC_FILES=`find ../kadu-core/ -type f -name *.cpp`

	for ts in *.ts; do
		$LUPDATE -locations none -noobsolete -verbose $SRC_FILES -ts $ts  >> $LOG 2>&1 || \
		( rm $ts && $LUPDATE -locations none -noobsolete -verbose $SRC_FILES -ts $ts  >> $LOG 2>&1 )
	done

fi

pushd ../plugins/ >> $LOG
for PLUGIN in *; do
	if [ ! -d $PLUGIN ]; then
		continue
	fi

	if [ ! -f $PLUGIN/$PLUGIN.desc ]; then
		continue;
	fi

	if [ -n "$PROCESSONLY" ] && [ "$PROCESSONLY" != "$PLUGIN" ]; then
		continue;
	fi

	echo "Updating plugin $PLUGIN translations"

	UI_TRANS=

	pushd $PLUGIN >> $LOG 2>&1

	if [ -d configuration ]; then
		UI_TRANS=.configuration-ui-translations.cpp

		pushd configuration >> $LOG 2>&1
		echo > ../.configuration-ui-translations.cpp
		for i in *.ui; do
			$XSLT_PROCESSOR $i ../../../translations/configuration-ui.xsl >> ../.configuration-ui-translations.cpp 2>> $LOG
		done
		popd >> $LOG 2>&1
	fi

	if [ -x ./translations/extract-custom-strings.js ]; then
		./translations/extract-custom-strings.js
	fi

	if [ -d data/configuration ]; then
		UI_TRANS=.configuration-ui-translations.cpp

		pushd data/configuration >> $LOG 2>&1
		echo > ../.configuration-ui-translations.cpp
		for i in *.ui; do
			$XSLT_PROCESSOR $i ../../../../translations/configuration-ui.xsl >> ../../.configuration-ui-translations.cpp 2>> $LOG
		done
		popd >> $LOG 2>&1
	fi

	if [ ! -d translations ]; then
		mkdir translations;
	fi

	SRC_FILES=`find . -type f -name "*.cpp"`

	for TS in `ls translations/*.ts`; do
		$LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts $TS || \
		( rm $TS && $LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts $TS )
	done
	if [ ! -f translations/${PLUGIN}_en.ts ]; then
		$LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${PLUGIN}_en.ts || \
		( rm translations/${PLUGIN}_en.ts && $LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${PLUGIN}_en.ts )
	fi

	popd >> $LOG 2>&1
done
popd >> $LOG 2>&1
