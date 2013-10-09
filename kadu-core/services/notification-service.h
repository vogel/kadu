/*
 * %kadu copyright begin%
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "exports.h"

class QAction;

class AccountEventListener;
class Action;
class ActionDescription;
class ChatEventListener;
class GroupEventListener;
class Notification;
class NotifyConfigurationUiHandler;
class ScreenModeChecker;
class StatusContainer;

class KADUAPI NotificationService : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

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
	explicit NotificationService(QObject *parent = 0);
	virtual ~NotificationService();

	bool notifyIgnoreOnConnection() { return NotifyIgnoreOnConnection; }
	bool ignoreOnlineToOnline() { return IgnoreOnlineToOnline; }
	bool newMessageOnlyIfInactive() { return NewMessageOnlyIfInactive; }
	void setSilentMode(bool silentMode);
	bool silentMode();

	void notify(Notification *notification);

signals:
	void silentModeToggled(bool);
};

void checkNotify(Action *);

#endif // NOTIFICATION_SERVICE_H
