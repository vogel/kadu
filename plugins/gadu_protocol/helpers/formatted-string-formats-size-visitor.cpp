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

#include <libgadu.h>

#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-part.h"

#include "formatted-string-formats-size-visitor.h"

FormattedStringFormatsSizeVisitor::FormattedStringFormatsSizeVisitor(bool allowImages) :
		AllowImages(allowImages), First(true), Result(sizeof(struct gg_msg_richtext))
{
}

FormattedStringFormatsSizeVisitor::~FormattedStringFormatsSizeVisitor()
{
}

void FormattedStringFormatsSizeVisitor::visit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringFormatsSizeVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	Q_UNUSED(formattedStringImageBlock);

	if (!AllowImages)
		return;

	First = false;
	Result += sizeof(struct gg_msg_richtext_format);
	Result += sizeof(struct gg_msg_richtext_image);
}

void FormattedStringFormatsSizeVisitor::visit(const FormattedStringPart * const formattedStringPart)
{
	if (!First && !formattedStringPart->bold() && !formattedStringPart->italic() && !formattedStringPart->underline() && !formattedStringPart->color().isValid())
		return;

	First = false;
	Result += sizeof(struct gg_msg_richtext_format);
	if (formattedStringPart->color().isValid())
		Result += sizeof(struct gg_msg_richtext_color);
}

unsigned int FormattedStringFormatsSizeVisitor::result() const
{
	return Result;
}
