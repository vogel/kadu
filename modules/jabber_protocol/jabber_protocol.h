#ifndef JABBER_PROTOCOL_H
#define JABBER_PROTOCOL_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QLineEdit>
#include <QList>
#include <QEvent>

#include "accounts/account.h"
#include "conference/conference.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "services/jabber-chat-service.h"

#include "jabber-client.h"
#include "jabber-contact-account-data.h"
#include "jabber-resource-pool.h"

class ActionDescription;
class JabberResourcePool;
class Conference;

class JabberProtocol : public Protocol
{
	Q_OBJECT

	public:
		static int initModule();
		static void closeModule();
		JabberProtocol(Account *account, ProtocolFactory *factory);
		~JabberProtocol();
		XMPP::JabberClient * client() { return JabberClient; }
		bool validateUserID(QString& uid);
		bool isConnecting() { return whileConnecting/*(State == NetworkConnecting)*/; }

		virtual Conference * loadConferenceFromStorage(StoragePoint *storage) { return 0; }

		virtual QPixmap statusPixmap(Status status);
		virtual void changeStatus(Status status);

		virtual ChatService * chatService() { return CurrentChatService; }
		JabberResourcePool *resourcePool();
		ActionDescription* loginJabberActionDescription;
		ActionDescription* logoutJabberActionDescription;

	protected:
		virtual void changeStatus();
		virtual void changePrivateMode();

	private:
		JabberChatService *CurrentChatService;
		void initializeJabberClient();


		XMPP::JabberClient *JabberClient;
		XMPP::Jid jabberID;
		JabberResourcePool *ResourcePool;
		/* Initial presence to set after connecting. */
		XMPP::Status InitialPresence;
		//JabberConfig *jconf;

		bool rosterRequestDone;
		bool usingSSL;
		bool confUseSSL;
		bool doReconnect;
		bool doAboutRoster;
		bool whileConnecting;

	private slots:
		void connectToServer();
		void connectedToServer();
		void disconnectedFromServer();
		void disconnect(const XMPP::Status &s = XMPP::Status ("", "", 0, false));
		void rosterRequestFinished(bool success);
		void clientResourceAvailable(const XMPP::Jid &j, const XMPP::Resource &r);
		void clientResourceUnavailable(const XMPP::Jid &j, const XMPP::Resource &r);
		void slotContactUpdated ( const XMPP::RosterItem & ri);
		void slotContactDeleted ( const XMPP::RosterItem & ri);
		void slotSubscription(const XMPP::Jid & jid, const QString & type);
		//void client_rosterItemUpdated(const XMPP::RosterItem &r);
		void slotHandleTLSWarning(QCA::TLS::IdentityResult identityResult, QCA::Validity validityResult);
		bool handleTLSWarning(XMPP::JabberClient *jabberClient, QCA::TLS::IdentityResult identityResult, QCA::Validity validityResult );
		//void slotCSError ( int error ); // why this was removed?
		void slotClientDebugMessage ( const QString &msg );
		void changeSubscription(const XMPP::Jid &jid, const QString type);
		void requestSubscription(const XMPP::Jid &jid);
		void resendSubscription(const XMPP::Jid &jid);
		void rejectSubscription(const XMPP::Jid &jid);

	public slots:
		void loginAction(QAction *sender, bool toggled);
		void logoutAction(QAction *sender, bool toggled);
		void setPresence(const XMPP::Status &status);
		void login();
		void logout(const XMPP::Status &s = XMPP::Status("", tr("Logged out"), 0, false));

	signals:
		void userStatusChangeIgnored(Contact);
		void sendMessageFiltering(Chat *chat, QByteArray &msg, bool &stop);
		void messageStatusChanged(int messsageId, ChatService::MessageStatus status);
		void receivedMessageFilter(Chat *chat, Contact sender, const QString &message, time_t time, bool &ignore);
};

#endif

