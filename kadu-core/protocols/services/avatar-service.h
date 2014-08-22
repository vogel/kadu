/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "protocols/services/account-service.h"
#include "exports.h"

class Account;
class AvatarDownloader;
class AvatarUploader;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class AvatarService
 * @short Service for downloading and uploading avatars.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service can return AvatarDownloader and AvatarUploader instances that can be used to download and upload
 * avatars. If for some reason these operations are not available, null values will be returned.
 */
class KADUAPI AvatarService : public AccountService
{
	Q_OBJECT

protected:
	explicit AvatarService(Account account, QObject *parent = 0);
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
	 * @short Get AvatarDownloader for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return AvatarDownloader for this service
	 *
	 * This method will create and return AvatarDownloader class that can be used to download avatar for a contact.
	 * This method can return null if it is impossible to download an avatar.
	 *
	 * Returned instance should be used immediately and should not be stored for future use. Returned object will delete
	 * itself after one use, so next instance should be created in case first upload fails.
	 */
	virtual AvatarDownloader * createAvatarDownloader() = 0;

	/**
	 * @short Get AvatarUploader for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return AvatarUploader for this service
	 *
	 * This method will create and return AvatarUploader class that can be used to upload new avatar for account owner.
	 * This method can return null if it is impossible to upload an avatar.
	 *
	 * Returned instance should be used immediately and should not be stored for future use. Returned object will delete
	 * itself after one use, so next instance should be created in case first upload fails.
	 */
	virtual AvatarUploader * createAvatarUploader() = 0;

};

/**
 * @}
 */

#endif // AVATAR_SERVICE_H
