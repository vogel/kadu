#!/bin/bash

OK=true

for PLUGIN in $(find plugins -mindepth 1 -maxdepth 1 -type d | cut -d/ -f2); do
	PLUGIN_DESCRIPTION=plugins/${PLUGIN}/${PLUGIN}.desc
	PLUGIN_CMAKE_LISTS=plugins/${PLUGIN}/CMakeLists.txt

	if [ ! -f $PLUGIN_DESCRIPTION ]; then
		echo "Missing file ${PLUGIN_DESCRIPTION}!"
		continue
	fi
	if [ ! -f $PLUGIN_CMAKE_LISTS ]; then
		echo "Missing file ${PLUGIN_CMAKE_LISTS}!"
		continue
	fi

	DESCRIPTION_DEPS=$(awk -F"=" '/^Dependencies=/ { print $2 }' "$PLUGIN_DESCRIPTION")
	# probably this _could_ be better/simpler :) the idea is to find PLUGIN_DEPENDENCIES that aren't like "${*}" and to remove unneeded whitespace
	CMAKE_DEPS=$(awk -F"PLUGIN_DEPENDENCIES " '/^[^#]*PLUGIN_DEPENDENCIES/ { print $2 }' "$PLUGIN_CMAKE_LISTS" | sed 's/${\w*}//g' | sed 's/ *\(.*\) */\1/' | tr -s ' ')

	if [[ "$DESCRIPTION_DEPS" != "$CMAKE_DEPS" ]]; then
		echo "In ${PLUGIN} plugin:"
		echo "    description dependencies are: ${DESCRIPTION_DEPS}"
		echo "    while CMake dependencies are: ${CMAKE_DEPS}"

		OK=false
	fi
done

$OK && echo "No issues found."
