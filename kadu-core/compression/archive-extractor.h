/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ARCHIVE_EXTRACTOR_H
#define ARCHIVE_EXTRACTOR_H

#include <QtCore/QWeakPointer>

class ArchiveExtractor
{
	struct ArchiveReadCustomDeleter;
	struct ArchiveWriteCustomDeleter;

        typedef QScopedPointer<struct archive, ArchiveReadCustomDeleter> ArchiveRead;
	typedef QScopedPointer<struct archive, ArchiveWriteCustomDeleter> ArchiveWrite;

	void copyData(struct archive *source, struct archive *dest);
public:
	ArchiveExtractor() {}
	virtual ~ArchiveExtractor() {}

	bool extract( const QString& sourcePath, const QString& destPath );
};

#endif // ARCHIVE_EXTRACTOR_H
