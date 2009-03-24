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

#include "gadu_images_manager.h"
#include "html_document.h"
#include "icons_manager.h"

#include "message.h"

QRegExp Message::ImageRegExp("\\[IMAGE ([^\\]]+)\\]");

MessagePart::MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color)
	: Image(false), Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color)
{
}

MessagePart::MessagePart(const QString &imagePath)
	: Image(true), ImagePath(imagePath), ImageSender(0), ImageSize(0), ImageCrc32(0), Content("\n")
{
}

MessagePart::MessagePart(UinType imageSender, int imageSize, int imageCrc32)
	: Image(true), ImagePath(icons_manager->iconPath("LoadingImage")),
		ImageSender(imageSender), ImageSize(imageSize), ImageCrc32(imageCrc32)
{
}

MessagePart::~MessagePart()
{
}

QString MessagePart::toHtml() const
{
	if (Image && ImageSender)
	{
		return GaduImagesManager::loadingImageHtml(ImageSender, ImageSize,ImageCrc32);;
	}
	else if (Image)
	{
		return QString("<img src=\"file:///%1\" />").arg(ImagePath);
	}
	else
	{
		QString result = Qt::escape(Content);
		result.replace("\n", "<br />");
		result.replace(QChar::LineSeparator, "<br />");

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
}

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
			message << MessagePart(fileName);

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
