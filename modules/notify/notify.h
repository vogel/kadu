#ifndef NOTIFY_H
#define NOTIFY_H
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qpair.h>
#include <qvaluelist.h>

#include "gadu.h"
#include "notify_slots.h"

class Notify : QObject
{
	Q_OBJECT
	private:
	class Notifier
	{
		public:
		Notifier();
		Notifier(QObject *notifier, const QMap<QString, QString> &notifierSlots);
		
		QObject *notifier; //obiekt "nas³uchuj±cy" na zdarzenia
		QMap<QString, QPair<QString, bool> > notifierSlots; //nazwa sygna³u("NewMessage") -> para<slot (SLOT(mójSlotMessage)), czy po³±czony>
	};
	QMap<QString, Notifier> notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia
	QMap<QString, QString> notifySignals; //nazwa sygna³u("NewMessage") -> sygna³ (SIGNAL(newMessage(...)))
	
	/*
	 * dodaje kolumnê checkboksów w konfiguracji,
	 * na podstawie notifierSlots decyduje czy dodaæ checkboksa aktywnego czy nie
	 */
	void addConfigColumn(const QString &name, const QMap<QString, QString> &notifierSlots);
	
	/* usuwa kolumnê checkboksów z konfiguracji */
	void removeConfigColumn(const QString &name, const QMap<QString, QPair<QString, bool> > &notifierSlots);
	
	private slots:
	/* obs³uga zmian statusów */
	void changingStatus(const UinType, const unsigned int, const unsigned int);

	/* obs³uga zmian statusów */
	void changedStatus(UserListElement *);
	
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
	Notify();
	~Notify();
	
	/*
	 * rejestruje obiekt, który chce otrzymywaæ informacje o zdarzeniach
	 * name bêdzie u¿yte w oknie konfiguracji (nale¿y podawaæ angielsk± nazwê)
	 * slots to mapa odwzoruj±ca nazwy sygna³ów na sloty w obiekcie notifier
	 * nazwy sygna³ów:
		"NewChat" - nowa rozmowa
		"NewMessage" - nowa wiadomo¶æ
		"ConnError" - b³±d po³±czenia
		"ChangingStatus" - zmiana statusu (dopiero siê odbêdzie)
		"toAvailable" - u¿ytkownik zmieni³ status na "Dostêpny"
		"toBusy" - u¿ytkownik zmieni³ status na "Zaraz wracam"
		"toNotAvailable" - u¿ytkownik zmieni³ status na "Niewidoczny lub Niedostêpny"
		"Message" - inna wiadomo¶æ
	 * 
	 * nie trzeba definiowaæ wszystkich slotów
	 */
	void registerNotifier(const QString &name, QObject *notifier,
							const QMap<QString, QString> &notifierSlots);
	/*
	 * wyrejestrowuje obiekt
	 */
	void unregisterNotifier(const QString &name);
	
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
	void emitMessage(const QString &from, const QString &to, const QString &type=QString(), const QString &message=QString(), const UserListElement *ule=NULL);
	
	signals:
	//UWAGA: razem ze zmianami nazw/parametrów tych sygna³ów nale¿y aktualizowaæ wpisy w konstruktorze Notify
	
	/* nowa rozmowa */
	void newChat(UinsList senders, const QString& msg, time_t time, bool &grab);
	
	/* nowa wiadomo¶æ w oknie chat */
	void newMessage(UinsList senders, const QString& msg, time_t time);
	
	/* b³±d po³±czenia */
	void connectionError(const QString &message);
	
	/* u¿ytkownik ZMIENIA status (mo¿na operowaæ tylko na podanych parametrach, bo status _jeszcze_ siê nie zmieni³) */
	void userChangingStatus(const UinType uin, const unsigned int oldstatus, const unsigned int status);
	
	/* u¿ytkownik ZMIENI£ status na "Dostêpny" */
	void userChangedStatusToAvailable(const UserListElement &);
	
	/* u¿ytkownik zmieni³ status na "Zaraz wracam" */
	void userChangedStatusToBusy(const UserListElement &);
	
	/* u¿ytkownik zmieni³ status na "Niewidoczny" lub "Niedostêpny" */
	void userChangedStatusToNotAvailable(const UserListElement &);

	/* inna informacja do powiadomienia */
	void message(const QString &from, const QString &type, const QString &message, const UserListElement *ule);
	
	signals:
	/* do u¿ytku wewnêtrznego */
	void privateMessage(const QString &from, const QString &message, const QString &type, const UserListElement *ule);
};

extern Notify *notify;
extern NotifySlots *notify_slots;

#endif
