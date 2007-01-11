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

enum CallbackRequirement {
	CallbackRequired,
	CallbackNotRequired
};

enum CallbackCapacity {
	CallbackSupported,
	CallbackNotSupported
};

/**
 * @defgroup notify Notify
 * @{
 */
class Notifier : public QObject {
	public:
		Notifier(QObject *parent = 0, const char *name = 0) : QObject(parent, name) {};
		virtual ~Notifier() {};
		virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }
		virtual void externalEvent(Notification *notification) = 0;
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

	private slots:
		/* pomocniczy slot */
		void newChatSlot(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);
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

	public:
		Notify(QObject *parent=0, const char *name=0);
		~Notify();

		/*
		 * rejestruje obiekt, który chce otrzymywaæ informacje o zdarzeniach
		 * name bêdzie u¿yte w oknie konfiguracji (nale¿y podawaæ angielsk± nazwê)
		 * slots to mapa odwzoruj±ca nazwy sygna³ów na sloty w obiekcie notifier
		 * nazwy sygna³ów:
			"NewChat" - nowa rozmowa
			"NewMessage" - nowa wiadomo¶æ
			"ConnError" - b³±d po³±czenia
			"StatusChanged" - zmiana statusu ogólna
			"toAvailable" - u¿ytkownik zmieni³ status na "Dostêpny"
			"toBusy" - u¿ytkownik zmieni³ status na "Zaraz wracam"
			"toInvisible" - u¿ytkownik zmieni³ status na "Ukryty"
			"toNotAvailable" - u¿ytkownik zmieni³ status na "Niewidoczny lub Niedostêpny"
			"UserBoxChangeToolTip"
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

		/* nowa rozmowa */
		void newChat(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);

		/* nowa wiadomo¶æ w oknie chat */
		void newMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);

		/* b³±d po³±czenia */
		void connectionError(Protocol *protocol, const QString &message);

		/* u¿ytkownik zmieni³ status */
		void userStatusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus);

		/* u¿ytkownik zmieni³ status na "Dostêpny" */
		void userChangedStatusToAvailable(const QString &protocolName, UserListElement);

		/* u¿ytkownik zmieni³ status na "Zaraz wracam" */
		void userChangedStatusToBusy(const QString &protocolName, UserListElement);

		/* u¿ytkownik zmieni³ status na "Niewidoczny" */
		void userChangedStatusToInvisible(const QString &protocolName, UserListElement);

		/* u¿ytkownik zmieni³ status na "Niewidoczny" lub "Niedostêpny" */
		void userChangedStatusToNotAvailable(const QString &protocolName, UserListElement);

		void userBoxChangeToolTip(const QPoint &point, UserListElement, bool show);

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
