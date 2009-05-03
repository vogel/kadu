/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

#include "html_document.h"
#include "icons-manager.h"

#include "message.h"

QRegExp Message::ImageRegExp("\\[IMAGE ([^\\]]+)\\]");

void Message::parseImages(Message &message, const QString &messageString, bool b, bool i, bool u, QColor color)
{
	QString partContent; 

	int lastPos = 0;
	int pos = 0;

	while ((pos = ImageRegExp.indexIn(messageString, pos)) != -1) {
		if (lastPos != pos)
		{
			partContent = messageString.mid(lastPos, pos - lastPos);
			HtmlDocument::unescapeText(partContent);
			message << MessagePart(partContent, b, i, u, color);
		}

		QString fileName = ImageRegExp.cap(1);
		if (!fileName.isEmpty())
			message << MessagePart(fileName, false);

		pos += ImageRegExp.matchedLength();
		lastPos = pos;
	}

	if (lastPos != messageString.length())
	{
		partContent = messageString.mid(lastPos, messageString.length() - lastPos);
		HtmlDocument::unescapeText(partContent);
		message << MessagePart(partContent, b, i, u, color);
	}
}

Message Message::parse(const QTextDocument *document)
{
	Message result;

	QString text;

	QTextBlock block = document->firstBlock();
	bool firstParagraph = true;
	while (block.isValid())
	{
		bool firstFragment = true;
		for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it)
		{
			QTextFragment fragment = it.fragment();
			if (!fragment.isValid())
				continue;

			if (!firstParagraph && firstFragment)
				text = QString("\n") + fragment.text();
			else
				text = fragment.text();

			QTextCharFormat format = fragment.charFormat();
			parseImages(result, text,
				format.font().bold(),
				format.font().italic(),
				format.font().underline(),
				format.foreground().color());

			firstFragment = false;
		}

		if (firstFragment)
			parseImages(result, "\n", false, false, false, QColor());

		block = block.next();
		firstParagraph = false;
	}

	return result;
}

Message::Message()
	: Id(0)
{
}

Message::Message(const QString &messageString)
	: Id(0)
{
	Parts.append(MessagePart(messageString, false, false, false, QColor()));
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

bool Message::isEmpty() const
{
	foreach (MessagePart part, Parts)
		if (!part.isEmpty())
			return false;

	return true;
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
