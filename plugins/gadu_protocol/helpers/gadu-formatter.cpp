/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#define GG_IGNORE_DEPRECATED
#include <libgadu.h>

#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-text-block.h"

#include "gadu-formatter.h"

struct FormatAttribute
{
	gg_msg_richtext_format format;
	union
	{
		gg_msg_richtext_color color;
		gg_msg_richtext_image image;
	};

	bool operator < (const FormatAttribute &compareTo) const
	{
		return format.position < compareTo.format.position;
	}
};

Q_DECLARE_TYPEINFO(FormatAttribute, Q_PRIMITIVE_TYPE);

namespace GaduFormatter
{

static QList<FormatAttribute> createFormatList(const unsigned char *formats, unsigned int size)
{
	QList<FormatAttribute> formatList;
	unsigned int memoryPosition = 0;

	if (size == 0 || !formats)
		return formatList;

	while (memoryPosition + sizeof(gg_msg_richtext_format) <= size)
	{
		FormatAttribute format;

		memcpy(&format.format, formats + memoryPosition, sizeof(format.format));
		memoryPosition += sizeof(format.format);
		format.format.position = gg_fix16(format.format.position);

		if (format.format.font & GG_FONT_IMAGE)
		{
			if (memoryPosition + sizeof(format.image) > size)
				break;

			memcpy(&format.image, formats + memoryPosition, sizeof(format.image));
			memoryPosition += sizeof(format.image);
		}
		else if (format.format.font & GG_FONT_COLOR)
		{
			if (memoryPosition + sizeof(format.color) > size)
				break;

			memcpy(&format.color, formats + memoryPosition, sizeof(format.color));
			memoryPosition += sizeof(format.color);
		}

		formatList.append(format);
	}

	qStableSort(formatList);

	return formatList;
}

static FormattedString * imagePart(const gg_msg_richtext_image &image)
{
	quint32 size = gg_fix32(image.size);
	quint32 crc32 = gg_fix32(image.crc32);

	if (size == 20 && (crc32 == 4567 || crc32 == 99)) // fake spy images
		return 0;

	ChatImageKey key(size, crc32);
	return new FormattedStringImageBlock(key);
}

static FormattedString * messagePart(const QString &content, const gg_msg_richtext_format &format, const gg_msg_richtext_color &color)
{
	QColor textColor;
	if (format.font & GG_FONT_COLOR)
	{
		textColor.setRed(color.red);
		textColor.setGreen(color.green);
		textColor.setBlue(color.blue);
	}

	return new FormattedStringTextBlock(content, format.font & GG_FONT_BOLD, format.font & GG_FONT_ITALIC, format.font & GG_FONT_UNDERLINE, textColor);
}

FormattedString * createMessage(const QString &content, const unsigned char *formats, unsigned int size)
{
	QVector<FormattedString *> items;
	QList<FormatAttribute> formatList = createFormatList(formats, size);

	// Initial value is 0 so that we will not loose any text potentially not covered by any formats.
	quint16 strayTextPosition = 0;
	bool hasStrayText = true;

	for (int i = 0, len = formatList.length(); ; ++i)
	{
		quint16 textPosition = (i < len)
				? formatList.at(i).format.position
				: static_cast<quint16>(content.length());

		if (hasStrayText && strayTextPosition < textPosition)
			items.append(new FormattedStringTextBlock(content.mid(strayTextPosition, textPosition - strayTextPosition), false, false, false, QColor()));
		hasStrayText = false;

		if (i >= len)
			break;

		const FormatAttribute &format = formatList.at(i);
		if (textPosition > content.length())
			break;

		if (format.format.font & GG_FONT_IMAGE)
		{
			FormattedString *formattedImage = imagePart(format.image);
			if (formattedImage)
			{
				items.append(formattedImage);

				// Assume only one character can represent GG_FONT_IMAGE and never loose the rest of the text.
				strayTextPosition = textPosition + 1;
				hasStrayText = true;
			}
		}
		else if (textPosition < content.length())
		{
			quint16 nextTextPosition = (i + 1 < len)
					? formatList.at(i + 1).format.position
					: static_cast<quint16>(content.length());

			items.append(messagePart(content.mid(textPosition, nextTextPosition - textPosition), format.format, format.color));
		}
	}

	return new CompositeFormattedString(items);
}

} // namespace
