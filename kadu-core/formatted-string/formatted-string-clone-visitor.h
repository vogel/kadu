/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FORMATTED_STRING_CLONE_VISITOR_H
#define FORMATTED_STRING_CLONE_VISITOR_H

#include <memory>
#include <stack>
#include <QtCore/QScopedPointer>
#include <QtCore/QStack>
#include <QtCore/QString>

#include "formatted-string/formatted-string-visitor.h"
#include "exports.h"

class FormattedString;

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedStringCloneVisitor
 * @short This visitor creates new FormattedString identical to visited FormattedString.
 * @author Rafał 'Vogel' Malinowski
 *
 * After being accepted by a FormattedString this visitor returns new FormattedString view identical to visited FormattedString.
 */
class KADUAPI FormattedStringCloneVisitor : public FormattedStringVisitor
{
	Q_DISABLE_COPY(FormattedStringCloneVisitor);

	std::stack<std::unique_ptr<FormattedString>> ItemsStack;

protected:
	void cloned(std::unique_ptr<FormattedString> &&clonedFormattedString);

public:
	FormattedStringCloneVisitor();
	virtual ~FormattedStringCloneVisitor();

	virtual void beginVisit(const CompositeFormattedString * const compositeFormattedString);
	virtual void endVisit(const CompositeFormattedString * const compositeFormattedString);
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock);
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock);

	/**
	* @short Returns new FormattedString instance identical to visited one.
	* @author Rafał 'Vogel' Malinowski
	* @return new FormattedString instance identical to visited one
	*
	* Caller gains ownership of the returned object. This method can only be called once.
	*/
	std::unique_ptr<FormattedString> result();

};

/**
 * @}
 */

#endif // FORMATTED_STRING_CLONE_VISITOR_H
