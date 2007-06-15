#ifndef NOTIFY_H
#define NOTIFY_H
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qvariant.h>
#include <qpair.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include <time.h>

#include "notify_slots.h"
#include "notification.h"
#include "protocol.h"
#include "userlist.h"

class MessageNotification;

/**
 * @defgroup notify Notify
 * @{
 */

/**
	@enum CallbackRequirement
	Okre¶la, czy dane zdarzenie wymaga podjêcia od u¿ytkownika akcji innej ni¿ domy¶lne zaakceptowanie/odrzucenie.
 **/
enum CallbackRequirement {
	CallbackRequired,
	CallbackNotRequired
};

/**
	@enum CallbackCapacity
	Okre¶la, czy dany notifikator potrafi obs³u¿yæ zdarzenia wymagajace od u¿ytkownika akcji innej ni¿ domy¶lne zaakceptowanie/odrzucenie.
 **/
enum CallbackCapacity {
	CallbackSupported,
	CallbackNotSupported
};

/**
	@class Notifier
	@brief Klasa abstrakcyjna opisuj±ca notifikator.

	Notifykatory zajmuj± siê wy¶wietlaniem lub informowaniem w inny sposób u¿ytkownika o wystêpuj±cych
	w programie zdarzeniach (nowa rozmowa, nowy transfer pliku, b³±d...).

	Notyfikatory mogê umo¿liwiaæ u¿ytkownikowi podjêcie akcji jak odebranie lub zignorownie rozmowy,
	odebranie pliku, kontynuacje odbierania pliku i inne. Niektóry notifikatory nie bêd±
	implementowaæ akcji, dlatego te¿ niektóre zdarzenia nie mog± byæ przez nie obs³ugiwane.
 **/
class Notifier : public virtual QObject {
	public:
		Notifier(QObject *parent = 0, const char *name = 0) : QObject(parent, name) {};
		virtual ~Notifier() {};

		/**
			Okre¶la, czy notifikator poradzi sobie ze zdarzeniami wymagajacymi podjêcia akcji.
		 **/
		virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

		/**
			Metoda informuj±ca notifikator o nowym zdarzeniu. Zdarzenie mo¿e wywo³aæ
			sygna³ closed(), po którym notyfikator musi przestaæ informowaæ u¿ytkownika
			o danym zdarzeniu (na przyk³ad, musi zamkn±æ skojarzone ze zdarzeniem okno).
		 **/
		virtual void notify(Notification *notification) = 0;

		/**
			Kopiuje konfiguracje jednego zdarzenia do drugiego.
			U¿ywane przy przej¶ciu z 0.5 na 0.6 - po 0.6 zostanie usuniête.
		 **/
		virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent) = 0;
};

class Notify : public QObject
{
	Q_OBJECT
	private:
		QMap<QString, Notifier *> notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia

		class NotifyEvent
		{
			public:
				QString name;
				QCString wname;
				CallbackRequirement callbackRequirement;
				const char *description;
				NotifyEvent() : name(), wname(), callbackRequirement(CallbackNotRequired), description(0){}
		};
		QValueList<NotifyEvent> NotifyEvents;
		QMap<QString, QValueList<QCString> > strs;

		/*
		 * dodaje kolumnê checkboksów w konfiguracji,
		 * na podstawie notifierSlots decyduje czy dodaæ checkboksa aktywnego czy nie
		 */
		void addConfigColumn(const QString &name, CallbackCapacity callbackCapacity);

		/* usuwa kolumnê checkboksów z konfiguracji */
		void removeConfigColumn(const QString &name);

		void addConfigRow(const QString &name, const char *description, CallbackRequirement callbackRequirement);
		void removeConfigRow(const QString &name);

		void import_connection_from_0_5_0(const QString &notifierName, const QString &oldConnectionName, const QString &newConnectionName);

	private slots:

		void newChatSlot(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool grabbed);
		void probablyNewMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);
		void connectionError(Protocol *protocol, const QString &message);
		void statusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);

	public:
		Notify(QObject *parent=0, const char *name=0);
		~Notify();

		void notify(Notification *notification);

		void registerNotifier(const QString &name, Notifier *notifier);
		/*
		 * wyrejestrowuje obiekt
		 */
		void unregisterNotifier(const QString &name);

		void registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement);
		void unregisterEvent(const QString &name);

		QStringList notifiersList() const;
		const QValueList<Notify::NotifyEvent> &notifyEvents();

};

extern Notify *notification_manager;
extern NotifySlots *notify_slots;

/** @} */

#endif
