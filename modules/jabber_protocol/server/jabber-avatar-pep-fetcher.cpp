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

#include "utils/pep-manager.h"
#include "base64.h"
#include "jabber-protocol.h"

#include "jabber-avatar-pep-fetcher.h"

#define XMLNS_AVATAR_DATA "urn:xmpp:avatar:data"
#define XMLNS_AVATAR_METADATA "urn:xmpp:avatar:metadata"
#define NS_AVATAR_DATA "http://www.xmpp.org/extensions/xep-0084.html#ns-data"

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
	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!jabberProtocol || !jabberProtocol->isConnected() || !jabberProtocol->client() || !jabberProtocol->client()->rootTask() ||
		!jabberProtocol->client()->isPEPAvailable() || !jabberProtocol->client()->pepManager()
	)
	{
		failed();
		deleteLater();
		return;
	}

	DiscoItems = new XMPP::JT_DiscoItems(jabberProtocol->client()->rootTask());
	connect(DiscoItems, SIGNAL(finished()), this, SLOT(discoItemsFinished()));
	DiscoItems->get(MyContact.id());
	DiscoItems->go();
}

void JabberAvatarPepFetcher::discoItemsFinished()
{
	XMPP::DiscoList result = DiscoItems->items();

	bool hasAvatar = false;
	foreach (const XMPP::DiscoItem &item, result)
		if (item.node() == XMLNS_AVATAR_DATA || item.node() == NS_AVATAR_DATA)
		{
			hasAvatar = true;
			break;
		}

	if (!hasAvatar)
	{
		failed();
		deleteLater();
	}

	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());

	connect(jabberProtocol->client()->pepManager(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarMetadataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
	jabberProtocol->client()->pepManager()->get(MyContact.id(), XMLNS_AVATAR_METADATA, "");
}

void JabberAvatarPepFetcher::avatarMetadataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item)
{
	if (jid.bare() != MyContact.id() || node != XMLNS_AVATAR_METADATA)
		return; // not our data :(

	AvatarId = item.id();
	if (AvatarId.isEmpty())
	{
		failed();
		deleteLater();
		return;
	}

	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	disconnect(jabberProtocol->client()->pepManager(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarMetadataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
	connect(jabberProtocol->client()->pepManager(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarDataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
	jabberProtocol->client()->pepManager()->get(MyContact.id(), XMLNS_AVATAR_DATA, item.id());
}

void JabberAvatarPepFetcher::avatarDataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item)
{
	if (jid.bare() != MyContact.id() || node != XMLNS_AVATAR_DATA || item.id() != AvatarId)
		return; // not our data :(

			JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	disconnect(jabberProtocol->client()->pepManager(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarDataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));

	XMPP::Base64 base64;
	QByteArray imageData = base64.decode(item.payload().text());

	if (!imageData.isEmpty())
	{
		Avatar contactAvatar = AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd);
		contactAvatar.setLastUpdated(QDateTime::currentDateTime());
		contactAvatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));

		QPixmap pixmap;
		pixmap.loadFromData(imageData);

		contactAvatar.setPixmap(pixmap);
		done();
	}
	else
		failed();

	deleteLater();
}
