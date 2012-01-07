#!/bin/bash

VERSION=$1

function usage
{
	echo "Usage: $0 package-version"
	echo "    package-version must be a git tag name"
}

if [ "!" == "!$VERSION" ]; then
	usage
	exit
fi

PACKAGE_NAME=kadu-$VERSION
PACKAGE_DIR=$PACKAGE_NAME
PACKAGE_FILE=$PACKAGE_NAME.tar.bz2

git clone git@gitorious.org:kadu/kadu.git $PACKAGE_DIR
pushd $PACKAGE_DIR
git checkout $VERSION
git submodule init
git submodule update
find -name ".git*" | xargs rm -rf
popd

tar cjf $PACKAGE_FILE $PACKAGE_DIR
md5sum $PACKAGE_FILE > $PACKAGE_FILE.md5
sha1sum $PACKAGE_FILE > $PACKAGE_FILE.sha1

echo "Package is available as $PACKAGE_NAME.tar.bz2"
