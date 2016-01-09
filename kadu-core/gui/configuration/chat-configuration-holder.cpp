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
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-title-composing-state-position.h"

#include "chat-configuration-holder.h"

ChatConfigurationHolder *ChatConfigurationHolder::Instance = 0;

ChatConfigurationHolder * ChatConfigurationHolder::instance()
{
	if (!Instance)
	{
		Instance = new ChatConfigurationHolder();
		Instance->init();
	}

	return Instance;
}

ChatConfigurationHolder::ChatConfigurationHolder() :
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

void ChatConfigurationHolder::init()
{
	configurationUpdated();
}

void ChatConfigurationHolder::configurationUpdated()
{
	AutoSend = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "AutoSend");
	NiceDateFormat = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "NiceDateFormat");

	ChatTextCustomColors = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ChatTextCustomColors");
	ChatTextBgColor = Core::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor");
	ChatTextFontColor = Core::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextFontColor");

	ForceCustomChatFont = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ForceCustomChatFont");
	ChatFont = Core::instance()->configuration()->deprecatedApi()->readFontEntry("Look", "ChatFont");

	MyBackgroundColor = Core::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatMyBgColor");
	MyFontColor = Core::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatMyFontColor");
	MyNickColor = Core::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatMyNickColor");
	UsrBackgroundColor = Core::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatUsrBgColor");
	UsrFontColor = Core::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatUsrFontColor");
	UsrNickColor = Core::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatUsrNickColor");

	ContactStateChats = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "ContactStateChats");
	ContactStateWindowTitle = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "ContactStateWindowTitle");
	ContactStateWindowTitlePosition = Core::instance()->configuration()->deprecatedApi()->readNumEntry("Chat", "ContactStateWindowTitlePosition");

	ChatBgFilled = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ChatBgFilled");
	ChatBgColor = Core::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatBgColor");

	UseTransparency = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "UseTransparency");

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
