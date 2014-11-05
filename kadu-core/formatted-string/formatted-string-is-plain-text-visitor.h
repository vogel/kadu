/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QString>

#include "formatted-string/formatted-string-visitor.h"
#include "exports.h"

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedStringIsPlainTextVisitor
 * @short This visitor check if formatted string is plain text number of items.
 * @author Rafał 'Vogel' Malinowski
 */
class KADUAPI FormattedStringIsPlainTextVisitor : public FormattedStringVisitor
{
	Q_DISABLE_COPY(FormattedStringIsPlainTextVisitor);

public:
	FormattedStringIsPlainTextVisitor();
	virtual ~FormattedStringIsPlainTextVisitor();

	virtual void beginVisit(const CompositeFormattedString * const compositeFormattedString) override;
	virtual void endVisit(const CompositeFormattedString * const compositeFormattedString) override;
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock) override;
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock) override;

	bool isPlainText() const;

private:
	bool m_first;
	bool m_isPlain;

};

/**
 * @}
 */
