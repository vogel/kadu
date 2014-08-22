/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef FORMATTED_STRING_VISITOR_H
#define FORMATTED_STRING_VISITOR_H

class CompositeFormattedString;
class FormattedStringImageBlock;
class FormattedStringTextBlock;

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedStringVisitor
 * @short Visitor that allows extraction of data from FormattedString instances.
 * @author Rafał 'Vogel' Malinowski
 *
 * Use instances of subclasses of FormattedStringVisitor to convert FormattedString to other formats of extract data from it.
 */
class FormattedStringVisitor
{

protected:
	FormattedStringVisitor() {}
	virtual ~FormattedStringVisitor() {}

public:
	/**
	 * @short Visit CompositeFormattedString instance before visiting sub-FormattedString.
	 * @author Rafał 'Vogel' Malinowski
	 * @param compositeFormattedString CompositeFormattedString instance to visit
	 */
	virtual void beginVisit(const CompositeFormattedString * const compositeFormattedString) = 0;

	/**
	 * @short Visit CompositeFormattedString instance after visiting sub-FormattedString.
	 * @author Rafał 'Vogel' Malinowski
	 * @param compositeFormattedString CompositeFormattedString instance to visit
	 */
	virtual void endVisit(const CompositeFormattedString * const compositeFormattedString) = 0;

	/**
	 * @short Visit FormattedStringImageBlock instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param formattedStringImageBlock FormattedStringImageBlock instance to visit
	 */
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock) = 0;

	/**
	 * @short Visit FormattedStringTextBlock instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param formattedStringTextBlock FormattedStringTextBlock instance to visit
	 */
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock) = 0;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_VISITOR_H
