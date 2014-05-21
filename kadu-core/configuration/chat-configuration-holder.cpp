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

#include "configuration-file.h"
#include "kadu-application.h"

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
	AutoSend = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Chat", "AutoSend");
	NiceDateFormat = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Look", "NiceDateFormat");

	ChatTextCustomColors = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Look", "ChatTextCustomColors");
	ChatTextBgColor = KaduApplication::instance()->deprecatedConfigurationApi()->readColorEntry("Look", "ChatTextBgColor");
	ChatTextFontColor = KaduApplication::instance()->deprecatedConfigurationApi()->readColorEntry("Look", "ChatTextFontColor");

	ForceCustomChatFont = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Look", "ForceCustomChatFont");
	ChatFont = KaduApplication::instance()->deprecatedConfigurationApi()->readFontEntry("Look", "ChatFont");

	ChatContents = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ChatContents");
	ConferenceContents = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ConferenceContents");
	ConferencePrefix = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ConferencePrefix");
	MyBackgroundColor = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ChatMyBgColor");
	MyFontColor = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ChatMyFontColor");
	MyNickColor = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ChatMyNickColor");
	UsrBackgroundColor = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ChatUsrBgColor");
	UsrFontColor = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ChatUsrFontColor");
	UsrNickColor = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Look", "ChatUsrNickColor");

	ContactStateChats = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Chat", "ContactStateChats");
	ContactStateWindowTitle = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Chat", "ContactStateWindowTitle");
	ContactStateWindowTitlePosition = KaduApplication::instance()->deprecatedConfigurationApi()->readNumEntry("Chat", "ContactStateWindowTitlePosition");
	ContactStateWindowTitleComposingSyntax = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("Chat", "ContactStateWindowTitleComposingSyntax");

	ChatBgFilled = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Look", "ChatBgFilled");
	ChatBgColor = KaduApplication::instance()->deprecatedConfigurationApi()->readColorEntry("Look", "ChatBgColor");

	UseTransparency = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Chat", "UseTransparency");


	emit chatConfigurationUpdated();
}

#include "moc_chat-configuration-holder.cpp"
