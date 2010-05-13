/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "services/jabber-avatar-service.h"
#include "services/jabber-chat-service.h"
#include "services/jabber-chat-state-service.h"
#include "services/jabber-file-transfer-service.h"

class Conference;
class JabberContactDetails;
class JabberResourcePool;
class JabberSubscriptionService;
class PEPManager;
class ServerInfoManager;

class JabberProtocol : public Protocol
{
	Q_OBJECT

	JabberAvatarService *CurrentAvatarService;
	JabberChatService *CurrentChatService;
	JabberChatStateService *CurrentChatStateService;
	JabberFileTransferService *CurrentFileTransferService;
	JabberSubscriptionService *CurrentSubscriptionService;

	XMPP::JabberClient *JabberClient;
	XMPP::Jid jabberID;
	JabberResourcePool *ResourcePool;
	ServerInfoManager *serverInfoManager;
	PEPManager *PepManager;
	QList<Contact> ContactsForDelete;
		
	bool rosterRequestDone;
	bool usingSSL;
	bool confUseSSL;
	bool doReconnect;
	bool doAboutRoster;
	bool pepAvailable;

	static bool ModuleUnloading;

	void initializeJabberClient();
	void setPEPAvailable(bool b);
	void connectContactManagerSignals();
	void disconnectContactManagerSignals();

private slots:
	void login(const QString &password, bool permanent);
	void connectedToServer();
	void disconnectedFromServer();
	void disconnectFromServer(const XMPP::Status &s = XMPP::Status ("", "", 0, false));
	void rosterRequestFinished(bool success);
	void clientResourceReceived(const XMPP::Jid &j, const XMPP::Resource &r);
	void slotContactUpdated(const XMPP::RosterItem &ri);
	void slotContactDeleted(const XMPP::RosterItem &ri);
	void slotIncomingFileTransfer();
	void slotClientDebugMessage (const QString &msg);

	void contactDetached(Contact contact);
	void contactAttached(Contact contact);
	void contactUpdated(Contact contact);

	void buddyUpdated(Buddy &buddy);

	void contactIdChanged(Contact contact, const QString &oldId);
	void authorizeContact(Contact contact, bool authorized);

	void serverFeaturesChanged();
	void itemPublished(const XMPP::Jid& j, const QString& n, const XMPP::PubSubItem& item);
	void itemRetracted(const XMPP::Jid& j, const QString& n, const XMPP::PubSubRetraction& item);

protected:
	virtual void changeStatus();
	virtual void changePrivateMode();

public:
	static int initModule();
	static void closeModule();

	JabberProtocol(Account account, ProtocolFactory *factory);
	virtual ~JabberProtocol();

	XMPP::JabberClient * client() { return JabberClient; }
	bool validateUserID(const QString& uid);
	bool isPEPAvailable() { return pepAvailable; }

	virtual Conference * loadConferenceFromStorage(StoragePoint *storage) { return 0; }

	virtual QPixmap statusPixmap(Status status);
	virtual QPixmap statusPixmap(const QString &statusType);
	
	virtual QIcon statusIcon(Status status);
	virtual QIcon statusIcon(const QString &statusType);
	
	virtual void changeStatus(Status status);

	virtual AvatarService * avatarService() { return CurrentAvatarService; }
	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ChatImageService * chatImageService() { return 0; }
	virtual ContactListService * contactListService() { return 0; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return 0; }
	virtual SearchService * searchService() { return 0; }
	JabberResourcePool *resourcePool();
	PEPManager *pepManager() { return PepManager; }

	JabberContactDetails * jabberContactDetails(Contact contact) const;

public slots:
	void connectToServer();
	void login();
	void logout();

signals:
	void userStatusChangeIgnored(Buddy);
	void sendMessageFiltering(Chat chat, QByteArray &msg, bool &stop);
	void messageStatusChanged(int messsageId, ChatService::MessageStatus status);
	void receivedMessageFilter(Chat chat, Buddy sender, const QString &message, time_t time, bool &ignore);

};

#endif //JABBER_PROTOCOL_H
