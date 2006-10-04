#ifndef KADU_CHAT_MESSAGE_H
#define KADU_CHAT_MESSAGE_H

#include <qcolor.h>
#include <qdatetime.h>
#include <qmap.h>
#include <qstring.h>

#include "chat_colors.h"
#include "emoticons.h"

class ChatStyle;

/**

	Klasa przechowuj±ca informacje o wiadomo¶ci, która ma siê pojawiæ
	w oknie Chat.
	\class ChatMessage
	\brief Klasa przechowuj±ca informacje o wiadomo¶ci.
**/
class ChatMessage
{
	private:
		/**
			\fn QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style)
			Funkcja zamienia specjalne tagi na emotikonki,
			html na czysty tekst itp.
			\param text tekst do konwersji
			\param bgcolor kolor t³a
			\param style styl emotikonek
		**/
		QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style);


	public:
	QString nick; /*!< nazwa u¿ytkownika */
	QDateTime date; /*!< data otrzymania wiadomo¶ci */
	QDateTime sdate; /*!< data wys³ania wiadomo¶ci */
	QString unformattedMessage; /*!< niesformatowana wiadomo¶æ */
	bool isMyMessage; /*!< zmienna mowi±ca czy wiadomo¶c zosta³a
				napisana przez nas */
	ChatColors Colors; /*!< przechowuje kolory u¿ywane w wiadomo¶ci
				(t³a, czcionki i nicku) */

	QMap<QString, bool> attributes; /*!<
		Mo¿e s³u¿yæ do przechowywania informacji o tym
		czy wiadomo¶æ by³a potwierdzona, szyfrowana, ...
		W zamy¶le ma s³u¿yæ do okre¶lania czy jaki¶ obrazek
		ma byæ dodawany do wiadomo¶ci czy nie - jaki obrazek -
		ta informacja bêdzie gdzie indziej
		*/

	//inne atrybuty?
	//QMap<QString, QString> stringAttributes;

	bool needsToBeFormatted; /*!< zmienna mowi±ca czy wiadomo¶c powinna
				  byæ sformatowana */

	QString message; /*!< Sformatowana wiadomo¶æ (razem z \<p\> lub \<table\>) **/

	/**
		\fn ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime())
		Konstruktor ustawiaj±cy nick, niesformatowan± wiadomo¶æ,
		czy wiadomo¶æ pochodzi od nas, datê wiadomo¶ci,
		datê nadania wiadomo¶ci przez serwer
		\param nick nazwa u¿ytkownika
		\param unformattedMessage niesformatowana wiadomo¶æ
		\param myMessage zmienna mowi±ca czy wiadomo¶æ pochodzi od nas
		\param date data otrzymania wiadomo¶ci
		\param sdate data wys³ania wiadomo¶ci
	**/
	ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime());

	/**
		\fn ChatMessage(const QString &formattedMessage, const ChatColors& colors = ChatColors(Qt::white, Qt::black, Qt::black))
		Konstruktor ustawiaj±cy sformatowan± wiadomo¶æ,
		kolor t³a i liter wiadomo¶ci
		\param nick nazwa u¿ytkownika
		\param unformattedMessage niesformatowana wiadomo¶æ
		\param myMessage zmienna mowi±ca czy wiadomo¶æ pochodzi od nas
		\param date data otrzymania wiadomo¶ci
		\param sdate data wys³ania wiadomo¶ci
	**/
	ChatMessage(const QString &formattedMessage,
		const ChatColors& colors = ChatColors(Qt::white, Qt::black, Qt::black));

	/**
		\fn void formatMessage(const ChatStyle* chat_style, const EmoticonsStyle emoticons_style,
			bool include_header, int separator_size);
		Funkcja formatuj±ca niesformatowan± wiadomo¶æ
		zgodnie z zadanymi parametrami
		\param chat_style styl dla okna, który powinien zostaæ u¿yty
		\param emoticons_style u¿ywany styl emotikon
		\param include_header czy powinien byæ do³±czony nag³ówek
		\param separator_size wysoko¶æ separatora
	**/
	void formatMessage(const ChatStyle* chat_style, const EmoticonsStyle emoticons_style,
		bool include_header, int separator_size);
};

#endif
