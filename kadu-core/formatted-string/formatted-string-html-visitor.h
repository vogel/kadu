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

#ifndef FORMATTED_STRING_HTML_VISITOR_H
#define FORMATTED_STRING_HTML_VISITOR_H

#include <QtCore/QString>

#include "formatted-string/formatted-string-visitor.h"
#include "exports.h"

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
class KADUAPI FormattedStringHtmlVisitor : public FormattedStringVisitor
{
	Q_DISABLE_COPY(FormattedStringHtmlVisitor);

public:
	FormattedStringHtmlVisitor();
	virtual ~FormattedStringHtmlVisitor();

	virtual void beginVisit(const CompositeFormattedString * const compositeFormattedString) override;
	virtual void endVisit(const CompositeFormattedString * const compositeFormattedString) override;
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock) override;
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock) override;

	/**
	* @short Returns HTML view of visited FormattedString.
	* @author Rafał 'Vogel' Malinowski
	* @return HTML view of visited FormattedString
	*/
	QString result() const;

protected:
	void append(QString content);

private:
	QString m_result;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_HTML_VISITOR_H
