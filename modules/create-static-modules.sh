#!/usr/bin/env bash

echo -n > static_modules.cpp

for module in $*;
do
	echo "extern \"C\" int ${module}_init(bool);" >> static_modules.cpp
	echo "extern \"C\" void ${module}_close();" >> static_modules.cpp
done

echo >> static_modules.cpp

echo "void ModulesManager::registerStaticModules()" >> static_modules.cpp
echo "{" >> static_modules.cpp

for module in $*;
do
	echo "	registerStaticModule(\"${module}\", ${module}_init, ${module}_close);" >> static_modules.cpp
done

echo "}" >> static_modules.cpp
