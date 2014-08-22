/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef IMAGE_STORAGE_SERVICE_H
#define IMAGE_STORAGE_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class QUrl;

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class ImageStorageService
 * @short Service for handling independent image storage.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service handles independent image storage. Kadu needs to store some images under profile directory to make it
 * available even after moving profile to another computer or another drive. This service allows storing images in this
 * directory and supports kaduimg:// scheme for WebKit views to enable access to these images.
 */
class KADUAPI ImageStorageService : public QObject
{
	Q_OBJECT

	QString StoragePath;

public:
	explicit ImageStorageService(QObject *parent = 0);
	virtual ~ImageStorageService();

	/**
	 * @short Returns full path to image storage directory.
	 * @author Rafał 'Vogel' Malinowski
	 * @return full path to image storage directory
	 */
	QString storagePath() const;

	/**
	 * @short Returns full path to given image.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageFilePath image to get full path
	 * @return full path to given image
	 *
	 * If imageFilePath is absolute path then imageFilePath is returned. In other case result is imageFilePath concatenated
	 * with storagePath() result.
	 */
	QString fullPath(const QString &imageFilePath);

	/**
	 * @short Store image under given path to image storage and return new file path relative to image storage directory.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageFilePath image to save under image storage
	 * @return relative path to new image
	 *
	 * If storing image in image storage is not possible (for example directory is not writable) then this method does
	 * nothing and returns original imageFilePath value. If storing was done properly then file path relative to image
	 * storage directory is returned. It can be then passed to fullPath() method to obtain full path.
	 */
	QString storeImage(const QString &imageFilePath);

	/**
	 * @short Store image under given path to image storage and return new file path relative to image storage directory.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageFileName name of file to create
	 * @param content content of file to create
	 * @return relative path to new image
	 *
	 * If storing image in image storage is not possible (for example directory is not writable) then this method does
	 * nothing and empty QString(). If storing was done properly then file path relative to image storage directory is
	 * returned. It can be then passed to fullPath() method to obtain full path.
	 */
	QString storeImage(const QString &imageFileName, const QByteArray &content);

	/**
	 * @short Return QUrl with file:// scheme for given kaduimg:// QUrl.
	 * @author Rafał 'Vogel' Malinowski
	 * @param url url to convert
	 * @return converted url
	 *
	 * If passed url is not of kaduimg:// scheme then it is returned without changes. In other case its path() property is
	 * expanded to full path and returned as new file:// url. If destination file does not exists then url pointing to
	 * wait animation is returned.
	 */
	QUrl toFileUrl(const QUrl &url);

};

/**
 * @}
 */

#endif // IMAGE_STORAGE_SERVICE_H
