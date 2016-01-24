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

class Actions;
class ActionDescription;
class Action;
class ChatWidgetManager;
class ConfigurationUiHandlerRepository;
class Configuration;
class MenuInventory;
class MessageManager;
class NotificationManager;
class NotificationCallbackRepository;
class NotificationEventRepository;
class Notification;
class NotifyConfigurationUiHandler;
class Parser;
class ScreenModeChecker;
class StatusContainerManager;
class StatusContainer;
class WindowNotifier;

class KADUAPI NotificationService : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<Actions> m_actions;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<Configuration> m_configuration;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<MessageManager> m_messageManager;
	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<NotificationManager> m_notificationManager;
	QPointer<NotifyConfigurationUiHandler> m_notifyConfigurationUiHandler;
	QPointer<Parser> m_parser;
	QPointer<StatusContainerManager> m_statusContainerManager;
	QPointer<WindowNotifier> m_windowNotifier;

	bool NewMessageOnlyIfInactive;
	bool NotifyIgnoreOnConnection;
	bool IgnoreOnlineToOnline;
	bool SilentMode;
	bool SilentModeWhenDnD;
	bool SilentModeWhenFullscreen;
	bool AutoSilentMode;

	bool IsFullScreen;
	ScreenModeChecker *FullscreenChecker;

	ActionDescription *notifyAboutUserActionDescription;
	ActionDescription *SilentModeActionDescription;

	void createDefaultConfiguration();
	bool ignoreNotifications();
	void createActionDescriptions();

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
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_SET void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);
	INJEQT_SET void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SET void setNotificationManager(NotificationManager *notificationManager);
	INJEQT_SET void setNotifyConfigurationUiHandler(NotifyConfigurationUiHandler *notifyConfigurationUiHandler);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_SET void setWindowNotifier(WindowNotifier *windowNotifier);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};

void checkNotify(Action *);
