/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "atomic-file-writer.h"

#include "file-system/atomic-file-write-exception.h"

#include <QtCore/QFile>
#include <QtCore/QString>

void AtomicFileWriter::write(const QString &fileName, const QString &content)
{
	auto tmpFileName = fileName + ".tmp";
	QFile file{tmpFileName};

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		throw AtomicFileWriteException();

	file.write(content.toUtf8());
	file.close();

	QFile::remove(fileName);
	if (!QFile::rename(tmpFileName, fileName))
		throw AtomicFileWriteException();
}
