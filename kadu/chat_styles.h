#ifndef KADU_CHAT_STYLES_H
#define KADU_CHAT_STYLES_H

#include <qstring.h>

class ChatStyle
{
	private:
		QString FormatStringFull;
		QString FormatStringPure;
		QString FormatStringWithoutSeparator;

	public:
		ChatStyle(const QString& format_string_full,
			const QString& format_string_pure,
			const QString& format_string_without_separator);
		const QString& formatStringFull();
		const QString& formatStringPure();
		const QString& formatStringWithoutSeparator();
};

class KaduChatStyle : public ChatStyle
{
	public:
		KaduChatStyle();
};

class HapiChatStyle : public ChatStyle
{
	public:
		HapiChatStyle();
};

class IrcChatStyle : public ChatStyle
{
	public:
		IrcChatStyle();
};

class CustomChatStyle : public ChatStyle
{
	public:
		CustomChatStyle(const QString& full_style);
};

#endif
