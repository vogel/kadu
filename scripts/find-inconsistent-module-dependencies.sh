#!/bin/bash

OK=true

for MODULE in $(find modules -mindepth 1 -maxdepth 1 -type d | cut -d/ -f2); do
	MODULE_DESCRIPTION=modules/${MODULE}/${MODULE}.desc
	MODULE_CMAKE_LISTS=modules/${MODULE}/CMakeLists.txt

	if [ ! -f $MODULE_DESCRIPTION ]; then
		echo "Missing file ${MODULE_DESCRIPTION}!"
		continue
	fi
	if [ ! -f $MODULE_CMAKE_LISTS ]; then
		echo "Missing file ${MODULE_CMAKE_LISTS}!"
		continue
	fi

	DESCRIPTION_DEPS=$(awk -F"=" '/^Dependencies=/ { print $2 }' "$MODULE_DESCRIPTION")
	# probably this _could_ be better/simpler :) the idea is to find MODULE_DEPENDENCIES that aren't like "${*}" and to remove unneeded whitespace
	CMAKE_DEPS=$(awk -F"MODULE_DEPENDENCIES " '/^[^#]*MODULE_DEPENDENCIES/ { print $2 }' "$MODULE_CMAKE_LISTS" | sed 's/${\w*}//g' | sed 's/ *\(.*\) */\1/' | tr -s ' ')

	if [[ "$DESCRIPTION_DEPS" != "$CMAKE_DEPS" ]]; then
		echo "In ${MODULE} module:"
		echo "    description dependencies are: ${DESCRIPTION_DEPS}"
		echo "    while CMake dependencies are: ${CMAKE_DEPS}"

		OK=false
	fi
done

$OK && echo "No issues found."
