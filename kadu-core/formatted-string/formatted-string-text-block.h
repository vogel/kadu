/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FORMATTED_STRING_TEXT_BLOCK_H
#define FORMATTED_STRING_TEXT_BLOCK_H

#include <QtCore/QString>
#include <QtGui/QColor>

#include "exports.h"

#include "formatted-string.h"

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedStringTextBlock
 * @short FormattedString subclass that contains information about block of text with one formatting settings.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class contains information about block of text with one formatting settings.
 */
class KADUAPI FormattedStringTextBlock : public FormattedString
{
	QString Content;
	bool Bold;
	bool Italic;
	bool Underline;
	QColor Color;

public:
	/**
	 * @short Create new instance of FormattedStringTextBlock with given content and formatting settings.
	 * @author Rafał 'Vogel' Malinowski
	 * @param content content of text block
	 * @param bold bold flag
	 * @param italic italic flag
	 * @param underline underline flag
	 * @param color color flag
	 */
	FormattedStringTextBlock(const QString &content, bool bold, bool italic, bool underline, QColor color);
	virtual ~FormattedStringTextBlock();

	virtual bool operator == (const FormattedString &compareTo);

	virtual void accept(FormattedStringVisitor *visitor) const;

	/**
	 * @short Return true if content is empty.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if content is empty
	 */
	virtual bool isEmpty() const;

	/**
	 * @short Return provided content.
	 * @author Rafał 'Vogel' Malinowski
	 * @return provided content
	 */
	QString content() const;

	/**
	 * @short Return provided bold flag.
	 * @author Rafał 'Vogel' Malinowski
	 * @return provided bold flag
	 */
	bool bold() const;

	/**
	 * @short Return provided italic flag.
	 * @author Rafał 'Vogel' Malinowski
	 * @return provided italic flag
	 */
	bool italic() const;

	/**
	 * @short Return provided underline flag.
	 * @author Rafał 'Vogel' Malinowski
	 * @return provided underline flag
	 */
	bool underline() const;

	/**
	 * @short Return provided color.
	 * @author Rafał 'Vogel' Malinowski
	 * @return provided color
	 */
	QColor color() const;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_TEXT_BLOCK_H
