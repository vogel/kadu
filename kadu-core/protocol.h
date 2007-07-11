#ifndef KADU_PROTOCOL_H
#define KADU_PROTOCOL_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>

#include "status.h"
#include "usergroup.h"

typedef uint32_t UinType;

class Protocol : public QObject
{
	Q_OBJECT

	private:
		UserStatus &writeableStatus() { return *NextStatus; }
		friend class Kadu;

	protected:
		QDateTime ConnectionTime;

		QString ProtocolID;
		QString id;

		/**
			Bie¿±cy status. Zmieniany po po³±czeniu, oraz w przypadku zmiany statusu kiedy po³±czenie
			jest ju¿ zainicjowane.

			@see login
			@see connected
			@see NextStatus
			@see UserStatus
			@see status
		**/
		UserStatus *CurrentStatus;

		/**
			Nastêpny status. Ustalany zewnêtrznie przy wykorzystaniu metody status i odpowiednich
			slotów klasy UserStatus. Zmiana wywo³uje jedn± z metod iWantGo... i w konsekwencji zmianê
			statusu (w razie konieczno¶ci te¿ zalogowanie).

			@see login
			@see connected
			@see CurrentStatus
			@see UserStatus
			@see status
		**/
		UserStatus *NextStatus;

	public:
		Protocol(const QString &proto, const QString &id, QObject *parent = 0, const char *name = 0);
		virtual ~Protocol();

		/**
			Status u¿ytkownika. Za pomoc± tej metody mo¿emy go zmieniæ, pobraæ ikonê statusu i wykonaæ
			kilka innych ciekawych rzeczy.

			1. Zmiana statusu:
			<code>
				Protocol *proto;

				...

				proto->status().setOnline("Jestem zalogowany"); // zalogowanie i ustawienie opisu
				proto->status().setFriendsOnly(true);           // tryb tylko dla przyjació³
				...
				proto->status().setOffline();                   // wylogowanie, usuniêcie opisu
			</code>

			2. Sprawdzenie statusu:
			<code>
				Protocol *proto;

				if (proto->status().isOnline())                 // jeste¶my online
					...
				else if (proto->status().isInvisible())         // jeste¶my niewidzialni
					...

				// mo¿na te¿:
				switch (proto->status().status())
				{
					case Online:
						break;
					case Busy:
						break;
					case Invisible:
						break;
					case Offline:
						break;
				}
			</code>

			3. Pobranie ikony i nazwy statusu
			<code>
				QPixmap pix;
				QString name;
				Protocol *proto;

				...

				pix = proto->status().pixmap();
				name = proto->status().name();
			</code>

			@see currentStatus
		**/
		const UserStatus &status() { return *NextStatus; }

		/**
			Rzeczywisty aktualny status. Mo¿na go wykorzystaæ tylko w trybie do odczytu (pobranie
			ikony, nazwy, sprawdzenie rzeczywistego stanu po³±czenia).

			@see status
		**/
		const UserStatus &currentStatus() const { return *CurrentStatus; }

		QString protocolID() const { return ProtocolID; }

		QString ID() const { return id; }

		virtual bool validateUserID(QString& uid) = 0;

		virtual UserStatus *newStatus() const = 0;

		const QDateTime &connectionTime() const;
	public slots:
		/**
			Wysy³a wiadomo¶æ bez formatowania tekstu. Je¶li adresatów jest wiêcej ni¿ jeden, to wysy³ana
			jest wiadomo¶æ konferencyjna. Zwracany jest numer sekwencyjny wiadomo¶ci, je¶li
			przypadkiem by¶my chcieli ¶ledziæ jej potwierdzenie.
			@param users lista u¿ytkowników, do których wysy³amy wiadomo¶æ
			@param mesg wiadomo¶æ, któr± wysy³amy - kodowanie zmieniane wewn±trz
		**/
		virtual /*int*/void sendMessage(UserListElements users, const QString &mesg) = 0;
		/**
			Wysy³a wiadomo¶æ bez formatowania tekstu. Zwracany jest numer sekwencyjny wiadomo¶ci, je¶li
			przypadkiem by¶my chcieli ¶ledziæ jej potwierdzenie.
			@param users lista u¿ytkowników, do których wysy³amy wiadomo¶æ
			@param mesg wiadomo¶æ, któr± wysy³amy - kodowanie zmieniane wewn±trz
		**/
		/*int*/void sendMessage(UserListElement user, const QString &mesg);


	signals:

		/**
			uda³o siê zalogowaæ
		**/
		void connected();

		/**
			rozpoczynamy procedurê logowania siê
		**/
		void connecting();

		/**
			roz³±czyli¶my siê z serwerem
		**/
		void disconnected();

		/**
			wyst±pi³ b³±d po³±czenia
			@param protocol protokó³
			@param reason napis do wy¶wietlenia dla u¿ytkownika
		**/
		void connectionError(Protocol *protocol, const QString &reason);

		/**
			\fn void messageFiltering(const UserGroup *users, QCString& msg, bool& stop)
			Sygnal daje mozliwosc operowania na wiadomosci
			ktora ma byc wyslana do serwera juz w jej docelowej
			formie po konwersji z unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic podmieniajac wskaznik
			msg na nowy bufor i zwalniajac stary (za pomoca free).
			Mozna tez przerwac dalsza jej obrobke ustawiajac
			wskaznik stop na true.
			\param users lista u¿ytkowników
			\param msg wiadomo¶æ
			\param stop zakoñczenie dalszej obróbki sygna³u
		**/
		void sendMessageFiltering(const UserListElements users, QString &msg, bool &stop);
		/**
			wiadomo¶æ nie zosta³a dostaczona
		**/
		void messageNotDelivered(const QString &message);
		/**
			wiadomo¶æ zosta³a przyjêta przez serwer
			TODO: WTF??
		**/
		void messageAccepted();

		/**
			\fn receivedMessageFilter(Protocol *protocol, UserListElements senders, const QString &msg, time_t time, bool &ignore);
			Filtrujemy wiadomo¶æ. Mo¿na j± odrzuciæ albo i nie.
			\param protocol protokó³ na którym otrzymali¶my wiadomo¶æ
			\param senders lista nadawców
			\param message komunikat w postaci Unicode HTML
			\param time czas nadania wiadomo¶ci
			\param ignore po ustawieniu na true wiadomo¶æ jest ignorowana
		**/
		void receivedMessageFilter(Protocol *protocol, UserListElements senders, const QString &message, time_t time, bool &ignore);
		/**
			\fn messageReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t time);
			Otrzymali¶my wiadomo¶æ.
			\param protocol protokó³ na którym otrzymali¶my wiadomo¶æ
			\param senders lista nadawców
			\param message komunikat w postaci Unicode HTML
			\param time czas nadania wiadomo¶ci
		**/
		void messageReceived(Protocol *protocol, UserListElements senders, const QString &message, time_t time);

	private:
		Protocol(const Protocol &) {}
		virtual Protocol &operator=(const Protocol &){return *this;}
};

#endif
