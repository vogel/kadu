/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef AVATAR_DOWNLOADER_H
#define AVATAR_DOWNLOADER_H

#include <QtCore/QObject>
#include <QtGui/QImage>

#include "exports.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class AvatarDownloader
 * @short Class for downloading avatars.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for downloading one avatar. To do that attach slot to avatarDownloaded() signal and call downloadAvatar()
 * method. After avatar is downloaded avatarDownloaded() signal is emitted and this object deletes itself.
 */
class KADUAPI AvatarDownloader : public QObject
{
	Q_OBJECT

protected:
	explicit AvatarDownloader(QObject *parent = 0) : QObject(parent) {}
	virtual ~AvatarDownloader() {}

public:
	/**
	 * @short Downloads avatar for given id.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id of contact to download avatar for
	 *
	 * Before calling this method attach to avatarDownloaded() signal to get informed about result. Please
	 * note that this method can be only called once. After that this object emits avatarDownloaded() and
	 * deletes itself.
	 */
	virtual void downloadAvatar(const QString &id) = 0;

signals:
	/**
	 * @short Signal emitted when job of this class is done.
	 * @author Rafał 'Vogel' Malinowski
	 * @param ok success flag
	 * @param avatar downloaded avatar
	 *
	 * If ok is true then avatar downloading was successfull. If ok is false then operation failed.
	 * If ok is true and avatar is empty then contact does not have an avatar.
	 */
	void avatarDownloaded(bool ok, QImage avatar);
};

/**
 * @}
 */

#endif // AVATAR_DOWNLOADER_H
