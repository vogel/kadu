/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Copyright 2011 Adam "Vertex" Makświej (vertexbz@gmail.com)
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

#include "docking.h"

#include "docking-configuration-provider.h"
#include "docking-menu-handler.h"
#include "status-notifier-item.h"
#include "status-notifier-item-attention-mode.h"
#include "status-notifier-item-configuration.h"

#include "core/session-service.h"
#include "gui/status-icon.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "message/message.h"
#include "message/unread-message-repository.h"
#include "plugin/plugin-injected-factory.h"
#include "provider/default-provider.h"
#include "status/status-container-manager.h"
#include "activate.h"
#include "attention-service.h"

Docking::Docking(QObject *parent) :
		QObject{parent}
{
}

Docking::~Docking()
{
	if (!m_sessionService->isClosing())
		m_kaduWindowService->kaduWindow()->window()->show();
	m_kaduWindowService->kaduWindow()->setDocked(false);
}

void Docking::setAttentionService(AttentionService *attentionService)
{
	connect(attentionService, SIGNAL(needAttentionChanged(bool)), this, SLOT(needAttentionChanged(bool)));
}

void Docking::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void Docking::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void Docking::setDockingConfigurationProvider(DockingConfigurationProvider *dockingConfigurationProvider)
{
	m_dockingConfigurationProvider = dockingConfigurationProvider;
}

void Docking::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void Docking::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void Docking::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void Docking::setSessionService(SessionService *sessionService)
{
	m_sessionService = sessionService;
}

void Docking::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void Docking::setStatusNotifierItem(StatusNotifierItem *statusNotifierItem)
{
	m_statusNotifierItem = statusNotifierItem;
}

void Docking::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;
}

void Docking::init()
{
	auto statusIcon = m_pluginInjectedFactory->makeOwned<StatusIcon>(m_statusContainerManager, this);
	connect(statusIcon.get(), SIGNAL(iconUpdated(KaduIcon)), this, SLOT(configurationUpdated()));

	connect(m_statusNotifierItem, SIGNAL(activateRequested()), this, SLOT(activateRequested()));
	connect(m_statusNotifierItem, SIGNAL(messageClicked()), this, SIGNAL(messageClicked()));

	connect(m_dockingConfigurationProvider, SIGNAL(updated()), this, SLOT(configurationUpdated()));
	configurationUpdated();

	if (m_dockingConfigurationProvider->configuration().RunDocked)
		m_kaduWindowService->setShowMainWindowOnStart(false);
	m_kaduWindowService->kaduWindow()->setDocked(true);
}

void Docking::needAttentionChanged(bool needAttention)
{
	m_statusNotifierItem->setNeedAttention(needAttention);
}

void Docking::openUnreadMessages()
{
	auto message = m_unreadMessageRepository->unreadMessage();
	m_chatWidgetManager->openChat(message.messageChat(), OpenChatActivation::Activate);
}

void Docking::showMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon, int msecs)
{
	m_statusNotifierItem->showMessage(title, message, icon, msecs);
}

void Docking::activateRequested()
{
	if (m_unreadMessageRepository->hasUnreadMessages())
	{
		openUnreadMessages();
		return;
	}

	auto kaduWindow = m_kaduWindowService->kaduWindow()->window();
	if (kaduWindow->isMinimized() || !kaduWindow->isVisible()
#ifndef Q_OS_WIN
			// NOTE: It won't work as expected on Windows since when you click on tray icon,
			// the tray will become active and any other window will loose focus.
			// See bug #1915.
			|| !_isActiveWindow(kaduWindow)
#endif
			)
		_activateWindow(m_configuration, kaduWindow);
	else
		kaduWindow->hide();
}

void Docking::configurationUpdated()
{
	auto configuration = StatusNotifierItemConfiguration{};
	configuration.AttentionMode = m_dockingConfigurationProvider->configuration().NewMessageIcon;
	configuration.AttentionIcon = KaduIcon{"protocols/common/message"};
	configuration.AttentionMovie = m_iconsManager->iconPath(KaduIcon{"protocols/common/message_anim", "16x16"});
	configuration.Icon = m_statusContainerManager->statusIcon();
	m_statusNotifierItem->setConfiguration(configuration);
}

#include "moc_docking.cpp"
