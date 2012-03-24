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

for main_dir in modules plugins;
do
	pushd ../$main_dir/ >> $LOG
	for module in *; do
		if [ ! -d $module ]; then
			continue
		fi

		if [ ! -f $module/$module.desc ]; then
			continue;
		fi

		if [ -n "$PROCESSONLY" ] && [ "$PROCESSONLY" != "$module" ]; then
			continue;
		fi

		echo "Updating plugin $module translations"

		UI_TRANS=

		pushd $module >> $LOG 2>&1

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

		for ts in translations/*.ts; do
			$LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts $ts || \
			( rm $ts && $LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts $ts )
		done
		if [ ! -f translations/${module}_en.ts ]; then
			$LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${module}_en.ts || \
			( rm translations/${module}_en.ts && $LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${module}_en.ts )
		fi

		popd >> $LOG 2>&1
	done
	popd >> $LOG 2>&1
done
popd >> $LOG 2>&1
