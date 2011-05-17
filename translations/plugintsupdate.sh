#!/usr/bin/env bash

echo "Updating translation files in '$1'..."

pushd "$1" 2>&1 > /dev/null

DIR=`dirname "$0"`

PLUGIN=`basename "$(pwd)"`

XSLT_PROCESSOR=$(which saxon-xslt) || XSLT_PROCESSOR=$(which saxon8) || XSLT_PROCESSOR=$(which xt) || XSLT_PROCESSOR=saxon-xslt

LUPDATE=$(which lupdate-qt4) || LUPDATE=$(which lupdate) || LUPDATE=$QTDIR/bin/lupdate

if [ ! -f $PLUGIN.desc ]; then
	echo "This is not a plugin directory."
	exit 1;
fi

UI_TRANS=""

if [ -d configuration ]; then
	UI_TRANS=.configuration-ui-translations.cpp
	pushd configuration 2>&1 > /dev/null
	echo > ../.configuration-ui-translations.cpp
	for i in *.ui; do
		$XSLT_PROCESSOR $i $DIR/configuration-ui.xsl >> ../.configuration-ui-translations.cpp
	done
	popd 2>&1 > /dev/null
fi

if [ -d data/configuration ]; then
	UI_TRANS=.configuration-ui-translations.cpp
	pushd data/configuration 2>&1 > /dev/null
	echo > ../../.configuration-ui-translations.cpp
	for i in *.ui; do
		$XSLT_PROCESSOR $i $DIR/configuration-ui.xsl >> ../../.configuration-ui-translations.cpp
	done
	popd 2>&1 > /dev/null
fi

if [ ! -d translations ]; then
	mkdir translations;
fi

SRC_FILES=`find . -type f -name "*.cpp"`

for ts in translations/*.ts; do
	$LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts $ts || \
		( rm $ts && $LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts $ts )
done
if [ ! -f translations/${PLUGIN}_en.ts ]; then
	$LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${PLUGIN}_en.ts || \
		( rm translations/${PLUGIN}_en.ts && $LUPDATE -locations none -noobsolete -verbose $SRC_FILES ${UI_TRANS} -ts translations/${PLUGIN}_en.ts )
fi

popd 2>&1 > /dev/null
