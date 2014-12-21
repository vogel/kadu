#!/usr/bin/env bash

# %kadu copyright begin%
# Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
# Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
# %kadu copyright end%
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

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

if [ -d $PACKAGE_DIR ]; then
	rm -rf $PACKAGE_DIR
fi

if [ -e $PACKAGE_FILE ]; then
	rm -rf $PACKAGE_FILE
fi

git clone --depth 1 --branch $VERSION -- git://gitorious.org/kadu/kadu.git $PACKAGE_DIR
find $PACKAGE_DIR -name ".git*" | xargs rm -rf

tar cjf $PACKAGE_FILE $PACKAGE_DIR
md5sum $PACKAGE_FILE > $PACKAGE_FILE.md5
sha1sum $PACKAGE_FILE > $PACKAGE_FILE.sha1

echo "Package is available as $PACKAGE_NAME.tar.bz2"
