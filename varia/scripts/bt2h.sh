#!/bin/sh
# skrypt wyci±gaj±cy z backtrace'a wyrzucanego przez kadu
# informacje o numerach linii
# póki co nie dzia³a to dla modu³ów :/

# autor: joi
# licencja: gplv2

SC=
function findbin()
{
	OLDIFS=$IFS
	IFS=":"
	for i in $PATH;
	do
		if [ -f $i/$1 ];
		then
			SC=$i
			IFS=$OLDIFS
			return 0
		fi
	done

	IFS=$OLDIFS
	SC=""
	return 1
}

#findbin kadu && echo $SC

bt=`cat - | grep "^\[" | sed "s/^\(\[[0-9]*\]\) \(.*\)(\(.*\)+[0-9xa-f]*) \[\(0x.*\)\]$/\2 \3 \4/" | grep -v "^\[" | sed "s/bin\/\.\.\///g"`

OLDIFS=$IFS
IFS="
"

for i in $bt;
do
	file=`echo $i | awk '{print $1}'`
	fun=`echo $i | awk '{print $2}'`
	adr=`echo $i | awk '{print $3}'`
	if [ `echo -n $file | grep "^[/|\.]" | wc -l` -eq 0 ];
	then
		findbin $file && file=$SC/$file
	fi
	echo -n "$fun "| c++filt
	addr2line -e $file $adr
done

IFS=$OLDIFS
