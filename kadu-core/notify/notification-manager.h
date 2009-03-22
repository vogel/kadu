#ifndef NOTIFY_H
#define NOTIFY_H

#include <QtGui/QGroupBox>

#include <time.h>

#include "accounts/accounts_aware_object.h"
#include "main_configuration_window.h"
#include "notification.h"
#include "notifier.h"
#include "protocols/protocol.h"

class ActionDescription;
class KaduAction;

class MessageNotification;
class Notifier;
class NotifierConfigurationWidget;
class NotifyGroupBox;

class QListWidget;

/**
 * @defgroup notify Notify
 * @{
 */

class KADUAPI Notify : public ConfigurationUiHandler, AccountsAwareObject
{
	Q_OBJECT

	QListWidget *allUsers;
	QListWidget *notifiedUsers;
	ConfigComboBox *notifications;
	ConfigGroupBox *notificationsGroupBox;

	ActionDescription *notifyAboutUserActionDescription;

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

private slots:

	void messageReceived(Account *account, ContactList contacts, const QString &msg, time_t t);

	void connectionError(Account *account, const QString &server, const QString &message);
	void statusChanged(Account *account, Contact contact, Status oldStatus);

	void notifyAboutUserActionActivated(QAction *sender, bool toggled);

	void moveToNotifyList();
	void moveToAllList();

	void configurationWindowApplied();
	void eventSwitched(int index);
	void notifierToggled(const QString &notifier, bool toggled);

	void mainConfigurationWindowDestroyed();

protected:
	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	Notify(QObject *parent = 0);
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

void checkNotify(KaduAction*);

extern KADUAPI Notify *notification_manager;

/** @} */

#endif
