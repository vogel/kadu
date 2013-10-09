/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

FormattedStringImageBlock::FormattedStringImageBlock(const QString &imagePath) :
		ImagePath(imagePath), ImageKey(0, 0)
{
}

FormattedStringImageBlock::FormattedStringImageBlock(const ChatImageKey &chatImageKey) :
		ImageKey(chatImageKey)
{
	if (!ImageKey.isNull())
		ImagePath = ImageKey.toString();
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
	    && ImageKey == compareToPointer->ImageKey;
}

void FormattedStringImageBlock::accept(FormattedStringVisitor *visitor) const
{
	visitor->visit(this);
}

bool FormattedStringImageBlock::isEmpty() const
{
	return ImagePath.isEmpty() && ImageKey.isNull();
}

QString FormattedStringImageBlock::imagePath() const
{
	return ImagePath;
}

ChatImageKey FormattedStringImageBlock::imageKey() const
{
	return ImageKey;
}
