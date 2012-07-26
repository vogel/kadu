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

FormattedStringFormatsVisitor::FormattedStringFormatsVisitor(int size) :
		First(false), MemoryPosition(0), TextPosition(0), Size(size), Result(new char[Size])
{
	struct gg_msg_richtext header;
	header.flag = 2;
	header.length = gg_fix16(size - sizeof(struct gg_msg_richtext));

	append(&header, sizeof(header));
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

void FormattedStringFormatsVisitor::append(void *data, unsigned int size)
{
	memcpy(Result.data() + MemoryPosition, data, size);
	MemoryPosition += size;
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

	append(&format, sizeof(format));

	if (!CurrentChatImageService)
		return;

	struct gg_msg_richtext_image image;

	QString imagePath = CurrentImageStorageService
			? CurrentImageStorageService.data()->fullPath(formattedStringImageBlock->imagePath())
			: formattedStringImageBlock->imagePath();
	QFile imageFile(imagePath);

	if (imageFile.open(QFile::ReadOnly))
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

	append(&image, sizeof(image));
}

void FormattedStringFormatsVisitor::visit(const FormattedStringTextBlock * const FormattedStringTextBlock)
{
	if (First && !FormattedStringTextBlock->bold() && !FormattedStringTextBlock->italic() && !FormattedStringTextBlock->underline() && !FormattedStringTextBlock->color().isValid())
		return;

	First = false;

	struct gg_msg_richtext_format format;

	format.position = gg_fix16(TextPosition);
	format.font = 0;

	if (FormattedStringTextBlock->bold())
		format.font |= GG_FONT_BOLD;
	if (FormattedStringTextBlock->italic())
		format.font |= GG_FONT_ITALIC;
	if (FormattedStringTextBlock->underline())
		format.font |= GG_FONT_UNDERLINE;
	if (FormattedStringTextBlock->color().isValid())
		format.font |= GG_FONT_COLOR;

	append(&format, sizeof(format));

	if (FormattedStringTextBlock->color().isValid())
	{
		struct gg_msg_richtext_color color;

		color.red = FormattedStringTextBlock->color().red();
		color.green = FormattedStringTextBlock->color().green();
		color.blue = FormattedStringTextBlock->color().blue();

		append(&color, sizeof(color));
	}

	TextPosition += FormattedStringTextBlock->content().length();
}

QByteArray FormattedStringFormatsVisitor::result() const
{
	return QByteArray(Result.data(), Size);
}
