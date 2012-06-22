/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef AVATAR_SERVICE_H
#define AVATAR_SERVICE_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

#include "exports.h"

class Account;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class AvatarService
 * @short Service for feteching and uploading avatars.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service has two functions. It allows fetching avatars of known buddies and uploading own avatar.
 *
 * To fetch avatar of known buddy call fetchAvatar() method and pass an object that has avatarFetched(bool ok, QPixmap avatar)
 * slot. When fetching is done this slot will be called. First parameter is success state - if true, then fetching
 * was successfull. Second argument is fetched avatar data.
 *
 * To upload own avatar call uploadAvatar() method. It requires login id and login password parameters, as well as
 * avatar image data. When uploading is done avatarUploaded() signal will be emitted.
 */
class KADUAPI AvatarService : public QObject
{
	Q_OBJECT

protected:
	explicit AvatarService(QObject *parent = 0);
	virtual ~AvatarService();

public:
	/**
	 * @short Return avatar service for given account.
	 * @author Rafał 'Vogel' Malinowski
	 * @param account account to get avatar service from
	 * @return avatar service for given account
	 */
	static AvatarService * fromAccount(Account account);

	/**
	 * @short Fetch avatar for buddy with given id.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id of buddy to get avatar for
	 * @param receiver object with avatarFetched(bool ok,QPixmap avatar) that will be notified of finished task
	 *
	 * This method will start fetching avatar for buddy with given id. When this task is finished receiver object
	 * will be informed of that by calling its avatarFetched(bool ok, QPixmap avatar). First parameter of this slot
	 * is success state - if true, then fetching was successfull. Second argument is fetched avatar data.
	 *
	 * Nothing will happen when receiver object is null or when id is empty. When given buddy does not have an
	 * avatar then ok parameter will be true and avatar will be empty. If fetching fails for whatever reason
	 * then ok parameter will be false.
	 *
	 * It is not possible for now to get reason of failure.
	 */
	virtual void fetchAvatar(const QString &id, QObject *receiver) = 0;

	/**
	 * @short Uploads avatar for self.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id login id
	 * @param password login password
	 * @param avatar avatar to upload
	 *
	 * This method will start uploading avatar for account owner. When this task is finished avatarUploaded() slot
	 * will be emitted. If no id provided then nothing will happen.
	 *
	 * When avatar is empty then it will be removed from server. In other cases, it will be updated. Some protocols
	 * may resize or modify avatar in other way before uploading. Modified version will be returned in avatarUploaded()
	 * signal.
	 */
	virtual void uploadAvatar(const QString &id, const QString &password, QImage avatar) = 0;

signals:
	/**
	 * @short Signal emitted when uploading of avatar has finished.
	 * @author Rafał 'Vogel' Malinowski
	 * @param ok success flag
	 * @param avatar uploaded avatar
	 *
	 * This Signal emitted when uploading of avatar has finished. If ok parameter is true then uploading was successfull.
	 * Uploaded image is passed back as avatar parameter. It can be different than avatar passed in uploadAvatar() method
	 * if resizing or other modifications were required.
	 */
	void avatarUploaded(bool ok, QImage avatar);

};

/**
 * @}
 */

#endif // AVATAR_SERVICE_H
