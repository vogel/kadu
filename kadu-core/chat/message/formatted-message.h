/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FORMATTED_MESSAGE_H
#define FORMATTED_MESSAGE_H

#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtGui/QColor>

#include "protocols/protocol.h"

#include "exports.h"

#include "formatted-message-part.h"

class QTextDocument;

/**
 * \class FormattedMessage
 * \brief Rich message (incoming or outcoming).
 *
 * This class represens incoming or outgoing message. Some protocols (like GG) uses its own
 * formatting, so this class acts like abstraction over all used formatting methods in Kadu.
 *
 * FormattedMessage is splited into parts (\see FormattedMessagePart) - each part can contain text and formatting or an image.
 *
 * Each message has an <code>id</code> field that is used by protocols to store its message sequental number.
 */
class KADUAPI FormattedMessage
{
	static QRegExp ImageRegExp;
	static void parseImages(FormattedMessage &message, const QString &messageString, bool b, bool i, bool u, QColor color);

	QList<FormattedMessagePart> Parts;

public:
	/**
	 * Creates an empty message.
	 */
	FormattedMessage();

	/**
	 * Creates a message with one, non-formatted text part.
	 *
	 * @arg messageString content of new message
	 */
	FormattedMessage(const QString &messageString);

	virtual ~FormattedMessage();

	/**
	 * Creates a message from given HTML document. The bold, italic, underline and
	 * color formatting are preserved and stored into result object.
	 * It also extracts images and inserts in into message.
	 *
	 * @arg messageDocument HTML document to parse
	 * @return FormattedMessage representation of HTML document
	 */
	static FormattedMessage parse(const QTextDocument *messageDocument);

	/**
	 * Returns all parts that composes this message.
	 * @return All parts that composes this message.
	 */
	QList<FormattedMessagePart> parts() const;

	/**
	 * Prepend a new part to message.
	 * @arg part New part to pepend.
	 */
	void prepend(FormattedMessagePart part);

	/**
	 * Append a new part to message.
	 * @arg part New part to append.
	 */
	void append(FormattedMessagePart part);

	/**
	 * Append a new part to message.
	 * @arg part New part to append.
	 */
	FormattedMessage & operator << (FormattedMessagePart part);

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

#endif // FORMATTED_MESSAGE_H
