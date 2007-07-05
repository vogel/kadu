#ifndef NOTIFY_H
#define NOTIFY_H

#include <qcheckbox.h>
#include <qmap.h>
#include <qobject.h>
#include <qpair.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qvariant.h>

#include <time.h>

#include "main_configuration_window.h"
#include "notification.h"
#include "protocol.h"
#include "userlist.h"

class MessageNotification;

class QListBox;

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

class NotifierConfigurationWidget : public QWidget
{
	Q_OBJECT

public:
	NotifierConfigurationWidget(QWidget *parent = 0, char *name = 0) : QWidget(parent, name) {}

	virtual void loadNotifyConfigurations() = 0;
	virtual void saveNotifyConfigurations() = 0;

public slots:
	virtual void switchToEvent(const QString &event) = 0;

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
class Notifier : public virtual QObject
{
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

		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0) = 0;
};

class NotifyCheckBox : public QCheckBox
{
	Q_OBJECT

	QString Notificator;

private slots:
	void toggledSlot(bool toggled);

public:
	NotifyCheckBox(const QString &notificator, const QString &caption, QWidget *parent = 0, char *name = 0);
	virtual ~NotifyCheckBox() {}

	QString notificator() { return Notificator; }

signals:
	void toggled(const QString &notificator, bool toggled);

};

class Notify : public ConfigurationUiHandler
{
	Q_OBJECT

	QListBox *allUsers;
	QListBox *notifiedUsers;
	ConfigComboBox *notifications;

	struct NotifierData
	{
		Notifier *notifier;
		NotifierConfigurationWidget *configurationWidget;
		NotifyCheckBox *configurationCheckBox;
		QMap<QString, bool> events;
	};

	QMap<QString, NotifierData> Notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia

	struct NotifyEvent
	{
		QString name;
		CallbackRequirement callbackRequirement;
		const char *description;
		NotifyEvent() : name(), callbackRequirement(CallbackNotRequired), description(0){}
	};
	QValueList<NotifyEvent> NotifyEvents;

	QString CurrentEvent;

	void import_connection_from_0_5_0(const QString &notifierName, const QString &oldConnectionName, const QString &newConnectionName);

private slots:

	void messageReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);

	void connectionError(Protocol *protocol, const QString &message);
	void statusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);

	void moveToNotifyList();
	void moveToAllList();

	void configurationWindowApplied();
	void eventSwitched(int index);
	void notifierToggled(const QString &notifier, bool toggled);

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
	const QValueList<Notify::NotifyEvent> &notifyEvents();

};

extern Notify *notification_manager;

/** @} */

#endif
