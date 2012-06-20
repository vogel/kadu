/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef JABBER_AVATAR_SERVICE_H
#define JABBER_AVATAR_SERVICE_H

#include "contacts/contact.h"
#include "protocols/services/avatar-service.h"

namespace XMPP { class JabberVCardService; }

class JabberPepService;

class JabberAvatarService : public AvatarService
{
	Q_OBJECT

	QWeakPointer<JabberPepService> PepService;
	QWeakPointer<XMPP::JabberVCardService> VCardService;

public:
	explicit JabberAvatarService(Account account, QObject *parent = 0);
	virtual ~JabberAvatarService();

	void setPepService(JabberPepService *pepService);
	void setVCardService(XMPP::JabberVCardService *vCardService);

	virtual void fetchAvatar(Contact contact, QObject *receiver);
	virtual void uploadAvatar(QImage avatar);

};

#endif // JABBER_AVATAR_SERVICE_H
