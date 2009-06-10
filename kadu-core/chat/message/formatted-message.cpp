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

#include "formatted-message.h"

QRegExp FormattedMessage::ImageRegExp("\\[IMAGE ([^\\]]+)\\]");

void FormattedMessage::parseImages(FormattedMessage &message, const QString &messageString, bool b, bool i, bool u, QColor color)
{
	QString partContent; 

	int lastPos = 0;
	int pos = 0;

	while ((pos = ImageRegExp.indexIn(messageString, pos)) != -1) {
		if (lastPos != pos)
		{
			partContent = messageString.mid(lastPos, pos - lastPos);
			HtmlDocument::unescapeText(partContent);
			message << FormattedMessagePart(partContent, b, i, u, color);
		}

		QString fileName = ImageRegExp.cap(1);
		if (!fileName.isEmpty())
			message << FormattedMessagePart(fileName, false);

		pos += ImageRegExp.matchedLength();
		lastPos = pos;
	}

	if (lastPos != messageString.length())
	{
		partContent = messageString.mid(lastPos, messageString.length() - lastPos);
		HtmlDocument::unescapeText(partContent);
		message << FormattedMessagePart(partContent, b, i, u, color);
	}
}

FormattedMessage FormattedMessage::parse(const QTextDocument *document)
{
	FormattedMessage result;

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

FormattedMessage::FormattedMessage()
	: Id(0)
{
}

FormattedMessage::FormattedMessage(const QString &messageString)
	: Id(0)
{
	Parts.append(FormattedMessagePart(messageString, false, false, false, QColor()));
}

FormattedMessage::~FormattedMessage()
{
}

QList<FormattedMessagePart> FormattedMessage::parts() const
{
	return Parts;
}


void FormattedMessage::append(FormattedMessagePart part)
{
	Parts.append(part);
}

FormattedMessage & FormattedMessage::operator << (FormattedMessagePart part)
{
	Parts << part;
	return *this;
}

bool FormattedMessage::isEmpty() const
{
	foreach (FormattedMessagePart part, Parts)
		if (!part.isEmpty())
			return false;

	return true;
}

QString FormattedMessage::toPlain() const
{
	QString result;
	foreach (FormattedMessagePart part, Parts)
		result += part.content();

	return result;
}

QString FormattedMessage::toHtml() const
{
	QString result;
	foreach (FormattedMessagePart part, Parts)
		result += part.toHtml();

	return result;
}
