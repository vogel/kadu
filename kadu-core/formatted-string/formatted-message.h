/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QColor>

#include "protocols/protocol.h"

#include "exports.h"

#include "formatted-message-part.h"

class QTextDocument;

class ImageStorageService;

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
	QVector<FormattedMessagePart> Parts;

public:
	/**
	 * Creates an empty message.
	 */
	FormattedMessage();

	FormattedMessage(const FormattedMessage &copyMe);

	FormattedMessage & operator = (const FormattedMessage &copyMe);

	virtual ~FormattedMessage();

	/**
	 * Returns all parts that composes this message.
	 * @return All parts that composes this message.
	 */
	const QVector<FormattedMessagePart> & parts() const;

	/**
	 * Append a new part to message.
	 * @arg part New part to append.
	 */
	void append(const FormattedMessagePart &part);

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
