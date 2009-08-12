/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_CHAT_MESSAGE_H
#define KADU_CHAT_MESSAGE_H

#include <QtCore/QDateTime>
#include <QtCore/QString>

#include "chat/message/message.h"

#include "contacts/contact.h"
#include "contacts/contact-list.h"

#include "protocols/protocol.h"

#include "emoticons.h"

#include "exports.h"

enum ChatMessageType
{
	TypeSystem,
	TypeReceived,
	TypeSent
};

/**

	Klasa przechowuj�ca informacje o wiadomo�ci, kt�ra ma si� pojawi�
	w oknie Chat.

	TODO: optimize and cache
	TODO: make API pretty

	\class ChatMessage
	\brief Klasa przechowuj�ca informacje o wiadomo�ci.
**/
class KADUAPI ChatMessage : public QObject
{
	/**
		\fn QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style)
		Funkcja zamienia specjalne tagi na emotikonki,
		html na czysty tekst itp.
		\param text tekst do konwersji
		\param bgcolor kolor t�a
		\param style styl emotikonek
	**/
	QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style);

	Message message;
	int SeparatorSize;

	ChatMessageType Type;

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
		\fn ChatMessage(const UserListElement &ule, const UserListElements &receivers, const QString &unformattedMessage, ChatMessageType type,
		QDateTime date, QDateTime sdate = QDateTime())
		Konstruktor ustawiaj�cy nick, niesformatowan� wiadomo��,
		czy wiadomo�� pochodzi od nas, dat� wiadomo�ci,
		dat� nadania wiadomo�ci przez serwer
		\param nick nazwa u�ytkownika
		\param unformattedMessage niesformatowana wiadomo��
		\param myMessage zmienna mowi�ca czy wiadomo�� pochodzi od nas
		\param date data otrzymania wiadomo�ci
		\param sdate data wys�ania wiadomo�ci
	**/
	ChatMessage(const Message &msg, ChatMessageType type);

	ChatMessage(const QString &rawContent, ChatMessageType type, QDateTime date,
		QString backgroundColor, QString fontColor, QString nickColor);

	void replaceLoadingImages(const QString &imageId, const QString &imagePath);

	Chat * chat() const { return message.chat(); }
	Contact sender() const { return message.sender(); }

	QDateTime date() const { return message.receiveDate(); }
	QDateTime sdate() const { return message.sendDate(); }
	ChatMessageType type() const { return Type; }

	void setSeparatorSize(int separatorSize) { SeparatorSize = separatorSize; }
	int separatorSize() const { return SeparatorSize; }

	void setShowServerTime(bool noServerTime, int noServerTimeDiff);

	void setColorsAndBackground(QString &backgroundColor, QString &nickColor, QString &fontColor);
};

QString formatMessage(const QString &text, const QString &backgroundColor);

#endif
