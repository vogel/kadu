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

#ifndef FORMATTED_STRING_FORMATS_SIZE_VISITOR_H
#define FORMATTED_STRING_FORMATS_SIZE_VISITOR_H

#include "formatted-string/formatted-string-visitor.h"

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class FormattedStringFormatsSizeVisitor
 * @short Visitor that couns size of libgadu formats structure required to describe visited FormattedString.
 * @author Rafał 'Vogel' Malinowski
 *
 * This visitor couns size of libgadu formats structure required to describe visited FormattedString. This size can be
 * then passed to FormattedStringFormatsVisitor contructor.
 */
class FormattedStringFormatsSizeVisitor : public FormattedStringVisitor
{
	Q_DISABLE_COPY(FormattedStringFormatsSizeVisitor);

	bool AllowImages;
	bool First;
	unsigned int Result;

public:
	/**
	 * @short Create new instance of FormattedStringFormatsSizeVisitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param allowImages if false then image formats from FormattedString will not be counted
	 */
	explicit FormattedStringFormatsSizeVisitor(bool allowImages);
	virtual ~FormattedStringFormatsSizeVisitor();

	virtual void visit(const CompositeFormattedString * const compositeFormattedString);
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock);
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock);

	/**
	 * @short Return computed formats structure size.
	 * @author Rafał 'Vogel' Malinowski
	 * @param computed formats structure size
	 */
	unsigned int result() const;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_FORMATS_SIZE_VISITOR_H
