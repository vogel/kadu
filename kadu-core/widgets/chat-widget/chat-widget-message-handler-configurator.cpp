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
#include "chat-widget-message-handler-configurator.moc"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "widgets/chat-widget/chat-widget-message-handler-configuration.h"
#include "widgets/chat-widget/chat-widget-message-handler.h"

ChatWidgetMessageHandlerConfigurator::ChatWidgetMessageHandlerConfigurator(QObject *parent) : QObject{parent}
{
}

ChatWidgetMessageHandlerConfigurator::~ChatWidgetMessageHandlerConfigurator()
{
}

void ChatWidgetMessageHandlerConfigurator::setChatWidgetMessageHandler(
    ChatWidgetMessageHandler *chatWidgetMessageHandler)
{
    m_chatWidgetMessageHandler = chatWidgetMessageHandler;
}

void ChatWidgetMessageHandlerConfigurator::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ChatWidgetMessageHandlerConfigurator::init()
{
    createDefaultConfiguration();
}

void ChatWidgetMessageHandlerConfigurator::configurationUpdated()
{
    if (m_chatWidgetMessageHandler)
        m_chatWidgetMessageHandler.data()->setChatWidgetMessageHandlerConfiguration(loadConfiguration());
}

void ChatWidgetMessageHandlerConfigurator::createDefaultConfiguration() const
{
    m_configuration->deprecatedApi()->addVariable("Chat", "OpenChatOnMessage", false);
    m_configuration->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageWhenOnline", true);
    m_configuration->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageMinimized", false);
}

ChatWidgetMessageHandlerConfiguration ChatWidgetMessageHandlerConfigurator::loadConfiguration() const
{
    auto configuration = ChatWidgetMessageHandlerConfiguration();
    configuration.setOpenChatOnMessage(
        m_configuration->deprecatedApi()->readBoolEntry("Chat", "OpenChatOnMessage", false));
    configuration.setOpenChatOnMessageOnlyWhenOnline(
        m_configuration->deprecatedApi()->readBoolEntry("Chat", "OpenChatOnMessageWhenOnline", true));
    configuration.setOpenChatOnMessageMinimized(
        m_configuration->deprecatedApi()->readBoolEntry("Chat", "OpenChatOnMessageMinimized", false));
    return configuration;
}
