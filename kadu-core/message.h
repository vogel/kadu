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
#include <QtCore/QRegExp>
#include <QtGui/QColor>

#include "protocol.h"

class MessagePart
{
	bool Image;
	UinType ImageSender;
	int ImageSize;
	int ImageCrc32;

	QString Content;
	bool Bold;
	bool Italic;
	bool Underline;
	QColor Color;

	QString ImagePath;

public:
	MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color);
	MessagePart(UinType imageSender, int imageSize, int imageCrc32);
	MessagePart(const QString &imagePath);
	virtual ~MessagePart();

	bool isImage() const { return Image; }

	QString content() const { return Content; }
	bool bold() const { return Bold; }
	bool italic() const { return Italic; }
	bool underline() const { return Underline; }
	QColor color() const { return Color; }

	QString imagePath() const { return ImagePath; }

	QString toHtml() const;

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

	QString toPlain() const;
	QString toHtml() const;

};

#endif // MESSAGE_H
