/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtGui/QTextDocument>

#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-text-block.h"
#include "misc/misc.h"

#include "formatted-string-is-plain-text-visitor.h"

FormattedStringIsPlainTextVisitor::FormattedStringIsPlainTextVisitor() :
		m_isPlain{true}
{
}

FormattedStringIsPlainTextVisitor::~FormattedStringIsPlainTextVisitor()
{
}

void FormattedStringIsPlainTextVisitor::beginVisit(const CompositeFormattedString * const)
{
}

void FormattedStringIsPlainTextVisitor::endVisit(const CompositeFormattedString * const)
{
}

void FormattedStringIsPlainTextVisitor::visit(const FormattedStringImageBlock * const)
{
	m_first = false;
	m_isPlain = false;
}

void FormattedStringIsPlainTextVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	// ignore colors
	if (formattedStringTextBlock->bold() || formattedStringTextBlock->italic() || formattedStringTextBlock->underline())
		m_isPlain = false;
}

bool FormattedStringIsPlainTextVisitor::isPlainText() const
{
	return m_isPlain;
}
