/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <xmpp_client.h>

#include "server/jabber-avatar-pep-fetcher.h"
#include "server/jabber-avatar-vcard-fetcher.h"
#include "services/jabber-pep-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-fetcher.h"

JabberAvatarFetcher::JabberAvatarFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

JabberAvatarFetcher::~JabberAvatarFetcher()
{
}

void JabberAvatarFetcher::failed()
{
	emit avatarFetched(false, QPixmap(), MyContact);
	deleteLater();
}

void JabberAvatarFetcher::fetchAvatarPEP()
{
	XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!protocol || !protocol->pepService())
	{
		failed();
		return;
	}

	JabberAvatarPepFetcher *pepFetcher = new JabberAvatarPepFetcher(MyContact, protocol->pepService(), this);
	connect(pepFetcher, SIGNAL(avatarFetched(bool,QPixmap,Contact)), this, SLOT(pepAvatarFetched(bool,QPixmap,Contact)));
	pepFetcher->fetchAvatar();
}

void JabberAvatarFetcher::fetchAvatarVCard()
{
	XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!protocol || !protocol->vcardService())
	{
		failed();
		return;
	}

	JabberAvatarVCardFetcher *vcardFetcher = new JabberAvatarVCardFetcher(MyContact, protocol->vcardService(), this);
	connect(vcardFetcher, SIGNAL(avatarFetched(bool,QPixmap,Contact)), this, SLOT(avatarFetchedSlot(bool,QPixmap,Contact)));
	vcardFetcher->fetchAvatar();
}

void JabberAvatarFetcher::pepAvatarFetched(bool ok, QPixmap avatar, Contact contact)
{
	if (ok)
	{
		emit avatarFetched(ok, avatar, contact);
		deleteLater();
		return;
	}

	// do a fallback to vcard
	fetchAvatarVCard();
}

void JabberAvatarFetcher::avatarFetchedSlot(bool ok, QPixmap avatar, Contact contact)
{
	emit avatarFetched(ok, avatar, contact);
	deleteLater();
}

void JabberAvatarFetcher::fetchAvatar()
{
	XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!protocol || !protocol->xmppClient() || !protocol->xmppClient()->rootTask())
	{
		failed();
		return;
	}

	if (protocol->pepService()->enabled())
		fetchAvatarPEP();
	else
		fetchAvatarVCard();
}
