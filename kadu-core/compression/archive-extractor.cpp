/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

/*
 * Some code is copied from Ark from KDE project:
 *
 * Copyright (c) 2007 Henrique Pinto <henrique.pinto@kdemail.net>
 * Copyright (c) 2008-2009 Harald Hvaal <haraldhv@stud.ntnu.no>
 * Copyright (c) 2010 Raphael Kubo da Costa <rakuco@FreeBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES ( INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION ) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * ( INCLUDING NEGLIGENCE OR OTHERWISE ) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <archive.h>
#include <archive_entry.h>

#include "archive-extractor.h"

#ifdef Q_OS_WIN
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
typedef qint64 ssize_t;
#endif

struct ArchiveExtractor::ArchiveReadCustomDeleter
{
	static inline void cleanup(struct archive *a)
	{
		if (a)
		{
			archive_read_free(a);
		}
	}
};

struct ArchiveExtractor::ArchiveWriteCustomDeleter
{
	static inline void cleanup(struct archive *a)
	{
		if (a)
		{
			archive_read_free(a);
		}
	}
};

bool ArchiveExtractor::extract(const QString &sourcePath, const QString &destPath)
{
	Message = "";
	QFileInfo info(sourcePath);
	QString filePath = info.absoluteFilePath();

	int flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_SECURE_NODOTDOT;

	QDir dest(destPath);
	if (!dest.exists())
	{
		if (!dest.mkpath(destPath))
		{
			Message = QString("Cannot create directory '%s'").arg(destPath);
			return false;
		}
	}

	qDebug() << "Setting destination path to: " << destPath;
	QDir::setCurrent(destPath);

	const bool preservePaths = true;
	QString rootNode = "";

	ArchiveRead arch(archive_read_new());

	if (!arch.data() || archive_read_support_filter_all(arch.data()) != ARCHIVE_OK
	    || archive_read_support_format_all(arch.data()) != ARCHIVE_OK
	    || archive_read_open_filename(arch.data(), QFile::encodeName(filePath), 10240) != ARCHIVE_OK)
	{
		Message = "Archive file does not contain valid Kadu emoticon theme.";
		return false;
	}

	ArchiveWrite writer(archive_write_disk_new());
	if (!(writer.data()))
	{
		Message = "Archive file does not contain valid Kadu emoticon theme.";
		return false;
	}

	archive_write_disk_set_options(writer.data(), flags);

	struct archive_entry *entry;

	QString fileBeingRenamed;

	while (archive_read_next_header(arch.data(), &entry) == ARCHIVE_OK)
	{
		fileBeingRenamed.clear();

		// retry with renamed entry, fire an overwrite query again
		// if the new entry also exists
		const bool entryIsDir = S_ISDIR(archive_entry_mode(entry));

		//we skip directories if not preserving paths
		if (!preservePaths && entryIsDir) {
			archive_read_data_skip(arch.data());
			continue;
		}

		//entryName is the name inside the archive, full path
		QString entryName = QDir::fromNativeSeparators(QFile::decodeName(archive_entry_pathname(entry)));

		if (entryName.startsWith(QLatin1Char( '/' )))
		{
			//for now we just can't handle absolute filenames in a tar archive.
			//TODO: find out what to do here!!
			//emit error(i18n("This archive contains archive entries with absolute paths, which are not yet supported by ark."));
			Message = "Archive file does not contain valid Kadu emoticon theme.";
			return false;
		}

		// entryFI is the fileinfo pointing to where the file will be
		// written from the archive
		QFileInfo entryFI(entryName);
		qDebug() << "setting path to " << archive_entry_pathname(entry);

		const QString fileWithoutPath(entryFI.fileName());

		//if we DON'T preserve paths, we cut the path and set the entryFI
		//fileinfo to the one without the path
		if (!preservePaths)
		{
			//empty filenames (ie dirs) should have been skipped already,
			//so asserting
			Q_ASSERT(!fileWithoutPath.isEmpty());

			archive_entry_copy_pathname(entry, QFile::encodeName(fileWithoutPath).constData());
			entryFI = QFileInfo(fileWithoutPath);

			//OR, if the commonBase has been set, then we remove this
			//common base from the filename
		}
		else if (!rootNode.isEmpty())
		{
			qDebug() << "Removing" << rootNode << "from" << entryName;

			const QString truncatedFilename(entryName.remove(0, rootNode.size()));
			archive_entry_copy_pathname(entry, QFile::encodeName(truncatedFilename).constData());

			entryFI = QFileInfo(truncatedFilename);
		}

		//if there is an already existing directory:
		if (entryIsDir && entryFI.exists() && !entryFI.isWritable())
		{
			qDebug() << "Warning, existing, but non-writable dir. skipping";
			Message = QString("Directory '%s' is not writable.").arg(destPath);
			archive_entry_clear(entry);
			archive_read_data_skip(arch.data());
			continue;
		}

		int header_response;
		qDebug() << "Writing " << fileWithoutPath << " to " << archive_entry_pathname(entry);
		if ((header_response = archive_write_header(writer.data(), entry)) == ARCHIVE_OK)
		{
			//if the whole archive is extracted and the total filesize is
			//available, we use partial progress
			copyData(arch.data(), writer.data());
		}
		else if (header_response == ARCHIVE_WARN)
		{
			qDebug() << "Warning while writing " << entryName;
		}
		else
		{
			qDebug() << "Writing header failed with error code " << header_response
			<< "While attempting to write " << entryName;
		}

		archive_entry_clear(entry);
	}

	bool result = archive_read_close(arch.data()) == ARCHIVE_OK;

	if (!result && Message.isEmpty())
	{
		Message = QString("Archive file is corrupted.").arg(destPath);
	}

	return result;
}

void ArchiveExtractor::copyData(struct archive *source, struct archive *dest)
{
	char buff[10240];
	ssize_t readBytes;

	readBytes = archive_read_data(source, buff, sizeof(buff));
	while (readBytes > 0)
	{
		archive_write_data(dest, buff, static_cast<size_t>(readBytes));
		if (archive_errno(dest) != ARCHIVE_OK)
		{
			qDebug() << "Error while extracting..." << archive_error_string(dest) << "(error nb =" << archive_errno(dest) << ')';
			return;
		}

		readBytes = archive_read_data(source, buff, sizeof(buff));
	}
}
