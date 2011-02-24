/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "protocols/services/chat-service.h"
#include "protocols/protocol.h"

#include "client/jabber-client.h"
#include "jabber-account-details.h"
#include "services/jabber-avatar-service.h"
#include "services/jabber-chat-service.h"
#include "services/jabber-chat-state-service.h"
#include "services/jabber-contact-personal-info-service.h"
#include "services/jabber-personal-info-service.h"
#include "services/jabber-roster-service.h"

class JabberContactDetails;
class JabberSubscriptionService;

class JabberProtocol : public Protocol
{
	Q_OBJECT

	JabberAvatarService *CurrentAvatarService;
	JabberChatService *CurrentChatService;
	JabberChatStateService *CurrentChatStateService;
	JabberContactPersonalInfoService *CurrentContactPersonalInfoService;
	JabberPersonalInfoService *CurrentPersonalInfoService;
	JabberRosterService *CurrentRosterService;
	JabberSubscriptionService *CurrentSubscriptionService;

	XMPP::JabberClient *JabberClient;
	XMPP::Jid jabberID;

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

private slots:
	void connectedToServer();
	void disconnectedFromServer();
	void disconnectFromServer(const XMPP::Status &s = XMPP::Status (QString(), QString(), 0, false));
	void rosterDownloaded(bool success);
	void clientResourceReceived(const XMPP::Jid &j, const XMPP::Resource &r);
	void slotClientDebugMessage (const QString &msg);

	void contactDetached(Contact contact);
	void contactAttached(Contact contact);
	void contactUpdated(Contact contact);

	void buddyUpdated(Buddy &buddy);

	void contactIdChanged(Contact contact, const QString &oldId);

	void connectionErrorSlot(const QString &message);
	void invalidPasswordSlot();
	void changeStatus(bool force);

protected:
	virtual void changeStatus();
	virtual void changePrivateMode();

public:
	static int initModule();
	static void closeModule();

	JabberProtocol(Account account, ProtocolFactory *factory);
	virtual ~JabberProtocol();

	XMPP::JabberClient * client() { return JabberClient; }

	void setContactsListReadOnly(bool contactsListReadOnly);
	virtual bool contactsListReadOnly();

	virtual QString statusPixmapPath();

	virtual AvatarService * avatarService() { return CurrentAvatarService; }
	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ChatStateService *chatStateService() { return CurrentChatStateService; }
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return CurrentContactPersonalInfoService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual RosterService * rosterService() { return CurrentRosterService; }
	JabberSubscriptionService * subscriptionService() { return CurrentSubscriptionService; }

	JabberContactDetails * jabberContactDetails(Contact contact) const;

public slots:
	void connectToServer();
	void login();
	virtual void login(const QString &password, bool permanent);
	void logout();

signals:
	void userStatusChangeIgnored(Buddy);

};

#endif //JABBER_PROTOCOL_H
