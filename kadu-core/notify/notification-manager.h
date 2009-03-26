/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QtGui/QGroupBox>

#include <time.h>

#include "accounts/accounts_aware_object.h"
#include "notification.h"
#include "notifier.h"
#include "protocols/protocol.h"

class ActionDescription;
class KaduAction;

class ConfigurationUiHandler;
class MessageNotification;
class Notifier;
class NotifierConfigurationWidget;
class NotifyConfigurationUiHandler;
class NotifyGroupBox;

/**
 * @defgroup notify Notify
 * @{
 */

class KADUAPI NotificationManager : QObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(NotificationManager)

	static NotificationManager *Instance;

	ActionDescription *notifyAboutUserActionDescription;
	NotifyConfigurationUiHandler *UiHandler;

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

	NotificationManager();
	virtual ~NotificationManager();

	void createDefaultConfiguration();

private slots:
	void messageReceived(Account *account, ContactList contacts, const QString &msg, time_t t);

	void connectionError(Account *account, const QString &server, const QString &message);
	void statusChanged(Account *account, Contact contact, Status oldStatus);

	void notifyAboutUserActionActivated(QAction *sender, bool toggled);

	friend class NotifyConfigurationUiHandler;

protected:
	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	static NotificationManager * instance();

	void notify(Notification *notification);

	void registerNotifier(const QString &name, Notifier *notifier);
	void unregisterNotifier(const QString &name);

	void registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement);
	void unregisterEvent(const QString &name);

	QStringList notifiersList() const;
	const QList<NotificationManager::NotifyEvent> &notifyEvents();

	ConfigurationUiHandler * configurationUiHandler();

};

void checkNotify(KaduAction*);

/** @} */

#endif // NOTIFICATION_MANAGER_H
