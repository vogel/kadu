#ifndef KADU_PROTOCOL_H
#define KADU_PROTOCOL_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>

#include "status.h"

class Protocol : public QObject
{
	Q_OBJECT
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
		Protocol(const QString &id, QObject *parent = 0, const char *name = 0);
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
		UserStatus &status() { return *NextStatus; }

		/**
			Rzeczywisty aktualny status. Mo¿na go wykorzystaæ tylko w trybie do odczytu (pobranie
			ikony, nazwy, sprawdzenie rzeczywistego stanu po³±czenia).

			@see status
		**/
		const UserStatus &currentStatus() const { return *CurrentStatus; }

		QString protocolID() const {	return ProtocolID;	}

		QString ID() const {	return id;	}

		virtual UserStatus *newStatus() const = 0;

		const QDateTime &connectionTime() const;

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
};

#endif
