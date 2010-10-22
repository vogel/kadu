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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtXml/QDomDocument>

#include "accounts/account.h"
#include "buddies/avatar.h"
#include "buddies/avatar-manager.h"
#include "misc/path-conversion.h"

#include "client/jabber-client.h"
#include "jabber-protocol.h"
#include "utils/vcard-factory.h"

#include "jabber-avatar-fetcher.h"

JabberAvatarFetcher::JabberAvatarFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

void JabberAvatarFetcher::done(const QByteArray &avatar)
{
	emit avatarFetched(MyContact, true, avatar);
}

void JabberAvatarFetcher::failed()
{
	emit avatarFetched(MyContact, false, QByteArray());
}

void JabberAvatarFetcher::fetchAvatar()
{
	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!jabberProtocol || !jabberProtocol->isConnected())
	{
		failed();
		return;
	}

	VCardFactory::instance()->getVCard(MyContact.id(), jabberProtocol->client()->rootTask(), this, SLOT(receivedVCard()));
}

void JabberAvatarFetcher::receivedVCard()
{
	const XMPP::VCard *vcard = VCardFactory::instance()->vcard(MyContact.id());

	if (vcard && !vcard->photo().isEmpty())
	{
		if (MyContact.contactAvatar().isNull())
			MyContact.setContactAvatar(Avatar::create());

		MyContact.contactAvatar().setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
		done(vcard->photo());
	}
	else
		failed();

	deleteLater();
}
