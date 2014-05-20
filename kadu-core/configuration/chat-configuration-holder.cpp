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
	AutoSend = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Chat", "AutoSend");
	NiceDateFormat = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Look", "NiceDateFormat");

	ChatTextCustomColors = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Look", "ChatTextCustomColors");
	ChatTextBgColor = KaduApplication::instance()->depreceatedConfigurationApi()->readColorEntry("Look", "ChatTextBgColor");
	ChatTextFontColor = KaduApplication::instance()->depreceatedConfigurationApi()->readColorEntry("Look", "ChatTextFontColor");

	ForceCustomChatFont = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Look", "ForceCustomChatFont");
	ChatFont = KaduApplication::instance()->depreceatedConfigurationApi()->readFontEntry("Look", "ChatFont");

	ChatContents = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ChatContents");
	ConferenceContents = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ConferenceContents");
	ConferencePrefix = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ConferencePrefix");
	MyBackgroundColor = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ChatMyBgColor");
	MyFontColor = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ChatMyFontColor");
	MyNickColor = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ChatMyNickColor");
	UsrBackgroundColor = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ChatUsrBgColor");
	UsrFontColor = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ChatUsrFontColor");
	UsrNickColor = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Look", "ChatUsrNickColor");

	ContactStateChats = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Chat", "ContactStateChats");
	ContactStateWindowTitle = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Chat", "ContactStateWindowTitle");
	ContactStateWindowTitlePosition = KaduApplication::instance()->depreceatedConfigurationApi()->readNumEntry("Chat", "ContactStateWindowTitlePosition");
	ContactStateWindowTitleComposingSyntax = KaduApplication::instance()->depreceatedConfigurationApi()->readEntry("Chat", "ContactStateWindowTitleComposingSyntax");

	ChatBgFilled = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Look", "ChatBgFilled");
	ChatBgColor = KaduApplication::instance()->depreceatedConfigurationApi()->readColorEntry("Look", "ChatBgColor");

	UseTransparency = KaduApplication::instance()->depreceatedConfigurationApi()->readBoolEntry("Chat", "UseTransparency");


	emit chatConfigurationUpdated();
}

#include "moc_chat-configuration-holder.cpp"
