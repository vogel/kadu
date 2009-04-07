#!/usr/bin/env bash

awk=$1
shift

function check_modules
{
	type=$1
	shift

	for module in $*;
	do
		$awk -F = "/^Dependencies/ { printf \$2 }" $module/$module.desc > .tmp
		deps=`cat .tmp`

		for dep_module in $deps;
		do
			dep_ok=0
			for check_dep_module in $*;
			do
				if [ "$dep_module" == "$check_dep_module" ];
				then
					dep_ok=1
					break
				fi
			done

			if [ $dep_ok -eq 0 ];
			then
				echo "Module $module dependency failed on $type module $dep_module"
				exit 1
			fi
		done
	done

	rm -f .tmp
}

modules=
for module in $*;
do
	if [ "$module" == "-" ];
	then
		check_modules "static" $modules
		continue
	fi

	modules="$modules $module"
done

check_modules "dynamic" $modules
