#ifndef KADU_PROTOCOL_H
#define KADU_PROTOCOL_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtGui/QIcon>

#ifdef __sun__
#include <sys/types.h>
#endif /* __sun__ */

#ifdef _MSC_VER
#include "kinttypes.h"
#endif

#include "contacts/contact.h"
#include "contacts/contact-list.h"

#include "../status.h"
#include "status.h"

typedef uint32_t UinType;

class QPixmap;

class AccountData;
class Message;
class ProtocolFactory;
class Status;

class KADUAPI Protocol : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Protocol)

public:
	enum NetworkState {
		NetworkDisconnected,
		NetworkConnecting,
		NetworkConnected,
		NetworkDisconnecting
	};

	enum MessageStatus {
		StatusAcceptedDelivered,
		StatusAcceptedQueued,
		StatusRejectedBlocked,
		StatusRejectedBoxFull,
		StatusRejectedUnknown
	};

private:
	ProtocolFactory *Factory;

	Account *CurrentAccount;

	NetworkState State;
	Status CurrentStatus;
	Status NextStatus;

protected:
	void setAllOffline();

	virtual void changeStatus(Status status) = 0;
	void statusChanged(Status status);

	void networkStateChanged(NetworkState state);

public:
	Protocol(Account *account, ProtocolFactory *factory);
	virtual ~Protocol();

	ProtocolFactory * protocolFactory() const { return Factory; }
	Account * account() const { return CurrentAccount; }

	virtual void setAccount(Account *account); 
	virtual bool validateUserID(QString &uid) = 0;

	NetworkState state() { return State; }

	void setStatus(Status status);
	const Status & status() const { return CurrentStatus; }
	const Status & nextStatus() const { return NextStatus; }

	virtual QPixmap statusPixmap(Status status) = 0;
	QPixmap statusPixmap() { return statusPixmap(CurrentStatus); }

	QIcon icon();

public slots:
	virtual bool sendMessage(Contact user, const QString &messageContent);
	virtual bool sendMessage(ContactList users, const QString &messageContent);
	virtual bool sendMessage(Contact user, Message &message);
	virtual bool sendMessage(ContactList users, Message &message) = 0;

signals:
	void connecting(Account *account);
	void connected(Account *account);
	void disconnecting(Account *account);
	void disconnected(Account *account);

	void statusChanged(Account *account, Status newStatus);
	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);


// TODO: REVIEW
	void connectionError(Account *account, const QString &server, const QString &reason);

	void sendMessageFiltering(const ContactList users, QByteArray &msg, bool &stop);
	void messageStatusChanged(int messsageId, Protocol::MessageStatus status);
	void receivedMessageFilter(Account *account, ContactList senders, const QString &message, time_t time, bool &ignore);
	void messageReceived(Account *account, ContactList senders, const QString &message, time_t time);

};

#endif
