/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTextDocument>

#include "html_document.h"

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

QString MessagePart::toHtml() const
{
	QString result = Qt::escape(Content);
	result.replace("\n", "<br />");

	if (!Bold && !Italic && !Underline && !Color.isValid())
		return result;

	QString span = "<span style='";
	if (Bold)
		span += "font-weight:600;";
	if (Italic)
		span += "font-style:italic;";
	if (Underline)
		span += "text-decoration:underline;";
	if (Color.isValid())
		span += QString("color:%1;").arg(Color.name());
	span += "'>";

	return span + result + "</span>";
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
			HtmlDocument::unescapeText(partContent);
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
		HtmlDocument::unescapeText(partContent);
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

QString Message::toPlain() const
{
	QString result;
	foreach (MessagePart part, Parts)
		result += part.content();

	return result;
}

QString Message::toHtml() const
{
	QString result;
	foreach (MessagePart part, Parts)
		result += part.toHtml();

	return result;
}
