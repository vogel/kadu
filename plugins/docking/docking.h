/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking-exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QSystemTrayIcon>
#include <injeqt/injeqt.h>

class AttentionService;
class ChatWidgetManager;
class Configuration;
class DockingConfigurationProvider;
class IconsManager;
class PluginInjectedFactory;
class KaduWindowService;
class SessionService;
class StatusContainerManager;
class StatusNotifierItem;
class UnreadMessageRepository;

class DOCKINGAPI Docking final : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE Docking(QObject *parent = nullptr);
	virtual ~Docking();

	void showMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon, int msecs);

signals:
	void messageClicked();

private:
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<Configuration> m_configuration;
	QPointer<DockingConfigurationProvider> m_dockingConfigurationProvider;
	QPointer<IconsManager> m_iconsManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<KaduWindowService> m_kaduWindowService;
	QPointer<SessionService> m_sessionService;
	QPointer<StatusContainerManager> m_statusContainerManager;
	QPointer<StatusNotifierItem> m_statusNotifierItem;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

	void openUnreadMessages();

private slots:
	INJEQT_SET void setAttentionService(AttentionService *attentionService);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setDockingConfigurationProvider(DockingConfigurationProvider *dockingConfigurationProvider);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
	INJEQT_SET void setSessionService(SessionService *sessionService);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_SET void setStatusNotifierItem(StatusNotifierItem *statusNotifierItem);
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);
	INJEQT_INIT void init();

	void configurationUpdated();
	void needAttentionChanged(bool needAttention);
	void activateRequested();

};
