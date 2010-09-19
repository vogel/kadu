#ifndef TLEN_PROTOCOL_H
#define TLEN_PROTOCOL_H

#error Module tlen is not supported. Do not compile it, and if you do, do not post any bug reports!

#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QList>
#include <QEvent>
#include <qpointer.h>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "services/tlen-avatar-service.h"
#include "services/tlen-chat-service.h"
#include "services/tlen-personal-info-service.h"

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

		bool validateUserID(const QString& uid);

		tlen * client() { return TlenClient; }

		Buddy nodeToBuddy(QDomNode node);

		virtual Conference * loadConferenceFromStorage(const QSharedPointer<StoragePoint> &storage)
		{
			Q_UNUSED(storage);

			return 0;
		}

		virtual QPixmap statusPixmap(Status status);
		virtual QPixmap statusPixmap(const QString &statusType);

		virtual QIcon statusIcon(Status status);
		virtual QIcon statusIcon(const QString &statusType);

		virtual void changeStatus(Status status);

		virtual AvatarService *avatarService() { return CurrentAvatarService; }
		virtual ChatService * chatService() { return CurrentChatService; }
		virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }

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
		TlenPersonalInfoService *CurrentPersonalInfoService;
		friend class TlenChatService;

	private slots:
		void login(const QString &password, bool permanent);
		void connectToServer();
		void login();
		void logout();
		//void connectedToServer();
		//void disconnectedFromServer();

		void presenceDisconnected();
		void itemReceived(QString jid, QString name, QString subscription, QString group);
		void presenceChanged(QString from, QString status, QString description);
		void authorizationAsk(QString to);
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

		void contactAdded(Contact contact);
		void contactUpdated(Contact contact);
		void contactAboutToBeRemoved(Contact contact);
		void contactIdChanged(Contact contact,const QString &oldId);

	public slots:
		bool sendMessage(Chat chat, FormattedMessage &message);

	signals:
		void authorize(QString to, bool subscribe);

		void userStatusChangeIgnored(Buddy);

		void filterOutgoingMessage(Chat chat, QByteArray &msg, bool &stop);
		void messageStatusChanged(int messsageId, ChatService::MessageStatus status);
		void filterIncomingMessage(Chat chat, Contact sender, const QString &message, time_t time, bool &ignore);
		void messageSent(const Message &message);
		void messageReceived(const Message &message);
};

#endif // TLEN_PROTOCOL_H
