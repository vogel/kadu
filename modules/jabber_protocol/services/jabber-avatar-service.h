/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_AVATAR_SERVICE_H
#define JABBER_AVATAR_SERVICE_H

#include "xmpp_vcard.h"
#include "xmpp_tasks.h"

#include "contacts/contact.h"
#include "protocols/services/avatar-service.h"

class JabberProtocol;

class JabberAvatarService : public AvatarService
{
	Q_OBJECT
	Contact MyContact;
	Account MyAccount;
	QImage AccountAvatar;
	XMPP::JT_VCard *VCardHandler;
	QByteArray selfAvatarData_;
	QString selfAvatarHash_;
	JabberProtocol *Protocol;
	
	QByteArray scaleAvatar(const QByteArray& b);
	void uploadAvatarPEP();
	void uploadAvatarVCard();

public:
	explicit JabberAvatarService(Account account, QObject *parent = 0) : AvatarService(account, parent), MyAccount(account) {}
	virtual ~JabberAvatarService() {}

	virtual void fetchAvatar(Contact contact);
	virtual void uploadAvatar(QImage avatar);
	
private slots:
	void itemPublished(const XMPP::Jid& jid, const QString& n, const XMPP::PubSubItem& item);
	void publish_success(const QString& n, const XMPP::PubSubItem& item);
	void publish_error(const QString& n, const XMPP::PubSubItem& item);
	void fetchingVCardFinished();
	void uploadingVCardFinished();
};

#endif // JABBER_AVATAR_SERVICE_H
