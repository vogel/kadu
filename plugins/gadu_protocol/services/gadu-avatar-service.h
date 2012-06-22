/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_AVATAR_SERVICE_H
#define GADU_AVATAR_SERVICE_H

#include "protocols/services/avatar-service.h"

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduAvatarService
 * @short Service for feteching and uploading avatars for Gadu-Gadu protocol.
 * @see AvatarService
 * @author Rafał 'Vogel' Malinowski
 *
 * See documentation of AvatarService to get general information about this service.
 *
 * GaduAvatarService does not require any protocol data or session as uses HTTP communication channel instead
 * of GaduGadu one. Fetching avatars is possible without any authorization. Uploading avatar is done with
 * OAuth authorization on http://avatars.nowe.gg/upload webservice.
 */
class GaduAvatarService : public AvatarService
{
	Q_OBJECT

public:
	/**
	 * @short Create service instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent of service
	 */
	explicit GaduAvatarService(QObject *parent = 0);
	virtual ~GaduAvatarService();

	virtual void fetchAvatar(const QString &id, QObject *receiver);
	virtual void uploadAvatar(const QString &id, const QString &password, QImage avatar);

};

/**
 * @}
 */

#endif // GADU_AVATAR_SERVICE_H
