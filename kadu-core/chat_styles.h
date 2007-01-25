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

		bool noServerTime; /*!< czy czas serwera powinien byæ usuwany */
		int noServerTimeDiff; /*!< maksymalna ró¿nica czasu dla której usuwamy */

	public:
		/**
			Konstruktor stylu okna rozmowy
			\fn ChatStyle(const QString& format_string_full,
				const QString& format_string_pure)
			\param format_string_full pe³ny styl okna
			\param format_string_pure styl bez nag³ówka
		**/
		ChatStyle(const QString& format_string_full,
			const QString& format_string_pure);

		/**
			Konstruktor stylu okna rozmowy
			\fn ChatStyle(const QString& format_string_full)
			\param format_string_full pe³ny styl okna
		**/
		ChatStyle(const QString& format_string_full);

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
			\fn const bool noServerTimes() const
			Zwraca warto¶æ okre¶laj±c±, czy czas serwera powinien
			byæ usuwany dla wiadomo¶ci przychodz±cych.
		**/
		const bool noServerTimes() const;

		/**
			\fn const int noServerTimesDiff() const
			Zwraca maksymaln± ró¿nicê czasu, dla której czas
			serwera powinien byæ pomijany.
		**/
		const int noServerTimesDiff() const;

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
