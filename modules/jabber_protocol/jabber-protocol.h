#ifndef JABBER_PROTOCOL_H
#define JABBER_PROTOCOL_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QLineEdit>
#include <QList>
#include <QEvent>

#include "accounts/account.h"
#include "conference/conference.h"
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "client/jabber-client.h"
#include "jabber-contact-details.h"
#include "resource/jabber-resource-pool.h"
#include "services/jabber-avatar-service.h"
#include "services/jabber-chat-service.h"
#include "services/jabber-chat-state-service.h"
#include "services/jabber-file-transfer-service.h"

class ActionDescription;
class JabberResourcePool;
class Conference;

class JabberProtocol : public Protocol
{
	Q_OBJECT

		JabberChatService *CurrentChatService;
		JabberChatStateService *CurrentChatStateService;
		JabberFileTransferService *CurrentFileTransferService;
		JabberAvatarService *CurrentAvatarService;

		XMPP::JabberClient *JabberClient;
		XMPP::Jid jabberID;
		JabberResourcePool *ResourcePool;
		/* Initial presence to set after connecting. */
//		XMPP::Status InitialPresence;
		//JabberConfig *jconf;

		bool rosterRequestDone;
		bool usingSSL;
		bool confUseSSL;
		bool doReconnect;
		bool doAboutRoster;
		bool whileConnecting;

		void initializeJabberClient();

	public:
		static int initModule();
		static void closeModule();
		JabberProtocol(Account account, ProtocolFactory *factory);
		~JabberProtocol();
		XMPP::JabberClient * client() { return JabberClient; }
		bool validateUserID(const QString& uid);
		bool isConnecting() { return whileConnecting; }
		XMPP::Status toXMPPStatus(Status status);
		Status toStatus(XMPP::Status status);

		virtual Conference * loadConferenceFromStorage(StoragePoint *storage) { return 0; }

		virtual QPixmap statusPixmap(Status status);
		virtual QPixmap statusPixmap(const QString &statusType);
		virtual void changeStatus(Status status);

		virtual AvatarService *avatarService() { return CurrentAvatarService; }
		virtual ChatService * chatService() { return CurrentChatService; }
		virtual ChatImageService * chatImageService() { return 0; }
		virtual ContactListService * contactListService() { return 0; }
		virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
		virtual PersonalInfoService * personalInfoService() { return 0; }
		virtual SearchService * searchService() { return 0; }
		JabberResourcePool *resourcePool();

		JabberContactDetails * jabberContactDetails(Contact contact) const;

	protected:
		virtual void changeStatus();
		virtual void changePrivateMode();

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
		void slotSubscription(const XMPP::Jid &jid, const QString &type);
		void slotClientDebugMessage (const QString &msg);
		
		void contactDetached(Contact contact);
		void contactAttached(Contact contact);
		
		void buddyUpdated(Buddy &buddy);
		void contactUpdated(Contact &contact);

		void contactIdChanged(Contact contact, const QString &oldId);
		void authorizeContact(Contact contact, bool authorized);

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
