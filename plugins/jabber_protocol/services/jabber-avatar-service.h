/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_AVATAR_SERVICE_H
#define JABBER_AVATAR_SERVICE_H

#include <QtCore/QPointer>

#include "protocols/services/avatar-service.h"

namespace XMPP { class JabberVCardService; }

class JabberPepService;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarService
 * @short Service for downloading and uploading avatars for XMPP/Jabber protocol.
 * @see AvatarService
 * @author Rafał 'Vogel' Malinowski
 *
 * See documentation of AvatarService to get general information about this service.
 *
 * JabberAvatarService uses JabberPepService or XMPP::JabberVCardService to create AvatarDownloader and
 * AvatarUploader instances.
 *
 * When neither JabberPepService nor XMPP::JabberVCardService are provided this service will return null
 * AvatarDownloader and AvatarUploader instances. If only one service is available then proper downloader and
 * uploader will be returned. In case both are available then returned instance will try to use PEP protocol
 * first, then VCard in case PEP fails.
 *
 * Use setPepService() and/or setVCardService() to set these services.
 */
class JabberAvatarService : public AvatarService
{
	Q_OBJECT

	QPointer<JabberPepService> PepService;
	QPointer<XMPP::JabberVCardService> VCardService;

public:
	/**
	 * @short Create service instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param account account of service
	 * @param parent QObject parent of service
	 */
	explicit JabberAvatarService(Account account, QObject *parent = 0);
	virtual ~JabberAvatarService();

	/**
	 * @short Set PEP service object to use in this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param pepService PEP service object to use
	 */
	void setPepService(JabberPepService *pepService);

	/**
	 * @short Set VCard service object to use in this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param vCardService VCard service object to use
	 */
	void setVCardService(XMPP::JabberVCardService *vCardService);

	virtual AvatarDownloader * createAvatarDownloader();
	virtual AvatarUploader * createAvatarUploader();

};

/**
 * @}
 */

#endif // JABBER_AVATAR_SERVICE_H
