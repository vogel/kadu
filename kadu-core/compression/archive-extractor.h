/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QPointer>

#include "exports.h"

class KADUAPI ArchiveExtractor
{
	struct ArchiveReadCustomDeleter;
	struct ArchiveWriteCustomDeleter;

        typedef QScopedPointer<struct archive, ArchiveReadCustomDeleter> ArchiveRead;
	typedef QScopedPointer<struct archive, ArchiveWriteCustomDeleter> ArchiveWrite;

	QString Message;

	void copyData(struct archive *source, struct archive *dest);
public:
	ArchiveExtractor() {}
	virtual ~ArchiveExtractor() {}

	bool extract( const QString& sourcePath, const QString& destPath );

	QString message() { return Message; }
};

#endif // ARCHIVE_EXTRACTOR_H
