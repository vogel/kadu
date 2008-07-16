/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QRegExp>
#include <QtGui/QColor>

#include "message.h"

QRegExp Message::ParagraphRegExp("<p[^>]*>(.*)</p>");
QRegExp Message::SpanRegExp("<span.*(font-weight:600)?.*(font-style:italic)?.*(text-decoration: underline)?.*(color:(#[0-9a-fA-F]+))?.*>(.*)</span>");

MessagePart::MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color)
	: Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color)
{
}

MessagePart::~MessagePart()
{
}

void MessagePart::dump()
{
	printf("content: [%s], biu:%d%d%d color: %s\n", Content.toLocal8Bit().data(), Bold, Italic, Underline, Color.name().toLocal8Bit().data());
}

void Message::dump()
{
	foreach (MessagePart part, Parts)
		part.dump();
}

Message Message::parse(const QString &messageString)
{
	Message result;

	QString workingMessage = messageString;
	QString cleanedMessage;
	QString partContent; 

	// TODO: move
	ParagraphRegExp.setMinimal(true);
	SpanRegExp.setMinimal(true);

	int lastPos = 0;
	int pos = 0;

	while ((pos = ParagraphRegExp.indexIn(workingMessage, pos)) != -1) {
		if (!cleanedMessage.isEmpty())
			cleanedMessage.append("\n");
		cleanedMessage.append(ParagraphRegExp.cap(1));
		pos += ParagraphRegExp.matchedLength();
	}

	pos = 0;

	while ((pos = SpanRegExp.indexIn(cleanedMessage, pos)) != -1) {
		if (lastPos != pos)
		{
			partContent = cleanedMessage.mid(lastPos, pos - lastPos);
			result << MessagePart(partContent, false, false, false, QColor());
		}

		bool b = !SpanRegExp.cap(1).isEmpty();
		bool i = !SpanRegExp.cap(2).isEmpty();
		bool u = !SpanRegExp.cap(3).isEmpty();
		QColor color = QColor(SpanRegExp.cap(5));

		partContent = SpanRegExp.cap(6);
		result << MessagePart(partContent, b, i, u, color);

		pos += SpanRegExp.matchedLength();
		lastPos = pos;
	}

	if (lastPos != cleanedMessage.length())
	{
		partContent = cleanedMessage.mid(lastPos, cleanedMessage.length() - lastPos);
		result << MessagePart(partContent, false, false, false, QColor());
	}

	return result;
}

Message::Message()
{
}

Message::~Message()
{
}

QList<MessagePart> Message::parts() const
{
	return Parts;
}


void Message::append(MessagePart part)
{
	Parts.append(part);
}

Message & Message::operator << (MessagePart part)
{
	Parts << part;
	return *this;
}

QString Message::toPlain()
{
	QString result;
	foreach (MessagePart part, Parts)
		result += part.content();

	return result;
}
