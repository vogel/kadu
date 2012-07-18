/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <QtCore/QTimer>

#include "configuration/configuration-aware-object.h"

class QAction;

class Action;
class ActionDescription;
class Notification;
class NotifyConfigurationUiHandler;
class ScreenModeChecker;

class NotificationService : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	bool NotifyAboutAll;
	bool NewMessageOnlyIfInactive;
	bool NotifyIgnoreOnConnection;
	bool IgnoreOnlineToOnline;
	bool SilentMode;
	bool SilentModeWhenDnD;
	bool SilentModeWhenFullscreen;
	bool AutoSilentMode;

	QTimer FullScreenCheckTimer;
	bool IsFullScreen;
	ScreenModeChecker *FullscreenChecker;

	NotifyConfigurationUiHandler *NotifyUiHandler;

	ActionDescription *notifyAboutUserActionDescription;
	ActionDescription *SilentModeActionDescription;


	void createDefaultConfiguration();
	bool ignoreNotifications();
	void createActionDescriptions();

private slots:
	void notifyAboutUserActionActivated(QAction *sender, bool toggled);
	void silentModeActionCreated(Action *action);
	void silentModeActionActivated(QAction *sender, bool toggled);

	void checkFullScreen();

protected:
	virtual void configurationUpdated();

public:
	explicit NotificationService(QObject *parent = 0);
	virtual ~NotificationService();

	bool notifyAboutAll() { return NotifyAboutAll; }
	bool newMessageOnlyIfInactive() { return NewMessageOnlyIfInactive; }

	void setSilentMode(bool silentMode);
	bool silentMode();

	void notify(Notification *notification);

	ActionDescription * silentModeActionDescription() { return SilentModeActionDescription; }

signals:
	void silentModeToggled(bool);
};

#endif // NOTIFICATION_SERVICE_H
