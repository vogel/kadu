/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtCore/QList>

class QColor;
class QString;

class MessagePart
{
	QString Content;
	bool Bold;
	bool Italic;
	bool Underline;
	QColor Color;

public:
	MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color);
	virtual ~MessagePart();

	QString content() const { return Content; }
	bool bold() const { return Bold; }
	bool italic() const { return Italic; }
	bool underline() const { return Underline; }
	QColor color() const { return Color; }

	void dump();

};

class Message
{
	static QRegExp ParagraphRegExp;
	static QRegExp SpanRegExp;

	QList<MessagePart> Parts;

public:
	Message();
	virtual ~Message();

	static Message parse(const QString &messageString);

	QList<MessagePart> parts() const;
	void append(MessagePart part);
	Message & operator << (MessagePart part);

	QString toPlain();

	void dump();

};

#endif // MESSAGE_H
