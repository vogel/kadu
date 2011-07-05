/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "chat-configuration-holder.h"

ChatConfigurationHolder *ChatConfigurationHolder::Instance = 0;

ChatConfigurationHolder * ChatConfigurationHolder::instance()
{
	if (0 == Instance)
		Instance = new ChatConfigurationHolder();
	return Instance;
}

ChatConfigurationHolder::ChatConfigurationHolder()
{
	configurationUpdated();
}

void ChatConfigurationHolder::configurationUpdated()
{
	AutoSend = config_file.readBoolEntry("Chat", "AutoSend");
	NiceDateFormat = config_file.readBoolEntry("Look", "NiceDateFormat");

	ChatTextCustomColors = config_file.readBoolEntry("Look", "ChatTextCustomColors");
	ChatTextBgColor = config_file.readColorEntry("Look", "ChatTextBgColor");
	ChatTextFontColor = config_file.readColorEntry("Look", "ChatTextFontColor");

	ForceCustomChatFont = config_file.readBoolEntry("Look", "ForceCustomChatFont");
	ChatFont = config_file.readFontEntry("Look", "ChatFont");

	EmoticonsStyle = config_file.readNumEntry("Chat", "EmoticonsStyle");

	ChatContents = config_file.readEntry("Look", "ChatContents");
	ConferenceContents = config_file.readEntry("Look", "ConferenceContents");
	ConferencePrefix = config_file.readEntry("Look", "ConferencePrefix");
	MyBackgroundColor = config_file.readEntry("Look", "ChatMyBgColor");
	MyFontColor = config_file.readEntry("Look", "ChatMyFontColor");
	MyNickColor = config_file.readEntry("Look", "ChatMyNickColor");
	UsrBackgroundColor = config_file.readEntry("Look", "ChatUsrBgColor");
	UsrFontColor = config_file.readEntry("Look", "ChatUsrFontColor");
	UsrNickColor = config_file.readEntry("Look", "ChatUsrNickColor");

	ContactStateChats = config_file.readBoolEntry("Chat", "ContactStateChats");
	ContactStateWindowTitle = config_file.readBoolEntry("Chat", "ContactStateWindowTitle");
	ContactStateWindowTitlePosition = config_file.readNumEntry("Chat", "ContactStateWindowTitlePosition");
	ContactStateWindowTitleComposingSyntax = config_file.readEntry("Chat", "ContactStateWindowTitleComposingSyntax");

	ChatBgFilled = config_file.readBoolEntry("Look", "ChatBgFilled");
	ChatBgColor = config_file.readColorEntry("Look", "ChatBgColor");

	UseTransparency = config_file.readBoolEntry("Chat", "UseTransparency");

	
	emit chatConfigurationUpdated();
}
