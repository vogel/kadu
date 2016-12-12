/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "widgets/chat-widget/chat-widget-title-composing-state-position.h"

#include "chat-configuration-holder.h"

ChatConfigurationHolder::ChatConfigurationHolder(QObject *parent) :
		ConfigurationHolder{parent},
		AutoSend{},
		NiceDateFormat{},
		ChatTextCustomColors{},
		ForceCustomChatFont{},
		ChatBgFilled{},
		UseTransparency{},
		ContactStateChats{},
		ContactStateWindowTitle{},
		ContactStateWindowTitlePosition{}
{
}

ChatConfigurationHolder::~ChatConfigurationHolder()
{
}

void ChatConfigurationHolder::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ChatConfigurationHolder::init()
{
	configurationUpdated();
}

void ChatConfigurationHolder::configurationUpdated()
{
	AutoSend = m_configuration->deprecatedApi()->readBoolEntry("Chat", "AutoSend");
	NiceDateFormat = m_configuration->deprecatedApi()->readBoolEntry("Look", "NiceDateFormat");

	ChatTextCustomColors = m_configuration->deprecatedApi()->readBoolEntry("Look", "ChatTextCustomColors");
	ChatTextBgColor = m_configuration->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor");
	ChatTextFontColor = m_configuration->deprecatedApi()->readColorEntry("Look", "ChatTextFontColor");

	ForceCustomChatFont = m_configuration->deprecatedApi()->readBoolEntry("Look", "ForceCustomChatFont");
	ChatFont = m_configuration->deprecatedApi()->readFontEntry("Look", "ChatFont");

	MyBackgroundColor = m_configuration->deprecatedApi()->readEntry("Look", "ChatMyBgColor");
	MyFontColor = m_configuration->deprecatedApi()->readEntry("Look", "ChatMyFontColor");
	MyNickColor = m_configuration->deprecatedApi()->readEntry("Look", "ChatMyNickColor");
	UsrBackgroundColor = m_configuration->deprecatedApi()->readEntry("Look", "ChatUsrBgColor");
	UsrFontColor = m_configuration->deprecatedApi()->readEntry("Look", "ChatUsrFontColor");
	UsrNickColor = m_configuration->deprecatedApi()->readEntry("Look", "ChatUsrNickColor");

	ContactStateChats = m_configuration->deprecatedApi()->readBoolEntry("Chat", "ContactStateChats");
	ContactStateWindowTitle = m_configuration->deprecatedApi()->readBoolEntry("Chat", "ContactStateWindowTitle");
	ContactStateWindowTitlePosition = m_configuration->deprecatedApi()->readNumEntry("Chat", "ContactStateWindowTitlePosition");

	ChatBgFilled = m_configuration->deprecatedApi()->readBoolEntry("Look", "ChatBgFilled");
	ChatBgColor = m_configuration->deprecatedApi()->readColorEntry("Look", "ChatBgColor");

	UseTransparency = m_configuration->deprecatedApi()->readBoolEntry("Chat", "UseTransparency");

	emit chatConfigurationUpdated();
}

ChatWidgetTitleComposingStatePosition ChatConfigurationHolder::composingStatePosition() const
{
	if (!ContactStateWindowTitle)
		return ChatWidgetTitleComposingStatePosition::None;
	if (ContactStateWindowTitlePosition == 0)
		return ChatWidgetTitleComposingStatePosition::AtBegining;
	return ChatWidgetTitleComposingStatePosition::AtEnd;
}

#include "moc_chat-configuration-holder.cpp"
