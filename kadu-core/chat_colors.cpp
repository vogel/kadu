/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_colors.h"

#include "config_file.h"

ChatColors::ChatColors(const QColor& background_color,
			const QColor& font_color,
			const QColor& nick_color)
	: BackgroundColor(background_color), FontColor(font_color), NickColor(nick_color)
{
}

const QColor& ChatColors::backgroundColor() const
{
	return BackgroundColor;
}

const QColor& ChatColors::fontColor() const
{
	return FontColor;
}

const QColor& ChatColors::nickColor() const
{
	return NickColor;
}

OwnChatColors::OwnChatColors()
	: ChatColors(
		config_file.readColorEntry("Look", "ChatMyBgColor"),
		config_file.readColorEntry("Look", "ChatMyFontColor"),
		config_file.readColorEntry("Look", "ChatMyNickColor"))
{
}

UserChatColors::UserChatColors()
	: ChatColors(
		config_file.readColorEntry("Look", "ChatUsrBgColor"),
		config_file.readColorEntry("Look", "ChatUsrFontColor"),
		config_file.readColorEntry("Look", "ChatUsrNickColor"))
{
}
