#ifndef TLEN_PROTOCOL_H
#define TLEN_PROTOCOL_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QList>
#include <QEvent>
#include <qpointer.h>

#include "chat/chat.h"
#include "conference/conference.h"
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "services/tlen-avatar-service.h"
#include "services/tlen-chat-service.h"

class ActionDescription;
class Conference;

class tlen;

class TlenProtocol : public Protocol
{
	Q_OBJECT

	public:
		static int initModule();
		static void closeModule();
		TlenProtocol(Account account, ProtocolFactory *factory);
		~TlenProtocol();

		bool validateUserID(QString& uid);

		tlen * client() { return TlenClient; }

		virtual Conference * loadConferenceFromStorage(StoragePoint *storage) { return 0; }

		virtual QPixmap statusPixmap(Status status);
		virtual QPixmap statusPixmap(const QString &statusType);
		virtual void changeStatus(Status status);

		virtual AvatarService *avatarService() { return CurrentAvatarService; }
		virtual ChatService * chatService() { return CurrentChatService; }

	protected:
		virtual void changeStatus();
		virtual void changePrivateMode();

	private:
		tlen *TlenClient;
		bool rosterRequestDone;
		bool usingSSL;
		bool confUseSSL;
		bool doReconnect;
		bool doAboutRoster;
		bool whileConnecting;
		bool active;
		QMap<QString, QString> TypingUsers;
		QString localHostName();

		TlenAvatarService *CurrentAvatarService;
		TlenChatService *CurrentChatService;
		friend class TlenChatService;

	private slots:
		void connectToServer();
		void login();
		void logout();
		//void connectedToServer();
		//void disconnectedFromServer();

		void presenceDisconnected();
		void itemReceived(QString jid, QString name, QString subscription, QString group, bool sort);
		void presenceChanged(QString from, QString status, QString description);
		void authorizationAsk(QString);
		void removeItem(QString);

		void sortRoster();

		void chatMsgReceived(QDomNode);

		void clearRosterView();
		void tlenLoggedIn();
		void tlenStatusChanged(); // FOR GUI TO UPDATE ICONS
		void tlenStatusUpdate();	// FOR TLEN TO WRITE STATUS
		void eventReceived(QDomNode);

		void chatNotify(QString,QString);

		void fetchAvatars(QString jid, QString type, QString md5);

	public slots:
		bool sendMessage(Chat *chat, FormattedMessage &message);

	signals:
		void authorize(QString,bool);

		void userStatusChangeIgnored(Buddy);
                void sendMessageFiltering(Chat *chat, QByteArray &msg, bool &stop);
        	void messageStatusChanged(int messsageId, ChatService::MessageStatus status);
		void receivedMessageFilter(Chat *chat, Buddy sender, const QString &message, time_t time, bool &ignore);
		void messageReceived(const Message &message);
		void messageSent(const Message &message);
};

#endif // TLEN_PROTOCOL_H
