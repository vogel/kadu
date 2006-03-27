#ifndef KADU_CHAT_COLORS_H
#define KADU_CHAT_COLORS_H

#include <qcolor.h>

class ChatColors
{
	private:
		QColor BackgroundColor;
		QColor FontColor;
		QColor NickColor;

	public:
		ChatColors(const QColor& background_color,
			const QColor& font_color,
			const QColor& nick_color);
		const QColor& backgroundColor() const;
		const QColor& fontColor() const;
		const QColor& nickColor() const;
};

class OwnChatColors : public ChatColors
{
	public:
		OwnChatColors();
};

class UserChatColors : public ChatColors
{
	public:
		UserChatColors();
};

#endif
