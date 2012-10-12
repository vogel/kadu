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

#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <archive.h>
#include <archive_entry.h>

#include "archive-extractor.h"


struct ArchiveExtractor::ArchiveReadCustomDeleter
{
	static inline void cleanup(struct archive *a)
	{
		if (a)
		{
			archive_read_finish(a);
		}
	}
};

struct ArchiveExtractor::ArchiveWriteCustomDeleter
{
	static inline void cleanup(struct archive *a)
	{
		if (a)
		{
			archive_write_finish(a);
		}
	}
};

bool ArchiveExtractor::extract(const QString &sourcePath, const QString &destPath)
{
	QFileInfo info(sourcePath);
	QString filePath = info.absoluteFilePath();

	int flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_SECURE_NODOTDOT;

	QDir::setCurrent(destPath);

	const bool preservePaths = true;
	QString rootNode = "";

	ArchiveRead arch(archive_read_new());

	if (!(arch.data()))
	{
		return false;
	}

	if (archive_read_support_compression_all(arch.data()) != ARCHIVE_OK)
	{
		return false;
	}

	if (archive_read_support_format_all(arch.data()) != ARCHIVE_OK)
	{
		return false;
	}

	if (archive_read_open_filename(arch.data(), QFile::encodeName(filePath), 10240) != ARCHIVE_OK)
	{
		return false;
	}

	ArchiveWrite writer(archive_write_disk_new());
	if (!(writer.data()))
	{
		return false;
	}

	archive_write_disk_set_options(writer.data(), flags);

	bool skipAll = false; // Whether to skip all files
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

		//now check if the file about to be written already exists
		if (!entryIsDir && entryFI.exists() && skipAll)
		{
			archive_read_data_skip(arch.data());
			archive_entry_clear(entry);
			continue;
		}

		//if there is an already existing directory:
		if (entryIsDir && entryFI.exists() && !entryFI.isWritable())
		{
			qDebug() << "Warning, existing, but non-writable dir. skipping";
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

	return archive_read_close(arch.data()) == ARCHIVE_OK;
}

void ArchiveExtractor::copyData(struct archive *source, struct archive *dest)
{
	char buff[10240];
	ssize_t readBytes;

	readBytes = archive_read_data(source, buff, sizeof(buff));
	while (readBytes > 0)
	{
		archive_write_data(dest, buff, readBytes);
		if (archive_errno(dest) != ARCHIVE_OK)
		{
			qDebug() << "Error while extracting..." << archive_error_string(dest) << "(error nb =" << archive_errno(dest) << ')';
			return;
		}

		readBytes = archive_read_data(source, buff, sizeof(buff));
	}
}
