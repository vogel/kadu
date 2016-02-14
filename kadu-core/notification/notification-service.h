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

#include "exports.h"

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>

class QAction;

class ActionDescription;
class Action;
class ChatWidgetManager;
class ConfigurationUiHandlerRepository;
class Configuration;
class InjectedFactory;
class MenuInventory;
class NotificationCallbackRepository;
class NotificationDispatcher;
struct Notification;
class NotifierRepository;
class NotifyConfigurationUiHandler;
class Parser;
class SilentModeService;
class StatusContainer;
class WindowNotifier;

class KADUAPI NotificationService : public QObject
{
	Q_OBJECT

	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;
	QPointer<NotificationDispatcher> m_notificationDispatcher;
	QPointer<NotifierRepository> m_notifierRepository;
	QPointer<NotifyConfigurationUiHandler> m_notifyConfigurationUiHandler;
	QPointer<Parser> m_parser;
	QPointer<SilentModeService> m_silentModeService;
	QPointer<WindowNotifier> m_windowNotifier;

	ActionDescription *m_notifyAboutUserActionDescription;

	void createDefaultConfiguration();
	void createActionDescriptions();

	bool silentMode() const;
	void setSilentMode(bool silentMode);

public:
	Q_INVOKABLE explicit NotificationService(QObject *parent = nullptr);
	virtual ~NotificationService();

	void notify(const Notification &notification);
	void acceptNotification(const Notification &notification);
	void discardNotification(const Notification &notification);

private slots:
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);
	INJEQT_SET void setNotificationDispatcher(NotificationDispatcher *notificationDispatcher);
	INJEQT_SET void setNotifierRepository(NotifierRepository *notifierRepository);
	INJEQT_SET void setNotifyConfigurationUiHandler(NotifyConfigurationUiHandler *notifyConfigurationUiHandler);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_SET void setSilentModeService(SilentModeService *silentModeService);
	INJEQT_SET void setWindowNotifier(WindowNotifier *windowNotifier);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void notifyAboutUserActionActivated(QAction *sender, bool toggled);

};

void checkNotify(Action *);
