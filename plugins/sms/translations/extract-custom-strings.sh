#!/usr/bin/env bash

grep "translator\\.tr(\\\".*\\\")" ../data/scripts/*.js -o | grep ""\\\".*\\\" -o | awk '{ print "QT_TRANSLATE_NOOP(\"@default\", "$0");"  }' | sort | uniq > .sms-translations.cpp