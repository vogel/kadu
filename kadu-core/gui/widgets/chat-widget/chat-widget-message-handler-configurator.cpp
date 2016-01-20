/*
 * %kadu copyright begin%
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

#include "chat-widget-message-handler-configurator.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-message-handler-configuration.h"
#include "gui/widgets/chat-widget/chat-widget-message-handler.h"

ChatWidgetMessageHandlerConfigurator::ChatWidgetMessageHandlerConfigurator()
{
	createDefaultConfiguration();
}

void ChatWidgetMessageHandlerConfigurator::setChatWidgetMessageHandler(ChatWidgetMessageHandler *chatWidgetMessageHandler)
{
	m_chatWidgetMessageHandler = chatWidgetMessageHandler;
	configurationUpdated();
}

void ChatWidgetMessageHandlerConfigurator::configurationUpdated()
{
	if (m_chatWidgetMessageHandler)
		m_chatWidgetMessageHandler.data()->setChatWidgetMessageHandlerConfiguration(loadConfiguration());
}

void ChatWidgetMessageHandlerConfigurator::createDefaultConfiguration() const
{
	Core::instance()->configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessage", false);
	Core::instance()->configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageWhenOnline", true);
	Core::instance()->configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageMinimized", false);
}

ChatWidgetMessageHandlerConfiguration ChatWidgetMessageHandlerConfigurator::loadConfiguration() const
{
	auto configuration = ChatWidgetMessageHandlerConfiguration();
	configuration.setOpenChatOnMessage(Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "OpenChatOnMessage", false));
	configuration.setOpenChatOnMessageOnlyWhenOnline(Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "OpenChatOnMessageWhenOnline", true));
	configuration.setOpenChatOnMessageMinimized(Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "OpenChatOnMessageMinimized", false));
	return configuration;
}
