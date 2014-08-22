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

#ifndef AVATAR_UPLOADER_H
#define AVATAR_UPLOADER_H

#include <QtCore/QObject>
#include <QtGui/QImage>

#include "exports.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class AvatarUploader
 * @short Class for uploading avatars.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for uploading avatar. To do that attach slot to avatarUploaded() signal and call uploadAvatar()
 * method. After avatar is downloaded avatarUploaded() signal is emitted and this object deletes itself.
 */
class KADUAPI AvatarUploader : public QObject
{
	Q_OBJECT

protected:
	explicit AvatarUploader(QObject *parent = 0) : QObject(parent) {}
	virtual ~AvatarUploader() {}

public:
	/**
	 * @short Uploads avatar with given authentication data.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id of contact to upload avatar for
	 * @param password password of contact to upload avatar for
	 * @param avatar avatar to upload
	 *
	 * Before calling this method attach to avatarUploaded() signal to get informed about result. Please
	 * note that this method can be only called once. After that this object emits avatarUploaded() and
	 * deletes itself.
	 */
	virtual void uploadAvatar(const QString &id, const QString &password, QImage avatar) = 0;

signals:
	/**
	 * @short Signal emitted when job of this class is done.
	 * @author Rafał 'Vogel' Malinowski
	 * @param ok success flag
	 * @param avatar uploaded avatar
	 *
	 * If ok is true then avatar uploading was successfull. If ok is false then operation failed.
	 * Second parameter is version of avatar that was uploaded. Some protocols may require resizing or
	 * other operations that change avatar before uploading.
	 */
	void avatarUploaded(bool ok, QImage avatar);
};

/**
 * @}
 */

#endif // AVATAR_UPLOADER_H
