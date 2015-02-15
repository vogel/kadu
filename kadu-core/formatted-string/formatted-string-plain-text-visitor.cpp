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

#include "formatted-string/formatted-string-text-block.h"

#include "formatted-string-plain-text-visitor.h"

FormattedStringPlainTextVisitor::FormattedStringPlainTextVisitor()
{
}

FormattedStringPlainTextVisitor::~FormattedStringPlainTextVisitor()
{
}

void FormattedStringPlainTextVisitor::beginVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringPlainTextVisitor::endVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringPlainTextVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	Q_UNUSED(formattedStringImageBlock);

	Result.append(QChar(QChar::Nbsp));
}

void FormattedStringPlainTextVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	Result.append(formattedStringTextBlock->content());
}

QString FormattedStringPlainTextVisitor::result() const
{
	QString fixedResult = Result;
	return fixedResult.replace(QChar::LineSeparator, "\n");
}
