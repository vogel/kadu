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

#include "formatted-string/formatted-string-visitor.h"

#include "formatted-string-image-block.h"

FormattedStringImageBlock::FormattedStringImageBlock()
{
}

FormattedStringImageBlock::FormattedStringImageBlock(QString imagePath) :
		ImagePath{std::move(imagePath)}, Image{imagePath, 0}
{
}

FormattedStringImageBlock::FormattedStringImageBlock(ChatImage image) :
		Image{std::move(image)}
{
	if (!Image.isNull())
		ImagePath = Image.key();
}

FormattedStringImageBlock::~FormattedStringImageBlock()
{
}

bool FormattedStringImageBlock::operator == (const FormattedString &compareTo)
{
	const FormattedStringImageBlock *compareToPointer = dynamic_cast<const FormattedStringImageBlock *>(&compareTo);
	if (!compareToPointer)
		return false;

	return ImagePath == compareToPointer->ImagePath
	    && Image == compareToPointer->Image;
}

void FormattedStringImageBlock::accept(FormattedStringVisitor *visitor) const
{
	visitor->visit(this);
}

bool FormattedStringImageBlock::isEmpty() const
{
	return ImagePath.isEmpty() && Image.isNull();
}

QString FormattedStringImageBlock::imagePath() const
{
	return ImagePath;
}

ChatImage FormattedStringImageBlock::image() const
{
	return Image;
}
