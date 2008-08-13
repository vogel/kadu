/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTextDocument>

#include "gadu_images_manager.h"
#include "html_document.h"
#include "icons_manager.h"

#include "message.h"

QRegExp Message::ParagraphRegExp("<p[^>]*>(.*)</p>");
QRegExp Message::SpanRegExp("<span.*(font-weight:600)?.*(font-style:italic)?.*(text-decoration: underline)?.*(color:(#[0-9a-fA-F]+))?.*>(.*)</span>");
QRegExp Message::ImageRegExp("\\[IMAGE ([^\\]]+)\\]");

MessagePart::MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color)
	: Image(false), Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color)
{
}

MessagePart::MessagePart(const QString &imagePath)
	: Image(true), ImagePath(imagePath), ImageSender(0), ImageSize(0), ImageCrc32(0)
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

Message Message::parse(const QString &messageString)
{
	Message result;

	QString workingMessage = messageString;
	QString cleanedMessage;
	QString partContent; 

	// TODO: move
	ParagraphRegExp.setMinimal(true);
	SpanRegExp.setMinimal(true);
	ImageRegExp.setMinimal(true);

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
			parseImages(result, partContent, false, false, false, QColor());
		}

		bool b = !SpanRegExp.cap(1).isEmpty();
		bool i = !SpanRegExp.cap(2).isEmpty();
		bool u = !SpanRegExp.cap(3).isEmpty();
		QColor color = QColor(SpanRegExp.cap(5));

		partContent = SpanRegExp.cap(6);
		parseImages(result, partContent, b, i, u, color);

		pos += SpanRegExp.matchedLength();
		lastPos = pos;
	}

	if (lastPos != cleanedMessage.length())
	{
		partContent = cleanedMessage.mid(lastPos, cleanedMessage.length() - lastPos);
		HtmlDocument::unescapeText(partContent);
		parseImages(result, partContent, false, false, false, QColor());
	}

	return result;
}

Message::Message()
{
}

Message::Message(const QString &messageString)
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
