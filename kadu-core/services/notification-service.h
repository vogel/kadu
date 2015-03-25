/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>

class QAction;

class AccountEventListener;
class Action;
class ActionDescription;
class ChatEventListener;
class GroupEventListener;
class Notification;
class NotificationCallbackRepository;
class NotifyConfigurationUiHandler;
class ScreenModeChecker;
class StatusContainer;
class WindowNotifier;

class KADUAPI NotificationService : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;

	bool NewMessageOnlyIfInactive;
	bool NotifyIgnoreOnConnection;
	bool IgnoreOnlineToOnline;
	bool SilentMode;
	bool SilentModeWhenDnD;
	bool SilentModeWhenFullscreen;
	bool AutoSilentMode;

	bool IsFullScreen;
	ScreenModeChecker *FullscreenChecker;

	NotifyConfigurationUiHandler *NotifyUiHandler;

	ActionDescription *notifyAboutUserActionDescription;
	ActionDescription *SilentModeActionDescription;

	ChatEventListener *ChatListener;
	AccountEventListener *AccountListener;
	GroupEventListener *GroupListener;
	WindowNotifier *CurrentWindowNotifier;

	void createDefaultConfiguration();
	bool ignoreNotifications();
	void createActionDescriptions();
	void createEventListeners();

private slots:
	void notifyAboutUserActionActivated(QAction *sender, bool toggled);
	void silentModeActionCreated(Action *action);
	void silentModeActionActivated(QAction *sender, bool toggled);
	void statusUpdated(StatusContainer *container);
	void fullscreenToggled(bool inFullscreen);
	void startScreenModeChecker();
	void stopScreenModeChecker();

protected:
	virtual void configurationUpdated();

public:
	Q_INVOKABLE explicit NotificationService(QObject *parent = nullptr);
	virtual ~NotificationService();

	bool notifyIgnoreOnConnection() { return NotifyIgnoreOnConnection; }
	bool ignoreOnlineToOnline() { return IgnoreOnlineToOnline; }
	bool newMessageOnlyIfInactive() { return NewMessageOnlyIfInactive; }
	void setSilentMode(bool silentMode);
	bool silentMode();

	void notify(Notification *notification);

signals:
	void silentModeToggled(bool);

private slots:
	INJEQT_SETTER void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);

};

void checkNotify(Action *);
