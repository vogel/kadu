#!/usr/bin/env bash

awk=$1
shift

for plugin in $*;
do
	$awk -F = "/^Dependencies/ { printf \$2 }" $plugin/$plugin.desc > .tmp
	deps=`cat .tmp`

	for dep_plugin in $deps;
	do
		dep_ok=0
		for check_dep_plugin in $*;
		do
			if [ "$dep_plugin" == "$check_dep_plugin" ];
			then
				dep_ok=1
				break
			fi
		done

		if [ $dep_ok -eq 0 ];
		then
			echo "'$plugin' plugin dependency failed on '$dep_plugin' plugin"
			exit 1
		fi
	done
done

rm -f .tmp
