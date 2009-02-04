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

public:
	enum MessageStatus {
		StatusAcceptedDelivered,
		StatusAcceptedQueued,
		StatusRejectedBlocked,
		StatusRejectedBoxFull,
		StatusRejectedUnknown
	};

private:
	Account *CurrentAccount;
	Status CurrentStatus;
	Status NextStatus;

	Protocol(const Protocol &) {}
	Protocol & operator = (const Protocol &) {}

protected:
	QDateTime ConnectionTime;

	ProtocolFactory *Factory;

	void setAllOffline();

	virtual void changeStatus(Status status) = 0;
	void statusChanged(Status status);

public:
	Protocol(Account *account, ProtocolFactory *factory);
	virtual ~Protocol();

	virtual void setAccount(Account *account);

	void setStatus(Status status);

	const Status & status() const { return CurrentStatus; }
	const Status & nextStatus() const { return NextStatus; }

	virtual bool validateUserID(QString &uid) = 0;

	virtual UserStatus * newStatus() const = 0;

	const QDateTime & connectionTime() const;

	virtual void setData(AccountData *data) = 0;
	virtual AccountData * createAccountData() = 0;
	ProtocolFactory * protocolFactory() const { return Factory; }
	Account * account() const { return CurrentAccount; }

	virtual QPixmap statusPixmap(Status status) = 0;
	QPixmap statusPixmap() { return statusPixmap(CurrentStatus); }

	QIcon icon();

public slots:
	virtual bool sendMessage(Contact user, const QString &messageContent);
	virtual bool sendMessage(ContactList users, const QString &messageContent);
	virtual bool sendMessage(Contact user, Message &message);
	virtual bool sendMessage(ContactList users, Message &message) = 0;


signals:
	/**
		uda�o si� zalogowa�
	**/
	void connected();

	/**
		rozpoczynamy procedur� logowania si�
	**/
	void connecting();

	/**
		roz��czyli�my si� z serwerem
	**/
	void disconnected();

	/**
		wyst�pi� b��d po��czenia
		@param account konto
		@param reason napis do wy�wietlenia dla u�ytkownika
	**/
	void connectionError(Account *account, const QString &server, const QString &reason);

	/**
		\fn void messageFiltering(const ContactList users, QCString& msg, bool& stop)
		Sygnal daje mozliwosc operowania na wiadomosci
		ktora ma byc wyslana do serwera juz w jej docelowej
		formie po konwersji z unicode i innymi zabiegami.
		Tresc wiadomosci mozna zmienic podmieniajac wskaznik
		msg na nowy bufor i zwalniajac stary (za pomoca free).
		Mozna tez przerwac dalsza jej obrobke ustawiajac
		wskaznik stop na true.
		\param users lista u�ytkownik�w
		\param msg wiadomo��
		\param stop zako�czenie dalszej obr�bki sygna�u
	**/
	void sendMessageFiltering(const ContactList users, QByteArray &msg, bool &stop);

	/**
		Message with id messageId was delivered or rejected.
	**/
	void messageStatusChanged(int messsageId, Protocol::MessageStatus status);

	/**
		\fn receivedMessageFilter(Account *account, ContactList senders, const QString &msg, time_t time, bool &ignore);
		Filtrujemy wiadomo��. Mo�na j� odrzuci� albo i nie.
		\param account konto na kt�rym otrzymali�my wiadomo��
		\param senders lista nadawc�w
		\param message komunikat w postaci Unicode HTML
		\param time czas nadania wiadomo�ci
		\param ignore po ustawieniu na true wiadomo�� jest ignorowana
	**/
	void receivedMessageFilter(Account *account, ContactList senders, const QString &message, time_t time, bool &ignore);

	/**
		\fn messageReceived(Account *account, ContactList senders, const QString &msg, time_t time);
		Otrzymali�my wiadomo��.
		\param account konto na kt�rym otrzymali�my wiadomo��
		\param senders lista nadawc�w
		\param message komunikat w postaci Unicode HTML
		\param time czas nadania wiadomo�ci
	**/
	void messageReceived(Account *account, ContactList senders, const QString &message, time_t time);

	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);

};

#endif
