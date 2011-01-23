/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
	JabberAvatarPepFetcher *pepUploader = new JabberAvatarPepFetcher(MyContact, this);
	connect(pepUploader, SIGNAL(avatarFetched(Contact,bool)), this, SLOT(pepAvatarFetched(Contact,bool)));
	pepUploader->fetchAvatar();
}

void JabberAvatarFetcher::fetchAvatarVCard()
{
	JabberAvatarVCardFetcher *vcardUploader = new JabberAvatarVCardFetcher(MyContact, this);
	connect(vcardUploader, SIGNAL(avatarFetched(Contact,bool)), this, SLOT(avatarFetchedSlot(Contact,bool)));
	vcardUploader->fetchAvatar();
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

void JabberAvatarFetcher::avatarFechedSlot(Contact contact, bool ok)
{
	emit avatarFetched(contact, ok);
	deleteLater();
}

void JabberAvatarFetcher::fetchAvatar()
{
	JabberProtocol *protocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
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
