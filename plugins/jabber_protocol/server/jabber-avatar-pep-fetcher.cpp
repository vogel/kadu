/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "iris/xmpp_tasks.h"
#include "services/jabber-pep-service.h"
#include "base64.h"

#include "jabber-avatar-pep-fetcher.h"

#define XMLNS_AVATAR_DATA "urn:xmpp:avatar:data"
#define XMLNS_AVATAR_METADATA "urn:xmpp:avatar:metadata"
#define NS_AVATAR_DATA "http://www.xmpp.org/extensions/xep-0084.html#ns-data"

JabberAvatarPepFetcher::JabberAvatarPepFetcher(const QString &id, JabberPepService *pepService, QObject *parent) :
		QObject(parent), PepService(pepService), Id(id)
{
}

JabberAvatarPepFetcher::~JabberAvatarPepFetcher()
{
}

void JabberAvatarPepFetcher::done(QPixmap avatar)
{
	emit avatarFetched(true, avatar);
	deleteLater();
}

void JabberAvatarPepFetcher::failed()
{
	emit avatarFetched(false, QPixmap());
	deleteLater();
}

void JabberAvatarPepFetcher::fetchAvatar()
{
	if (!PepService || !PepService.data()->xmppClient() || !PepService.data()->enabled())
	{
		failed();
		return;
	}

	XMPP::JT_DiscoItems *discoItems = new XMPP::JT_DiscoItems(PepService.data()->xmppClient()->rootTask());
	connect(discoItems, SIGNAL(finished()), this, SLOT(discoItemsFinished()));

	discoItems->get(Id);
	discoItems->go(true);
}

void JabberAvatarPepFetcher::discoItemsFinished()
{
	if (!PepService || !PepService.data()->xmppClient() || !PepService.data()->enabled())
	{
		failed();
		return;
	}

	XMPP::JT_DiscoItems *discoItems = static_cast<XMPP::JT_DiscoItems *>(sender());
	XMPP::DiscoList result = discoItems->items();

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
		return;
	}

	connect(PepService.data(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarMetadataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
	PepService.data()->get(Id, XMLNS_AVATAR_METADATA, "");
}

void JabberAvatarPepFetcher::avatarMetadataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item)
{
	if (jid.bare() != Id || node != XMLNS_AVATAR_METADATA)
		return; // not our data :(

	AvatarId = item.id();
	if (AvatarId == "current") // removed
	{
		done(QPixmap());
		return;
	}

	if (!PepService || !PepService.data()->xmppClient() || !PepService.data()->enabled())
	{
		failed();
		return;
	}

	disconnect(PepService.data(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarMetadataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
	connect(PepService.data(), SIGNAL(itemPublished(XMPP::Jid,QString,XMPP::PubSubItem)), this, SLOT(avatarDataQueryFinished(XMPP::Jid,QString,XMPP::PubSubItem)));
	PepService.data()->get(Id, XMLNS_AVATAR_DATA, AvatarId);
}

void JabberAvatarPepFetcher::avatarDataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item)
{
	if (jid.bare() != Id || node != XMLNS_AVATAR_DATA || item.id() != AvatarId)
		return; // not our data :(

	QByteArray imageData = XMPP::Base64::decode(item.payload().text());

	QPixmap pixmap;

	if (!imageData.isEmpty())
		pixmap.loadFromData(imageData);

	done(pixmap);
}
