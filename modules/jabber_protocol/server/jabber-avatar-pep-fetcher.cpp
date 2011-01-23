/*
 * %kadu copyright begin%
 * %kadu copyright end%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include "avatars/avatar-manager.h"

#include "jabber-protocol.h"

#include "jabber-avatar-pep-fetcher.h"

JabberAvatarPepFetcher::JabberAvatarPepFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

JabberAvatarPepFetcher::~JabberAvatarPepFetcher()
{
}

void JabberAvatarPepFetcher::done()
{
	emit avatarFetched(MyContact, true);
}

void JabberAvatarPepFetcher::failed()
{
	emit avatarFetched(MyContact, false);
}

void JabberAvatarPepFetcher::fetchAvatar()
{
// 	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
// 	if (!jabberProtocol || !jabberProtocol->isConnected())
// 	{
		failed();
		deleteLater();
		return;
// 	}
}
