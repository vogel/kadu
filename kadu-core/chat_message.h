#ifndef KADU_CHAT_MESSAGE_H
#define KADU_CHAT_MESSAGE_H

#include <qcolor.h>
#include <qdatetime.h>
#include <qmap.h>
#include <qstring.h>

#include "emoticons.h"
#include "gadu.h"
#include "userlistelement.h"

/**

	Klasa przechowuj±ca informacje o wiadomo¶ci, która ma siê pojawiæ
	w oknie Chat.
	\class ChatMessage
	\brief Klasa przechowuj±ca informacje o wiadomo¶ci.
**/
class ChatMessage : public QObject
{
	/**
		\fn QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style)
		Funkcja zamienia specjalne tagi na emotikonki,
		html na czysty tekst itp.
		\param text tekst do konwersji
		\param bgcolor kolor t³a
		\param style styl emotikonek
	**/
	QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style);
	UserListElement Ule;
	QDateTime Date;
	int SeparatorSize;

public:

	QString unformattedMessage;
	QString backgroundColor;
	QString fontColor;
	QString nickColor;
	QString sentDate;
	QString receivedDate;

	static void registerParserTags();
	static void unregisterParserTags();

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
	ChatMessage(const UserListElement &ule, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate = QDateTime());

	void replaceLoadingImages(UinType sender, uint32_t size, uint32_t crc32);

	UserListElement sender() const { return Ule; }
	QDateTime date() const { return Date; }

	void setSeparatorSize(int separatorSize) { SeparatorSize = separatorSize; }
	int separatorSize() const { return SeparatorSize; }
};

#endif
