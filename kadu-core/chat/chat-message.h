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

class KADUAPI ChatMessage : public QObject
{
	Q_OBJECT

	Message MyMessage;
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

	ChatMessage(const Message &msg, ChatMessageType type);

	ChatMessage(const QString &rawContent, ChatMessageType type, QDateTime date,
		QString backgroundColor, QString fontColor, QString nickColor);

	Message message() { return MyMessage; }

	void replaceLoadingImages(const QString &imageId, const QString &imagePath);

	ChatMessageType type() const { return Type; }

	void setSeparatorSize(int separatorSize) { SeparatorSize = separatorSize; }
	int separatorSize() const { return SeparatorSize; }

	void setShowServerTime(bool noServerTime, int noServerTimeDiff);

	void setColorsAndBackground(QString &backgroundColor, QString &nickColor, QString &fontColor);

signals:
	void statusChanged(Message::Status);

};

QString formatMessage(const QString &text, const QString &backgroundColor);

#endif // KADU_CHAT_MESSAGE_H
