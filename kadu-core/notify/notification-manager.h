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

#include "accounts/accounts-aware-object.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "buddies/buddy.h"
#include "buddies/buddy-set.h"
#include "status/status.h"

class Action;
class ActionDescription;

class ConfigurationUiHandler;
class Group;
class Message;
class Notification;
class Notifier;
class NotifyConfigurationUiHandler;
class NotifyEvent;

/**
 * @defgroup notify Notify
 * @{
 */

class KADUAPI NotificationManager : public QObject, AccountsAwareObject, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(NotificationManager)

	static NotificationManager *Instance;

	void init();

	bool NotifyAboutAll;

	ActionDescription *notifyAboutUserActionDescription;
	NotifyConfigurationUiHandler *UiHandler;

	QList<Notifier *> Notifiers;
	QList<NotifyEvent *> NotifyEvents;

	NotificationManager();
	virtual ~NotificationManager();

	void createDefaultConfiguration();

private slots:
	void messageReceived(const Message &message);

	void statusChanged(Account account, Buddy buddy, Status oldStatus);

	void notifyAboutUserActionActivated(QAction *sender, bool toggled);

	void groupAdded(Group *group);
	void groupNotifyChanged(Group *group);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual void configurationUpdated();

public:
	static NotificationManager * instance();

	void notify(Notification *notification);

	void registerNotifier(Notifier *notifier);
	void unregisterNotifier(Notifier *notifier);

	void registerNotifyEvent(NotifyEvent *notifyEvent);
	void unregisterNotifyEvent(NotifyEvent *notifyEvent);

	QList<Notifier *> notifiers();
	QList<NotifyEvent *> notifyEvents();

	ConfigurationUiHandler * configurationUiHandler();

signals:
	void notiferRegistered(Notifier *notifier);
	void notiferUnregistered(Notifier *notifier);

	void notifyEventRegistered(NotifyEvent *notifyEvent);
	void notifyEventUnregistered(NotifyEvent *notifyEvent);

};

void checkNotify(Action *);

/** @} */

#endif // NOTIFICATION_MANAGER_H
