/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_AVATAR_PEP_FETCHER_H
#define JABBER_AVATAR_PEP_FETCHER_H

#include "contacts/contact.h"

namespace XMPP
{
	class Jid;
	class JT_DiscoItems;
	class PubSubItem;
}

class JabberAvatarPepFetcher : public QObject
{
	Q_OBJECT

	Contact MyContact;
	XMPP::JT_DiscoItems *DiscoItems;
	QString AvatarId;

	void done();
	void failed();

private slots:
	void discoItemsFinished();
	void avatarMetadataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item);
	void avatarDataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item);

public:
	explicit JabberAvatarPepFetcher(Contact contact, QObject *parent = 0);
	virtual ~JabberAvatarPepFetcher();

	void fetchAvatar();

signals:
	void avatarFetched(Contact contact, bool ok);

};

#endif // JABBER_AVATAR_PEP_FETCHER_H
