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

#include <string.h>

#include <QtCore/QFile>

#include <libgadu.h>

#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-text-block.h"
#include "protocols/services/chat-image-service.h"
#include "services/image-storage-service.h"

#include "formatted-string-formats-visitor.h"

FormattedStringFormatsVisitor::FormattedStringFormatsVisitor() :
		First(false), TextPosition(0)
{
	struct gg_msg_richtext header;
	header.flag = 2;
	header.length = 0;

	Result.append((const char *)&header, sizeof(header));
}

FormattedStringFormatsVisitor::~FormattedStringFormatsVisitor()
{
}

void FormattedStringFormatsVisitor::setChatImageService(ChatImageService *chatImageService)
{
	CurrentChatImageService = chatImageService;
}

void FormattedStringFormatsVisitor::setImageStorageService(ImageStorageService *imageStorageService)
{
	CurrentImageStorageService = imageStorageService;
}

void FormattedStringFormatsVisitor::visit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringFormatsVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	First = false;

	struct gg_msg_richtext_format format;

	format.position = gg_fix16(TextPosition);
	format.font = GG_FONT_IMAGE;

	Result.append((const char *)&format, sizeof(format));

	struct gg_msg_richtext_image image;

	QString imagePath = CurrentImageStorageService
			? CurrentImageStorageService.data()->fullPath(formattedStringImageBlock->imagePath())
			: formattedStringImageBlock->imagePath();
	QFile imageFile(imagePath);

	if (CurrentChatImageService && imageFile.open(QFile::ReadOnly))
	{
		QByteArray content = imageFile.readAll();
		const ChatImageKey &chatImageKey = CurrentChatImageService.data()->prepareImageToBeSent(content);
		imageFile.close();

		image.unknown1 = 0x0109;
		image.size = gg_fix32(chatImageKey.size());
		image.crc32 = gg_fix32(chatImageKey.crc32());
	}
	else
	{
		image.unknown1 = 0x0109;
		image.size = gg_fix32(0);
		image.crc32 = gg_fix32(0);
	}

	Result.append((const char *)&image, sizeof(image));
}

void FormattedStringFormatsVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	if (First && !formattedStringTextBlock->bold() && !formattedStringTextBlock->italic() && !formattedStringTextBlock->underline() && !formattedStringTextBlock->color().isValid())
		return;

	First = false;

	struct gg_msg_richtext_format format;

	format.position = gg_fix16(TextPosition);
	format.font = 0;

	if (formattedStringTextBlock->bold())
		format.font |= GG_FONT_BOLD;
	if (formattedStringTextBlock->italic())
		format.font |= GG_FONT_ITALIC;
	if (formattedStringTextBlock->underline())
		format.font |= GG_FONT_UNDERLINE;
	if (formattedStringTextBlock->color().isValid())
		format.font |= GG_FONT_COLOR;

	Result.append((const char *)&format, sizeof(format));

	if (formattedStringTextBlock->color().isValid())
	{
		struct gg_msg_richtext_color color;

		color.red = formattedStringTextBlock->color().red();
		color.green = formattedStringTextBlock->color().green();
		color.blue = formattedStringTextBlock->color().blue();

		Result.append((const char *)&color, sizeof(color));
	}

	TextPosition += formattedStringTextBlock->content().length();
}

QByteArray FormattedStringFormatsVisitor::result()
{
	struct gg_msg_richtext header;
	header.flag = 2;
	header.length = gg_fix16(Result.size() - sizeof(struct gg_msg_richtext));

	Result.replace(0, sizeof(header), (const char *)&header, sizeof(header));

	return Result;
}
