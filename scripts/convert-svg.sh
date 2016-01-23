#!/bin/bash

sizes="16 22 32 48 64 96"

for svg_file in `find -name "*.svg"`; do
	echo "Converting $svg_file ..."
	for size in $sizes; do
		with_size=${svg_file/svg/${size}x${size}}
		png_file=${with_size/.svg/.png}
		dir=$(dirname $png_file)
		mkdir -p $dir
		echo "... to $png_file"
		rsvg-convert $svg_file -w $size -h $size -o $png_file
	done
done;
