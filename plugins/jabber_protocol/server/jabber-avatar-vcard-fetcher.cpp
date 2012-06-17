/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <xmpp_vcard.h>

#include "avatars/avatar-manager.h"

#include "services/jabber-vcard-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-vcard-fetcher.h"

JabberAvatarVCardFetcher::JabberAvatarVCardFetcher(Contact contact, XMPP::JabberVCardService *vCardService, QObject *parent) :
		QObject(parent), MyContact(contact), VCardService(vCardService)
{
}

JabberAvatarVCardFetcher::~JabberAvatarVCardFetcher()
{
}

void JabberAvatarVCardFetcher::done()
{
	emit avatarFetched(MyContact, true);
	deleteLater();
}

void JabberAvatarVCardFetcher::failed()
{
	emit avatarFetched(MyContact, false);
	deleteLater();
}

void JabberAvatarVCardFetcher::fetchAvatar()
{
	XMPP::JabberProtocol *jabberProtocol = qobject_cast<XMPP::JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!jabberProtocol || !jabberProtocol->isConnected())
	{
		failed();
		return;
	}

	if (VCardService)
		VCardService.data()->fetch(MyContact.id(), this);
	else
		failed();
}

void JabberAvatarVCardFetcher::vCardFetched(bool ok, const XMPP::VCard &vCard)
{
	if (!ok)
	{
		failed();
		return;
	}

	Avatar contactAvatar = AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd);
	contactAvatar.setLastUpdated(QDateTime::currentDateTime());
	contactAvatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));

	QPixmap pixmap;
	pixmap.loadFromData(vCard.photo());

	contactAvatar.setPixmap(pixmap);
	done();
}
