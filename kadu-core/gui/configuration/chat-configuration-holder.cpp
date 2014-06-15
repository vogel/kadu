/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "core/application.h"

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
	AutoSend = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "AutoSend");
	NiceDateFormat = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "NiceDateFormat");

	ChatTextCustomColors = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ChatTextCustomColors");
	ChatTextBgColor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor");
	ChatTextFontColor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextFontColor");

	ForceCustomChatFont = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ForceCustomChatFont");
	ChatFont = Application::instance()->configuration()->deprecatedApi()->readFontEntry("Look", "ChatFont");

	ChatContents = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatContents");
	ConferenceContents = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ConferenceContents");
	ConferencePrefix = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ConferencePrefix");
	MyBackgroundColor = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatMyBgColor");
	MyFontColor = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatMyFontColor");
	MyNickColor = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatMyNickColor");
	UsrBackgroundColor = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatUsrBgColor");
	UsrFontColor = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatUsrFontColor");
	UsrNickColor = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatUsrNickColor");

	ContactStateChats = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "ContactStateChats");
	ContactStateWindowTitle = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "ContactStateWindowTitle");
	ContactStateWindowTitlePosition = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Chat", "ContactStateWindowTitlePosition");
	ContactStateWindowTitleComposingSyntax = Application::instance()->configuration()->deprecatedApi()->readEntry("Chat", "ContactStateWindowTitleComposingSyntax");

	ChatBgFilled = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ChatBgFilled");
	ChatBgColor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatBgColor");

	UseTransparency = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "UseTransparency");


	emit chatConfigurationUpdated();
}

#include "moc_chat-configuration-holder.cpp"
