/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-message-handler-configuration.h"

ChatWidgetMessageHandlerConfiguration::ChatWidgetMessageHandlerConfiguration() :
		m_openChatOnMessage{false},
		m_openChatOnMessageOnlyWhenOnline{true},
		m_openChatOnMessageMinimized{false}
{
}

void ChatWidgetMessageHandlerConfiguration::setOpenChatOnMessage(bool openChatOnMessage)
{
	m_openChatOnMessage = openChatOnMessage;
}

bool ChatWidgetMessageHandlerConfiguration::openChatOnMessage() const
{
	return m_openChatOnMessage;
}

void ChatWidgetMessageHandlerConfiguration::setOpenChatOnMessageOnlyWhenOnline(bool openChatOnMessageOnlyWhenOnline)
{
	m_openChatOnMessageOnlyWhenOnline = openChatOnMessageOnlyWhenOnline;
}

bool ChatWidgetMessageHandlerConfiguration::openChatOnMessageOnlyWhenOnline() const
{
	return m_openChatOnMessageOnlyWhenOnline;
}

void ChatWidgetMessageHandlerConfiguration::setOpenChatOnMessageMinimized(bool openChatOnMessageMinimized)
{
	m_openChatOnMessageMinimized = openChatOnMessageMinimized;
}

bool ChatWidgetMessageHandlerConfiguration::openChatOnMessageMinimized() const
{
	return m_openChatOnMessageMinimized;
}
