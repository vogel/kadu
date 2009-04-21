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
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "protocols/status.h"

class ActionDescription;
class KaduAction;

class ConfigurationUiHandler;
class Notification;
class Notifier;
class NotifyConfigurationUiHandler;
class NotifyEvent;

/**
 * @defgroup notify Notify
 * @{
 */

class KADUAPI NotificationManager : public QObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(NotificationManager)

	static NotificationManager *Instance;

	ActionDescription *notifyAboutUserActionDescription;
	NotifyConfigurationUiHandler *UiHandler;

	QList<Notifier *> Notifiers;
	QList<NotifyEvent *> NotifyEvents;

	NotificationManager();
	virtual ~NotificationManager();

	void createDefaultConfiguration();

private slots:
	void messageReceived(Chat *chat, Contact sender, const QString &message);

	void statusChanged(Account *account, Contact contact, Status oldStatus);

	void notifyAboutUserActionActivated(QAction *sender, bool toggled);

protected:
	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

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

	void notifyEventRegisterd(NotifyEvent *notifyEvent);
	void notifyEventUnregistered(NotifyEvent *notifyEvent);

};

void checkNotify(KaduAction *);

/** @} */

#endif // NOTIFICATION_MANAGER_H
