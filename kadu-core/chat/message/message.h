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

#include "protocols/protocol.h"

#include "exports.h"

#include "message-part.h"

class QTextDocument;

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
