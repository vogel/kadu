#ifndef KADU_CHAT_COLORS_H
#define KADU_CHAT_COLORS_H

#include <qcolor.h>

/**
	Klasa opisuj±ca kolory wykorzystywane
	w oknie rozmowy.
	\class ChatColors
	\brief Kolory w oknie Chat
**/
class ChatColors
{
	private:
		QColor BackgroundColor; /*!< kolor t³a */
		QColor FontColor; /*!< kolor czcionki */
		QColor NickColor; /*!< kolor nicku */

	public:
		/**
			Konstruktor kolorów dla okna Chat
			\fn ChatColors(const QColor& background_color,
				const QColor& font_color,
				const QColor& nick_color)
			\param background_color kolor t³a
			\param font_color kolor czcionki
			\param nick_color kolor nicku
		**/
		ChatColors(const QColor& background_color,
			const QColor& font_color,
			const QColor& nick_color);

		/**
			\fn const QColor& backgroundColor() const
			Zwraca kolor t³a
		**/
		const QColor& backgroundColor() const;

		/**
			\fn const QColor& fontColor() const
			Zwraca kolor czcionki
		**/
		const QColor& fontColor() const;

		/**
			\fn const QColor& nickColor() const
			Zwraca kolor nicku
		**/
		const QColor& nickColor() const;
};

/**
	Klasa opisuj±ca kolory w³asne u¿ytkownika
	\class OwnChatColors
	\brief Kolory w³asne
**/
class OwnChatColors : public ChatColors
{
	public:
		/**
			\fn UserChatColors()
			Konstruktor kolorów w³asnych
		**/
		OwnChatColors();
};

/**
	Klasa opisuj±ca kolory rozmówcy
	\class UserChatColors
	\brief Kolory rozmówcy
**/
class UserChatColors : public ChatColors
{
	public:
		/**
			\fn UserChatColors()
			Konstruktor kolorów rozmówcy
		**/
		UserChatColors();
};

#endif
