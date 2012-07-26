/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FORMATTED_STRING_HTML_VISITOR_H
#define FORMATTED_STRING_HTML_VISITOR_H

#include <QtCore/QString>

#include "formatted-string/formatted-string-visitor.h"

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedStringHtmlVisitor
 * @short This visitor creates HTML view of visited FormattedString.
 * @author Rafał 'Vogel' Malinowski
 *
 * After being accepted by a FormattedString this visitor returns HTML view of this FormattedString as result() getter.
 */
class FormattedStringHtmlVisitor : public FormattedStringVisitor
{
	Q_DISABLE_COPY(FormattedStringHtmlVisitor);

	QString Result;

public:
	FormattedStringHtmlVisitor();
	virtual ~FormattedStringHtmlVisitor();

	virtual void visit(const CompositeFormattedString * const compositeFormattedString);
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock);
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock);

	/**
	* @short Returns HTML view of visited FormattedString.
	* @author Rafał 'Vogel' Malinowski
	* @return HTML view of visited FormattedString
	*/
	QString result() const;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_HTML_VISITOR_H
