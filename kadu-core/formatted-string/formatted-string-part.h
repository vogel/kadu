/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef FORMATTED_STRING_PART_H
#define FORMATTED_STRING_PART_H

#include <QtCore/QString>
#include <QtGui/QColor>

#include "protocols/protocol.h"
#include "protocols/services/chat-image-key.h"

#include "exports.h"

#include "formatted-string.h"

class QTextDocument;

/**
 * \class FormattedStringPart
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
class KADUAPI FormattedStringPart : public FormattedString
{
	QString Content;
	bool Bold;
	bool Italic;
	bool Underline;
	QColor Color;

public:
	/**
	 * Creates empty message part.
	 */
	FormattedStringPart() {}

	/**
	 * Creates text message part with formatting.
	 * @arg content content of message
	 * @arg bold if true, the whole part is presented with bold font
	 * @arg italic if true, the whole part is presented with italic font
	 * @arg underline if true, the whole part is presented with underline font
	 * @arg color color of whole part
	 */
	FormattedStringPart(const QString &content, bool bold, bool italic, bool underline, QColor color);

	FormattedStringPart(const FormattedStringPart &copyMe);

	virtual ~FormattedStringPart();

	FormattedStringPart & operator = (const FormattedStringPart &copyMe);

	virtual void accept(FormattedStringVisitor *visitor) const;

	virtual bool isEmpty() const { return Content.isEmpty(); }

	const QString & content() const { return Content; }
	bool bold() const { return Bold; }
	bool italic() const { return Italic; }
	bool underline() const { return Underline; }
	const QColor & color() const { return Color; }

};

#endif // FORMATTED_STRING_PART_H
