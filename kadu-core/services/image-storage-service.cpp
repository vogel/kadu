/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QUuid>

#include "core/application.h"
#include "icons/kadu-icon.h"
#include "misc/paths-provider.h"

#include "image-storage-service.h"

ImageStorageService::ImageStorageService(QObject *parent) :
		QObject(parent)
{
	StoragePath = Application::instance()->pathsProvider()->profilePath() + QLatin1String("images/");
}

ImageStorageService::~ImageStorageService()
{
}

QString ImageStorageService::storagePath() const
{
	return StoragePath;
}

QString ImageStorageService::fullPath(const QString &imageFilePath)
{
	return toFileUrl(QUrl(imageFilePath)).toLocalFile();
}

QString ImageStorageService::storeImage(const QString &imageFilePath)
{
	QFileInfo fileInfo(imageFilePath);
	if (!fileInfo.exists())
		return imageFilePath;

	QFileInfo imagesPathInfo(storagePath());
	if (fileInfo.absolutePath() == imagesPathInfo.absolutePath())
		return fileInfo.fileName();

	if (!imagesPathInfo.isDir() && !QDir().mkdir(imagesPathInfo.absolutePath()))
		return imageFilePath;

	QString copyFileName = QUuid::createUuid().toString();
	// Make this file name less exotic. First, get rid of '{' and '}' on edges.
	if (copyFileName.length() > 2)
		copyFileName = copyFileName.mid(1, copyFileName.length() - 2);

	// Second, try to add extension.
	QString ext = fileInfo.completeSuffix();
	if (!ext.isEmpty())
		copyFileName += '.' + ext;

	if (QFile::copy(imageFilePath, imagesPathInfo.absolutePath() + '/' + copyFileName))
		return copyFileName;

	return imageFilePath;
}

QString ImageStorageService::storeImage(const QString &imageFileName, const QByteArray &content)
{
	if (!QFileInfo(storagePath()).isDir() && !QDir().mkdir(storagePath()))
		return QString();

	QFile file(storagePath() + imageFileName);
	if (!file.open(QIODevice::WriteOnly))
		return QString();

	file.write(content);
	file.close();

	QFileInfo fileInfo(file.fileName());
	return fileInfo.fileName();
}

QUrl ImageStorageService::toFileUrl(const QUrl &url)
{
	if (url.scheme() != "kaduimg" && !url.scheme().isEmpty())
		return url;

	QString filePath = storagePath() + url.path();
	QFileInfo fileInfo(filePath);

	if (!fileInfo.exists())
		filePath = KaduIcon("kadu_icons/please-wait", "16x16").fullPath();

	QUrl result(url);
	result.setScheme("file");
	result.setPath(filePath);

	return result;
}

#include "moc_image-storage-service.cpp"
