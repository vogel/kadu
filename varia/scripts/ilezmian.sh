#!/bin/sh
#skrypt do zliczania ilo¶ci zmian w poszczególnych wersjach Kadu
#nale¿y go uruchomiæ bez parametrów _z katalogu_ varia/scripts
# lub z parametrem oznaczaj±cym ¶cie¿kê do ChangeLoga z dowolnego katalogu
#w bie¿acym katalogu skrypt _musi_ mieæ prawo zapisu

#autor: joi
#licencja: gpl v2

function nazwa()
{
	echo ilezmian_`printf "%02d" $i`;
}

function ipp()
{
	i=`expr $i + 1`
}

if [ "$1" == "" ]; then
	plik=../../ChangeLog
else
	plik=$1
fi

i=0
n=`nazwa`
cp $plik $n
chmod u+w $n
csplit $n "/^+++.*/+1" 2>/dev/null >/dev/null
mv xx01 $n

while [ true ];
do
	csplit $n "/^+++.*/+1" 2>/dev/null >/dev/null || break
	mv xx00 $n
	ipp
	n=`nazwa`
	mv xx01 $n
done

printf "%11s\t%5s\t%5s\n" "wersja" "wc -c" "^*|wc -l"

p=`grep "^+++" $plik|head -n 1|sed "s/+++ wersja //"`
printf "%11s\t" "$p"

i=0
n=`nazwa`
while [ -f $n ];
do
	printf "%5d\t%5d\n" `wc -c $n|cut -f1 -d" "` `cat $n|grep "^*"|wc -l`
	
	p=`grep "^+++" $n|sed "s/+++ wersja //"|cut -f1 -d" "`
	printf "%11s\t" "$p"
	rm $n
	ipp
	n=`nazwa`
done
echo
