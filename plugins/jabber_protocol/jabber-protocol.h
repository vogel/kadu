/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_PROTOCOL_H
#define JABBER_PROTOCOL_H

#include <qxmpp/QXmppClient.h>

#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"

#include "services/jabber-avatar-service.h"
#include "services/jabber-contact-personal-info-service.h"
#include "services/jabber-file-transfer-service.h"
#include "services/jabber-personal-info-service.h"
#include "services/jabber-subscription-service.h"
#include "jabber-account-details.h"

class JabberClientInfoService;
class JabberContactDetails;
class JabberPepService;
class JabberResourcePool;
class JabberServerInfoService;
class JabberStreamDebugService;

class QXmppClient;

class JabberProtocol : public Protocol
{
	Q_OBJECT

	JabberAvatarService *CurrentAvatarService;
	JabberContactPersonalInfoService *CurrentContactPersonalInfoService;
	JabberFileTransferService *CurrentFileTransferService;
	JabberPersonalInfoService *CurrentPersonalInfoService;
	JabberSubscriptionService *CurrentSubscriptionService;
	JabberClientInfoService *CurrentClientInfoService;
	JabberServerInfoService *CurrentServerInfoService;
	JabberPepService *CurrentPepService;
	JabberStreamDebugService *CurrentStreamDebugService;
	JabberVCardService *CurrentVCardService;

	JabberResourcePool *ResourcePool;

	QXmppClient *m_client;

	bool ContactsListReadOnly;

	// void notifyAboutPresenceChanged(const Jid &jid, const Resource &resource);

private slots:
	void connectedToServer();
	void disconenctedFromServer();
	void error(QXmppClient::Error error);

	void rosterReady(bool success);

	void presenceReceived(const QXmppPresence &presence);

	// void clientAvailableResourceReceived(const Jid &j, const Resource &r);
	// void clientUnavailableResourceReceived(const Jid &j, const Resource &r);

	void serverInfoUpdated();

protected:
	virtual void login();
	virtual void afterLoggedIn();
	virtual void logout();
	virtual void sendStatusToServer();

	virtual void changePrivateMode();

public:
	JabberProtocol(Account account, ProtocolFactory *factory);
	virtual ~JabberProtocol();

	void setContactsListReadOnly(bool contactsListReadOnly);
	virtual bool contactsListReadOnly() { return ContactsListReadOnly; }

	virtual QString statusPixmapPath();

	virtual AvatarService * avatarService() { return CurrentAvatarService; }
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return CurrentContactPersonalInfoService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual SubscriptionService * subscriptionService() { return CurrentSubscriptionService; }
	// virtual JabberClientInfoService * clientInfoService() { return CurrentClientInfoService; }
	// virtual JabberServerInfoService * serverInfoService() { return CurrentServerInfoService; }
	// virtual JabberPepService * pepService() { return CurrentPepService; }
	// virtual JabberStreamDebugService * streamDebugService() { return CurrentStreamDebugService; }
	// virtual JabberVCardService * vcardService() { return CurrentVCardService; }

	JabberResourcePool *resourcePool();

	JabberContactDetails * jabberContactDetails(Contact contact) const;

signals:
	void userStatusChangeIgnored(Buddy);

};

#endif //JABBER_PROTOCOL_H
