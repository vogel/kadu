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

#include "utils/vcard-factory.h"
#include "jabber-protocol.h"

#include "jabber-avatar-vcard-fetcher.h"

JabberAvatarVCardFetcher::JabberAvatarVCardFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

JabberAvatarVCardFetcher::~JabberAvatarVCardFetcher()
{
}

void JabberAvatarVCardFetcher::done()
{
	emit avatarFetched(MyContact, true);
}

void JabberAvatarVCardFetcher::failed()
{
	emit avatarFetched(MyContact, false);
}

void JabberAvatarVCardFetcher::fetchAvatar()
{
	JabberProtocol *jabberProtocol = qobject_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!jabberProtocol || !jabberProtocol->isConnected())
	{
		failed();
		deleteLater();
		return;
	}

	VCardFactory::instance()->getVCard(MyContact.id(), jabberProtocol->client()->rootTask(), this, SLOT(vcardReceived()));
}

void JabberAvatarVCardFetcher::vcardReceived()
{
	const XMPP::VCard *vcard = VCardFactory::instance()->vcard(MyContact.id());

	if (vcard)
	{
		Avatar contactAvatar = AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd);
		contactAvatar.setLastUpdated(QDateTime::currentDateTime());
		contactAvatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));

		QPixmap pixmap;
		pixmap.loadFromData(vcard->photo());

		contactAvatar.setPixmap(pixmap);
		done();
	}
	else
		failed();

	deleteLater();
}
