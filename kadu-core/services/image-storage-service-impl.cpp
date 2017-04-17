/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "image-storage-service-impl.h"
#include "image-storage-service-impl.moc"

#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/paths-provider.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QUuid>

ImageStorageServiceImpl::ImageStorageServiceImpl(QObject *parent) : ImageStorageService{parent}
{
}

ImageStorageServiceImpl::~ImageStorageServiceImpl()
{
}

void ImageStorageServiceImpl::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void ImageStorageServiceImpl::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void ImageStorageServiceImpl::init()
{
    m_storagePath = m_pathsProvider->profilePath() + QStringLiteral("images/");
}

QString ImageStorageServiceImpl::storagePath() const
{
    return m_storagePath;
}

QString ImageStorageServiceImpl::fullPath(const QString &imageFilePath)
{
    return toFileUrl(QUrl(imageFilePath)).toLocalFile();
}

QString ImageStorageServiceImpl::storeImage(const QString &imageFilePath)
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

QString ImageStorageServiceImpl::storeImage(const QString &imageFileName, const QByteArray &content)
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

QUrl ImageStorageServiceImpl::toFileUrl(const QUrl &url)
{
    if (url.scheme() != "kaduimg" && !url.scheme().isEmpty())
        return url;

    QString filePath = storagePath() + url.path();
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
        filePath = m_iconsManager->iconPath(KaduIcon{"kadu_icons/please-wait", "16x16"});

    QUrl result(url);
    result.setScheme("file");
    result.setPath(filePath);

    return result;
}
