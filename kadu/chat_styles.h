#ifndef KADU_CHAT_STYLES_H
#define KADU_CHAT_STYLES_H

#include <qstring.h>

/**
	Klasa okre¶laj±ca wygl±d styli
	wykorzystywanych w oknie rozmowy.
	\class ChatStyle
	\brief Styl okna Chat
**/
class ChatStyle
{
	private:
		QString FormatStringFull; /*!< ³añcuch opisuj±cy pe³en styl */
		QString FormatStringPure; /*!< ³añcuch opisuj±cy styl bez nag³ówka */
		QString FormatStringWithoutSeparator; /*!< ³añcuch opisuj±cy styl bez separatora */

	public:
		/**
			Konstruktor stylu okna rozmowy
			\fn ChatStyle(const QString& format_string_full,
				const QString& format_string_pure,
				const QString& format_string_without_separator)
			\param format_string_full pe³ny styl okna
			\param format_string_pure styl bez nag³ówka
			\param format_string_without_separator styl bez separatora
		**/
		ChatStyle(const QString& format_string_full,
			const QString& format_string_pure,
			const QString& format_string_without_separator);

		/**
			\fn const QString& formatStringFull() const
			Zwraca ³añcuch dla pe³nego stylu
		**/
		const QString& formatStringFull() const;

		/**
			\fn const QString& formatStringPure() const
			Zwraca ³añcuch dla stylu bez nag³ówka
		**/
		const QString& formatStringPure() const;

		/**
			\fn const QString& formatStringWithoutSeparator() const
			Zwraca ³añcuch dla stylu bez separatora
		**/
		const QString& formatStringWithoutSeparator() const;

		/**
			Destruktor styli w oknie Chat
			\fn ~ChatStyle()
		**/
		virtual ~ChatStyle();
};

/**
	Klasa opisuj±ca wygl±d stylu Kadu.
	\class KaduChatStyle
	\brief Styl Kadu dla okna Chat
**/
class KaduChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Kadu
			\fn KaduChatStyle()
		**/
		KaduChatStyle();
};

/**
	Klasa opisuj±ca wygl±d stylu Hapi.
	\class HapiChatStyle
	\brief Styl Hapi dla okna Chat
**/
class HapiChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Hapi
			\fn HapiChatStyle()
		**/
		HapiChatStyle();
};

/**
	Klasa opisuj±ca wygl±d stylu Irc.
	\class IrcChatStyle
	\brief Styl Irc dla okna Chat
**/
class IrcChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Irc
			\fn IrcChatStyle()
		**/
		IrcChatStyle();
};

/**
	Klasa opisuj±ca wygl±d stylu stworzonego
	przez u¿ytkownika.
	\class CustomChatStyle
	\brief Styl Custom dla okna Chat
**/
class CustomChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Custom
			\fn CustomChatStyle()
			\param full_style w³asna sk³adnia stylu
		**/
		CustomChatStyle(const QString& full_style);
};

#endif
