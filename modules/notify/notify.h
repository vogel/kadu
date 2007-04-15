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
class Notifier : public QObject {
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
		virtual void externalEvent(Notification *notification) = 0;

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
		class NotifierSlots
		{
			public:
			NotifierSlots();
			NotifierSlots(Notifier *notifier, const QMap<QString, QString> &notifierSlots);

			Notifier *notifier; //obiekt "nas³uchuj±cy" na zdarzenia
			QMap<QString, QPair<QString, bool> > notifierSlots; //nazwa sygna³u("NewMessage") -> para<slot (SLOT(mójSlotMessage)), czy po³±czony>
		};
		QMap<QString, NotifierSlots> notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia
		QMap<QString, QString> notifySignals; //nazwa sygna³u("NewMessage") -> sygna³ (SIGNAL(newMessage(...)))
		QStringList eventNames;

		class NotifyEvent
		{
			public:
				QString name;
				QCString wname;
				CallbackRequirement callbackRequirement;
				const char *description;
				NotifyEvent() : name(), wname(), callbackRequirement(CallbackNotRequired), description(0){}
		};
		QValueList<NotifyEvent> notifyEvents;
		QMap<QString, QValueList<QCString> > strs;

		/*
		 * dodaje kolumnê checkboksów w konfiguracji,
		 * na podstawie notifierSlots decyduje czy dodaæ checkboksa aktywnego czy nie
		 */
		void addConfigColumn(const QString &name, const QMap<QString, QString> &notifierSlots, CallbackCapacity callbackCapacity);

		/* usuwa kolumnê checkboksów z konfiguracji */
		void removeConfigColumn(const QString &name, const QMap<QString, QPair<QString, bool> > &notifierSlots);

		void addConfigRow(const QString &name, const char *description, CallbackRequirement callbackRequirement);
		void removeConfigRow(const QString &name);

		void import_connection_from_0_5_0(const QString &notifierName, const QString &oldConnectionName, const QString &newConnectionName);

	private slots:

		/* pomocniczy slot */
		void newChatSlot(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool grabbed);
		/* pomocniczy slot */
		void probablyNewMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);

		/*
		 * ³±czy odpowiedni sygna³ z notifierName (np.: Window, Hint, Sound)
		 * ze slotem slotName (np.: NewChat, ConnError)
		 */
		void connectSlot(const QString &notifierName, const QString &slotName);

		/*
		 * roz³±cza odpowiedni sygna³ z notifierName (np.: Window, Hint, Sound)
		 * od slotu slotName (np.: NewChat, ConnError)
		 */
		void disconnectSlot(const QString &notifierName, const QString &slotName);

		void connectionError(Protocol *protocol, const QString &message);

	public:
		Notify(QObject *parent=0, const char *name=0);
		~Notify();

		/*
		 * rejestruje obiekt, który chce otrzymywaæ informacje o zdarzeniach
		 * name bêdzie u¿yte w oknie konfiguracji (nale¿y podawaæ angielsk± nazwê)
		 * slots to mapa odwzoruj±ca nazwy sygna³ów na sloty w obiekcie notifier
		 * nazwy sygna³ów:
			"Message" - inna wiadomo¶æ
		 *
		 * nie trzeba definiowaæ wszystkich slotów
		 */
		void registerNotifier(const QString &name, Notifier *notifier,
							const QMap<QString, QString> &notifierSlots);
		/*
		 * wyrejestrowuje obiekt
		 */
		void unregisterNotifier(const QString &name);

		void registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement);
		void unregisterEvent(const QString &name);

		QStringList notifiersList() const;
		const QValueList<Notify::NotifyEvent> &externalNotifyTypes();

	public slots:
		/*
		 * aktualizuje wszystkie po³±czenia
		 * wywo³ywany po zmianie konfiguracji dotycz±cej obs³ugi
		 */
		void updateConnections();

		/*
		 * emituje sygna³ message
		 *  je¿eli to==QString::null, to wysy³a do wszystkich, którzy s± zaznaczeni w konfiguracji
		 *  je¿eli to!=QString::null, to wysy³a tylko do jednego
		 * Notifier decyduje, których argumentów u¿yæ
		 */
		void emitMessage(const QString &from, const QString &to, const QString &message=QString(), const QMap<QString, QVariant> *parameters=NULL, const UserListElement *ule=NULL);

		/* obs³uga zmian statusów */
		void statusChanged(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);

		void notify(Notification *notification);

	signals:
	//UWAGA: razem ze zmianami nazw/parametrów tych sygna³ów nale¿y aktualizowaæ wpisy w konstruktorze Notify

		/* inna informacja do powiadomienia */
		void message(const QString &from, const QString &msg, const QMap<QString, QVariant> *parameters, const UserListElement *ule);

	signals:
		/* do u¿ytku wewnêtrznego */
		void privateMessage(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
};

extern Notify *notify;
extern NotifySlots *notify_slots;

/** @} */

#endif
