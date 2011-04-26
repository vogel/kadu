/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "server/jabber-avatar-pep-fetcher.h"
#include "server/jabber-avatar-vcard-fetcher.h"
#include "jabber-protocol.h"

#include "jabber-avatar-fetcher.h"

JabberAvatarFetcher::JabberAvatarFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

void JabberAvatarFetcher::fetchAvatarPEP()
{
	JabberAvatarPepFetcher *pepFetcher = new JabberAvatarPepFetcher(MyContact, this);
	connect(pepFetcher, SIGNAL(avatarFetched(Contact,bool)), this, SLOT(pepAvatarFetched(Contact,bool)));
	pepFetcher->fetchAvatar();
}

void JabberAvatarFetcher::fetchAvatarVCard()
{
	JabberAvatarVCardFetcher *vcardFetcher = new JabberAvatarVCardFetcher(MyContact, this);
	connect(vcardFetcher, SIGNAL(avatarFetched(Contact,bool)), this, SLOT(avatarFetchedSlot(Contact,bool)));
	vcardFetcher->fetchAvatar();
}

void JabberAvatarFetcher::pepAvatarFetched(Contact contact, bool ok)
{
	if (ok)
	{
		emit avatarFetched(contact, ok);
		deleteLater();
		return;
	}

	// do a fallback to vcard
	fetchAvatarVCard();
}

void JabberAvatarFetcher::avatarFetchedSlot(Contact contact, bool ok)
{
	emit avatarFetched(contact, ok);
	deleteLater();
}

void JabberAvatarFetcher::fetchAvatar()
{
	JabberProtocol *protocol = qobject_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!protocol || !protocol->client() || !protocol->client()->rootTask())
	{
		emit avatarFetched(MyContact, false);
		deleteLater();
		return;
	}

	if (protocol->client()->isPEPAvailable() && protocol->client()->pepManager())
		fetchAvatarPEP();
	else
		fetchAvatarVCard();
}
