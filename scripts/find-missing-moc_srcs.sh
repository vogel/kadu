#!/usr/bin/env bash

echo "The following header files need to be included in MOC_SRCS part of respecting CMakeLists.txt files:"

for header in $(find kadu-core modules -name "*\.h" -not -regex ".*libiris.*"); do
	if [ $(grep -c "Q_OBJECT" ${header}) -ge 1 ]; then
		if [[ "$header" = "modules/"* ]]; then
			dir=$(echo $header | sed 's/\(modules\/[^\/]*\).*/\1/')
		else
			dir="kadu-core"
		fi

		header=${header#kadu-core/}
		header=${header#modules/*/}

		if [ $(grep -ce "^[^#]*${header}" ${dir}/CMakeLists.txt) -lt 1 ]; then
			if [[ "${dir}" != "${lastDir}" ]]; then
				echo "DIR: ${dir}"
				lastDir=$dir
			fi
			echo "    $header"
		fi
	fi
done
