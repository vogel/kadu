/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QtCore/QTimer>
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
class MultilogonSession;
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
	bool SilentMode;
	bool SilentModeWhenDnD;
	bool SilentModeWhenFullscreen;
	bool AutoSilentMode;

	ActionDescription *notifyAboutUserActionDescription;
	ActionDescription *SilentModeActionDescription;

	NotifyConfigurationUiHandler *UiHandler;

	QList<Notifier *> Notifiers;
	QList<NotifyEvent *> NotifyEvents;

	QTimer FullScreenCheckTimer;
	bool IsFullScreen;

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	Display *x11display;
#endif

	NotificationManager();
	virtual ~NotificationManager();

	void createDefaultConfiguration();

	bool ignoreNotifications();

private slots:
	void messageReceived(const Message &message);
	void multilogonSessionConnected(MultilogonSession *session);
	void multilogonSessionDisconnected(MultilogonSession *session);

	void statusChanged();
	void contactStatusChanged(Contact contact, Status oldStatus);

	void notifyAboutUserActionActivated(QAction *sender, bool toggled);
	void silentModeActionCreated(Action *action);
	void silentModeActionActivated(QAction *sender, bool toggled);

	void groupAdded(const Group &group);
	void groupUpdated();

	void accountConnected();

	void checkFullScreen();
	bool isScreenSaverRunning();

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

	const QList<Notifier *> & notifiers() const;
	const QList<NotifyEvent *> & notifyEvents() const;

	bool notifyAboutAll() { return NotifyAboutAll; }

	void setSilentMode(bool silentMode);
	bool silentMode();

	QString notifyConfigurationKey(const QString &eventType);

	ConfigurationUiHandler * configurationUiHandler();

	ActionDescription * silentModeActionDescription() { return SilentModeActionDescription; }

signals:
	void notiferRegistered(Notifier *notifier);
	void notiferUnregistered(Notifier *notifier);

	void notifyEventRegistered(NotifyEvent *notifyEvent);
	void notifyEventUnregistered(NotifyEvent *notifyEvent);

	void silentModeToggled(bool);

};

void checkNotify(Action *);

/** @} */

#endif // NOTIFICATION_MANAGER_H
