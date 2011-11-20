/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"

#include "client/jabber-client.h"
#include "services/jabber-avatar-service.h"
#include "services/jabber-chat-service.h"
#include "services/jabber-chat-state-service.h"
#include "services/jabber-contact-personal-info-service.h"
#include "services/jabber-file-transfer-service.h"
#include "services/jabber-personal-info-service.h"
#include "services/jabber-roster-service.h"
#include "jabber-account-details.h"

class JabberContactDetails;
class JabberResourcePool;
class JabberRosterService;
class JabberSubscriptionService;

class JabberProtocol : public Protocol
{
	Q_OBJECT

	JabberAvatarService *CurrentAvatarService;
	JabberChatService *CurrentChatService;
	JabberChatStateService *CurrentChatStateService;
	JabberContactPersonalInfoService *CurrentContactPersonalInfoService;
	JabberFileTransferService *CurrentFileTransferService;
	JabberPersonalInfoService *CurrentPersonalInfoService;
	JabberRosterService *CurrentRosterService;
	JabberSubscriptionService *CurrentSubscriptionService;

	friend class XMPP::JabberClient;
	XMPP::JabberClient *JabberClient;
	XMPP::Jid jabberID;
	JabberResourcePool *ResourcePool;

	bool usingSSL;
	bool confUseSSL;
	bool doReconnect;
	bool doAboutRoster;

	bool ContactsListReadOnly;

	void initializeJabberClient();

	friend class JabberRosterService;
	void connectContactManagerSignals();
	void disconnectContactManagerSignals();
	XMPP::ClientStream::AllowPlainType plainAuthToXMPP(JabberAccountDetails::AllowPlainType type);

	void notifyAboutPresenceChanged(const XMPP::Jid &jid, const XMPP::Resource &resource);

private slots:
	void connectedToServer();
	void disconnectedFromServer();
	void disconnectFromServer(const XMPP::Status &s = XMPP::Status (QString(), QString(), 0, false));
	void rosterDownloaded(bool success);

	void clientAvailableResourceReceived(const XMPP::Jid &j, const XMPP::Resource &r);
	void clientUnavailableResourceReceived(const XMPP::Jid &j, const XMPP::Resource &r);

	void slotClientDebugMessage (const QString &msg);

	void contactDetached(Contact contact, Buddy previousBuddy, bool reattaching);
	void contactAttached(Contact contact, bool reattached);
	void contactUpdated(Contact contact);

	void buddyUpdated(Buddy &buddy);

	void contactIdChanged(Contact contact, const QString &oldId);

	void connectionErrorSlot(const QString &message);

protected:
	virtual void login();
	virtual void afterLoggedIn();
	virtual void logout();
	virtual void sendStatusToServer();

	virtual void changePrivateMode();

public:
	JabberProtocol(Account account, ProtocolFactory *factory);
	virtual ~JabberProtocol();

	XMPP::JabberClient * client() { return JabberClient; }
	XMPP::Client * xmppClient() { return JabberClient->client(); }

	void setContactsListReadOnly(bool contactsListReadOnly);
	virtual bool contactsListReadOnly() { return ContactsListReadOnly; }

	virtual QString statusPixmapPath();

	virtual AvatarService * avatarService() { return CurrentAvatarService; }
	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ChatStateService *chatStateService() { return CurrentChatStateService; }
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return CurrentContactPersonalInfoService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual RosterService * rosterService() { return CurrentRosterService; }

	JabberSubscriptionService * subscriptionService() { return CurrentSubscriptionService; }

	JabberResourcePool *resourcePool();

	JabberContactDetails * jabberContactDetails(Contact contact) const;

signals:
	void userStatusChangeIgnored(Buddy);

};

#endif //JABBER_PROTOCOL_H
