#!/bin/sh
find .. -iname "*.svg" -exec ./svgdebug {} ';'
find .. -iname "*.svgz" -exec ./svgdebug {} ';'
