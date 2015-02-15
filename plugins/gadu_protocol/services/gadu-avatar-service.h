/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_AVATAR_SERVICE_H
#define GADU_AVATAR_SERVICE_H

#include "protocols/services/avatar-service.h"

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduAvatarService
 * @short Service for downloading and uploading avatars for Gadu-Gadu protocol.
 * @see AvatarService
 * @author Rafał 'Vogel' Malinowski
 *
 * See documentation of AvatarService to get general information about this service.
 *
 * GaduAvatarService does not require any protocol data or session as it uses HTTP communication channel instead
 * of Gadu Gadu one. Downloading avatars is possible without any authorization. Uploading avatar is done with
 * OAuth authorization on http://avatars.nowe.gg/upload webservice.
 *
 * Both AvatarDownloader and AvatarUploader instances are available at any time.
 */
class GaduAvatarService : public AvatarService
{
	Q_OBJECT

public:
	/**
	 * @short Create service instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param account account of service
	 * @param parent QObject parent of service
	 */
	explicit GaduAvatarService(Account account, QObject *parent = 0);
	virtual ~GaduAvatarService();

	virtual AvatarDownloader * createAvatarDownloader() override;
	virtual AvatarUploader * createAvatarUploader() override;
	virtual bool eventBasedUpdates() override { return true; }

};

/**
 * @}
 */

#endif // GADU_AVATAR_SERVICE_H
