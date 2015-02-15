/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef FORMATTED_STRING_H
#define FORMATTED_STRING_H

#include "exports.h"

class FormattedStringVisitor;

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedString
 * @short Generic FormattedString item.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class contains FormattedString item. It can be either formatted text block, image, item composed of other items.
 */
class KADUAPI FormattedString
{

protected:
	FormattedString();

public:
	virtual ~FormattedString();

	virtual bool operator == (const FormattedString &compareTo) = 0;
	bool operator != (const FormattedString &compareTo);

	/**
	 * @short Accept given visitor on current item.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitor visitor to accept
	 */
	virtual void accept(FormattedStringVisitor *visitor) const = 0;

	/**
	 * @short Return true if item is empty.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if item is empty
	 */
	virtual bool isEmpty() const = 0;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_H
