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

#ifndef COMPOSITE_FORMATTED_STRING_H
#define COMPOSITE_FORMATTED_STRING_H

#include <QtCore/QVector>
#include <QtGui/QColor>
#include <memory>

#include "exports.h"

#include "formatted-string.h"

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class CompositeFormattedString
 * @short This class represents FormattedString that is composed of other FormattedString instances.
 * @author Rafał 'Vogel' Malinowski
 */
class KADUAPI CompositeFormattedString : public FormattedString
{
	Q_DISABLE_COPY(CompositeFormattedString)

	std::vector<std::unique_ptr<FormattedString>> Items;

public:
	/**
	 * @short Create new instance of CompositeFormattedString.
	 * @author Rafał 'Vogel' Malinowski
	 * @param items items of composite FormattedString
	 */
	explicit CompositeFormattedString(std::vector<std::unique_ptr<FormattedString>> &&items);
	virtual ~CompositeFormattedString();

	virtual bool operator == (const FormattedString &compareTo);

	/**
	 * @short Accept a visitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitor visitor to accept
	 *
	 * This method executes visit() method of passed visitor on itself and then on all items.
	 */
	virtual void accept(FormattedStringVisitor *visitor) const;

	/**
	 * @short Return true if his FormattedString is empty or consists only of empty items.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if his FormattedString is empty or consists only of empty items
	 */
	virtual bool isEmpty() const;

	/**
	 * @short Return all items that compose this CompositeFormattedString.
	 * @author Rafał 'Vogel' Malinowski
	 * @return all items that compose this CompositeFormattedString
	 */
	const std::vector<std::unique_ptr<FormattedString>> & items() const;

};

#endif // COMPOSITE_FORMATTED_STRING_H
