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

#include "exports.h"

class QTextDocument;

/**
 * \class MessagePart
 * \brief Part of message that has text and formatting (or an image)
 *
 * This class represents a part of message - text with formatting or an image.
 * Each \see Message is constructed from a series of message parts.
 *
 * Method isImage is used to distinguish image-parts from text-parts.
 *
 * There are two kinds of image parts:
 * <ul>
 *   <li>not-yet-received image parts -
 *       they contains informations about sender of image, and image size and crc32
 *       (used with Gadu-Gadu protocol to assign incoming images with messages).
 *   </li>
 *   <li>received image parts -
 *       they contains only local image path
 *   </li>
 * </ul>
 * @TODO: refactor
 */
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
	/**
	 * Creates text message part with formatting.
	 * @arg content content of message
	 * @arg bold if true, the whole part is presented with bold font
	 * @arg italic if true, the whole part is presented with italic font
	 * @arg underline if true, the whole part is presented with underline font
	 * @arg color color of whole part
	 */
	MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color);

	/**
	 * Creates image message part. All informations are used to
	 * assign an image to this part, when the real image is received from sender.
	 *
	 * @arg imageSender sender of the image
	 * @arg imageSize size of the image
	 * @arg imageCrc32 crc32 of the image
	 */
	MessagePart(UinType imageSender, int imageSize, int imageCrc32);

	/**
	 * Creates image message part.
	 * @arg imagePath local image path
	 */
	MessagePart(const QString &imagePath);
	virtual ~MessagePart();

	bool isImage() const { return Image; }
	bool isEmpty() const { return !Image && Content.isEmpty(); }

	QString content() const { return Content; }
	bool bold() const { return Bold; }
	bool italic() const { return Italic; }
	bool underline() const { return Underline; }
	QColor color() const { return Color; }

	QString imagePath() const { return ImagePath; }

	/**
	 * Converts message part to HTML - either formatted text or image.
	 * @return HTML representation of message parh.
	 */
	QString toHtml() const;

};

/**
 * \class Message
 * \brief Rich message (incoming or outcoming).
 *
 * This class represens incoming or outgoing message. Some protocols (like GG) uses its own
 * formatting, so this class acts like abstraction over all used formatting methods in Kadu.
 *
 * Message is splited into parts (\see MessagePart) - each part can contain text and formatting or an image.
 *
 * Each message has an <code>id</code> field that is used by protocols to store its message sequental number.
 */
class KADUAPI Message
{
	static QRegExp ImageRegExp;
	static void parseImages(Message &message, const QString &messageString, bool b, bool i, bool u, QColor color);

	QList<MessagePart> Parts;
	int Id;

public:
	/**
	 * Creates an empty message.
	 */
	Message();

	/**
	 * Creates a message with one, non-formatted text part.
	 *
	 * @arg messageString content of new message
	 */
	Message(const QString &messageString);

	virtual ~Message();

	/**
	 * Creates a message from given HTML document. The bold, italic, underline and
	 * color formatting are preserved and stored into result object.
	 * It also extracts images and inserts in into message.
	 *
	 * @arg messageDocument HTML document to parse
	 * @return Message representation of HTML document
	 */
	static Message parse(const QTextDocument *messageDocument);

	/**
	 * Returns all parts that composes this message.
	 * @return All parts that composes this message.
	 */
	QList<MessagePart> parts() const;

	/**
	 * Append a new part to message.
	 * @arg part New part to append.
	 */
	void append(MessagePart part);

	/**
	 * Append a new part to message.
	 * @arg part New part to append.
	 */
	Message & operator << (MessagePart part);

	void setId(int id) { Id = id; }
	int id() { return Id; }

	/**
	 * Returns true if message does not have any parts or if all parts are empty.
	 * @return True if message is empty.
	 */
	bool isEmpty() const;

	/**
	 * Returns message content, without formatting or images.
	 * @return Plain message content.
	 */
	QString toPlain() const;

	/**
	 * Converts message to HTML, with formatting and images. Resulting code is
	 * not a full HTML page - only the content.
	 * @return HTML representation of message.
	 */
	QString toHtml() const;

};

#endif // MESSAGE_H
