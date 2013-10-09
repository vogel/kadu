/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "formatted-string/formatted-string-visitor.h"

#include "formatted-string-text-block.h"

FormattedStringTextBlock::FormattedStringTextBlock(const QString &content, bool bold, bool italic, bool underline, QColor color) :
		Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color)
{
}

FormattedStringTextBlock::~FormattedStringTextBlock()
{
}


bool FormattedStringTextBlock::operator == (const FormattedString &compareTo)
{
	const FormattedStringTextBlock *compareToPointer = dynamic_cast<const FormattedStringTextBlock *>(&compareTo);
	if (!compareToPointer)
		return false;

	return Content == compareToPointer->Content
	    && Bold == compareToPointer->Bold
	    && Italic == compareToPointer->Italic
	    && Underline == compareToPointer->Underline
	    && Color == compareToPointer->Color;
}

void FormattedStringTextBlock::accept(FormattedStringVisitor *visitor) const
{
	visitor->visit(this);
}

bool FormattedStringTextBlock::isEmpty() const
{
	return Content.isEmpty();
}

QString FormattedStringTextBlock::content() const
{
	return Content;
}

bool FormattedStringTextBlock::bold() const
{
	return Bold;
}

bool FormattedStringTextBlock::italic() const
{
	return Italic;
}

bool FormattedStringTextBlock::underline() const
{
	return Underline;
}

QColor FormattedStringTextBlock::color() const
{
	return Color;
}
