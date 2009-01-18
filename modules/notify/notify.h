#ifndef NOTIFY_H
#define NOTIFY_H

#include <QtGui/QGroupBox>

#include <time.h>

#include "accounts/accounts_aware_object.h"
#include "main_configuration_window.h"
#include "notification.h"
#include "protocols/protocol.h"
#include "notify_exports.h"

class MessageNotification;

class QListWidget;

/**
 * @defgroup notify Notify
 * @{
 */

/**
	@enum CallbackRequirement
	Okre�la, czy dane zdarzenie wymaga podj�cia od u�ytkownika akcji innej ni� domy�lne zaakceptowanie/odrzucenie.
 **/
enum CallbackRequirement {
	CallbackRequired,
	CallbackNotRequired
};

/**
	@enum CallbackCapacity
	Okre�la, czy dany notifikator potrafi obs�u�y� zdarzenia wymagajace od u�ytkownika akcji innej ni� domy�lne zaakceptowanie/odrzucenie.
 **/
enum CallbackCapacity {
	CallbackSupported,
	CallbackNotSupported
};

/**
	@class NotifierConfigurationWidget
	@author Vogel
	@short Widget konfiguracyjny dla Notifiera.

	Widget jest tworzony i dodawany w odpowiednim miejscu w oknie konfiguracyjnym.
	Zawarto�� element�w zmienia si� po wyborze innej notyfikacji w oknie konfiguracyjnym.
	Wiget zapamietuje wszystkie zmiany dla wszystkich typ�w notyfikacji i w odpowienim
	momencie je zapisuje.
**/
class NOTIFYAPI NotifierConfigurationWidget : public QWidget
{
	Q_OBJECT

public:
	NotifierConfigurationWidget(QWidget *parent = 0, char *name = 0) : QWidget(parent, name) {}

	/**
		W tej metodzie widget mo�e wczyta� konfigruacje wszystkich zdarze�.
	 **/
	virtual void loadNotifyConfigurations() = 0;
	/**
		W tej metodzie widget musi zapisa� wszystkie zmienione konfiguracje
		wszystkich zmienionych zdarze�.
	 **/
	virtual void saveNotifyConfigurations() = 0;

public slots:
	/**
		Slot wywo�ywany, gdy widget ma prze��czy� si� na konfigruacj� innego zdarzenia.
		Zmiany w aktualnym zdarzeniu powinny zosta� zapisane.

		@arg event - nazwa nowego zdarzenia
	 **/
	virtual void switchToEvent(const QString &event) = 0;

};

/**
	@class Notifier
	@brief Klasa abstrakcyjna opisuj�ca notifikator.

	Notifykatory zajmuj� si� wy�wietlaniem lub informowaniem w inny spos�b u�ytkownika o wyst�puj�cych
	w programie zdarzeniach (nowa rozmowa, nowy transfer pliku, b��d...).

	Notyfikatory mog� umo�liwia� u�ytkownikowi podj�cie akcji jak odebranie lub zignorownie rozmowy,
	odebranie pliku, kontynuacje odbierania pliku i inne. Niekt�ry notifikatory nie b�d�
	implementowa� akcji, dlatego te� niekt�re zdarzenia nie mog� by� przez nie obs�ugiwane.
 **/
class NOTIFYAPI Notifier : public virtual QObject
{
	public:
		Notifier(QObject *parent = 0, const char *name = 0) : QObject(parent, name) {};
		virtual ~Notifier() {};

		/**
			Okre�la, czy notifikator poradzi sobie ze zdarzeniami wymagajacymi podj�cia akcji.
		 **/
		virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

		/**
			Metoda informuj�ca notifikator o nowym zdarzeniu. Zdarzenie mo�e wywo�a�
			sygna� closed(), po kt�rym notyfikator musi przesta� informowa� u�ytkownika
			o danym zdarzeniu (na przyk�ad, musi zamkn�� skojarzone ze zdarzeniem okno).
		 **/
		virtual void notify(Notification *notification) = 0;

		/**
			Kopiuje konfiguracje jednego zdarzenia do drugiego.
			U�ywane przy przej�ciu z 0.5 na 0.6 - po 0.6 zostanie usuni�te.
		 **/
		virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent) = 0;

		/**
			Zwraca widget, jaki zostanie dodany do okna konfiguracyjnego
			na prawo od odpowiedniego CheckBoxa.
			Mo�e zwr�ci� zero.
		 **/
		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0) = 0;
};

class NotifyGroupBox : public QGroupBox
{
	Q_OBJECT

	QString Notificator;

private slots:
	void toggledSlot(bool toggled);

public:
	NotifyGroupBox(const QString &notificator, const QString &caption, QWidget *parent = 0, char *name = 0);
	virtual ~NotifyGroupBox() {}

	QString notificator() { return Notificator; }

signals:
	void toggled(const QString &notificator, bool toggled);

};

class NOTIFYAPI Notify : public ConfigurationUiHandler, AccountsAwareObject
{
	Q_OBJECT

	QListWidget *allUsers;
	QListWidget *notifiedUsers;
	ConfigComboBox *notifications;
	ConfigGroupBox *notificationsGroupBox;

	struct NotifierData
	{
		Notifier *notifier;
		NotifierConfigurationWidget *configurationWidget;
		NotifyGroupBox *configurationGroupBox;
		QMap<QString, bool> events;
	};

	QMap<QString, NotifierData> Notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia

	struct NotifyEvent
	{
		QString name;
		CallbackRequirement callbackRequirement;
		const char *description;
		NotifyEvent() : name(), callbackRequirement(CallbackNotRequired), description(0){}

		bool operator == (const NotifyEvent &compare) { return name == compare.name; }
	};
	QList<NotifyEvent> NotifyEvents;

	QString CurrentEvent;

	void import_connection_from_0_5_0(const QString &notifierName, const QString &oldConnectionName, const QString &newConnectionName);
	void createDefaultConfiguration();

	void addConfigurationWidget(NotifierData &notifier, const QString &name);
	void removeConfigurationWidget(NotifierData &notifier);

	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

private slots:

	void messageReceived(Account *account, ContactList contacts, const QString &msg, time_t t);

	void connectionError(Account *account, const QString &server, const QString &message);
	void statusChanged(Account *account, Contact contact, Status oldStatus);

	void moveToNotifyList();
	void moveToAllList();

	void configurationWindowApplied();
	void eventSwitched(int index);
	void notifierToggled(const QString &notifier, bool toggled);

	void mainConfigurationWindowDestroyed();

public:
	Notify(QObject *parent=0, const char *name=0);
	virtual ~Notify();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void notify(Notification *notification);

	void registerNotifier(const QString &name, Notifier *notifier);
	void unregisterNotifier(const QString &name);

	void registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement);
	void unregisterEvent(const QString &name);

	QStringList notifiersList() const;
	const QList<Notify::NotifyEvent> &notifyEvents();

};

extern NOTIFYAPI Notify *notification_manager;

/** @} */

#endif
