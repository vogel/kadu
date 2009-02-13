/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <libgadu.h>

#include <QtGui/QApplication>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "chat/message/message.h"

#include "contacts/contact-manager.h"

#include "config_file.h"

#include "gadu_images_manager.h"
#include "gadu-protocol.h"

#include "gadu_formatter.h"

unsigned int GaduFormater::computeFormatsSize(const Message &message)
{
	unsigned int size = sizeof(struct gg_msg_richtext);
	bool first = true;

	foreach (const MessagePart part, message.parts())
	{
		if (!first || part.isImage() || part.bold() || part.italic() || part.underline() || part.color().isValid())
		{
			size += sizeof(struct gg_msg_richtext_format);
			first = false;
		}

		if (part.isImage())
		{
			size += sizeof(struct gg_msg_richtext_image);
			first = false;
			continue;
		}

		if (part.color().isValid())
		{
			size += sizeof(struct gg_msg_richtext_color);
			first = false;
		}
	}

	return first ? 0 : size;
}

unsigned char * GaduFormater::createFormats(const Message &message, unsigned int &size)
{
	size = computeFormatsSize(message);
	if (!size)
		return 0;

	unsigned char *result = new unsigned char[size];
	bool first = true;
	unsigned int memoryPosition = sizeof(struct gg_msg_richtext);
	unsigned int textPosition = 0;

	struct gg_msg_richtext header;
	struct gg_msg_richtext_format format;
	struct gg_msg_richtext_color color;
	struct gg_msg_richtext_image image;

	header.flag = 2;
	header.length = gg_fix16(size - sizeof(struct gg_msg_richtext));
	memcpy(result, &header, sizeof(header));

	foreach (MessagePart part, message.parts())
	{
		if (first && !part.bold() && !part.italic() && !part.underline() && !part.color().isValid())
		{
			first = false;
			textPosition += part.content().length();
			continue;
		}

		format.position = gg_fix16(textPosition);
		format.font = 0;

		if (part.isImage())
		{
			format.font |= GG_FONT_IMAGE;
		}
		else
		{
			if (part.bold())
				format.font |= GG_FONT_BOLD;
			if (part.italic())
				format.font |= GG_FONT_ITALIC;
			if (part.underline())
				format.font |= GG_FONT_UNDERLINE;
			if (part.color().isValid())
				format.font |= GG_FONT_COLOR;
		}

		memcpy(result + memoryPosition, &format, sizeof(format));
		memoryPosition += sizeof(format);

		if (part.isImage())
		{
			uint32_t size;
			uint32_t crc32;
			gadu_images_manager.addImageToSend(part.imagePath(), size, crc32);

			image.unknown1 = 0x0109;
			image.size = gg_fix32(size);
			image.crc32 = gg_fix32(crc32);

			memcpy(result + memoryPosition, &image, sizeof(image));
			memoryPosition += sizeof(image);
		}
		else if (part.color().isValid())
		{
			color.red = part.color().red();
			color.green = part.color().green();
			color.blue = part.color().blue();

			memcpy(result + memoryPosition, &color, sizeof(color));
			memoryPosition += sizeof(color);
		}

		textPosition += part.content().length();
	}

	return result;
}

void GaduFormater::appendToMessage(Account *account, Message &result, UinType sender, const QString &content,
		struct gg_msg_richtext_format &format,
		struct gg_msg_richtext_color &color, struct gg_msg_richtext_image &image, bool receiveImages)
{
	QColor textColor;

	if (format.font & GG_FONT_IMAGE)
	{
		uint32_t size = gg_fix32(image.size);
		uint32_t crc32 = gg_fix32(image.crc32);

		if (size == 20 && (crc32 == 4567 || crc32 == 99)) // fake spy images
			return;

		QString file_name = gadu_images_manager.getSavedImageFileName(size, crc32);
		if (!file_name.isEmpty())
		{
			result << MessagePart(file_name);
			return;
		}

		if (!receiveImages)
		{
			result << MessagePart(qApp->translate("@default", QT_TR_NOOP("###IMAGE BLOCKED###")), false, false, false, textColor);
			return;
		}

		unsigned int maxSize = config_file.readUnsignedNumEntry("Chat", "MaxImageSize");
		if (size > maxSize * 1024)
		{
			result << MessagePart(qApp->translate("@default", QT_TR_NOOP("###IMAGE TOO BIG###")), false, false, false, textColor);
			return;
		}

		// TODO: fix
		GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(account->protocol());
		if (gadu)
		{
			gadu->sendImageRequest(account->getContactById(QString::number(sender)), size, crc32);
			result << MessagePart(sender, size, crc32);
		}
	}
	else
	{
		if (format.font & GG_FONT_COLOR)
		{
			textColor.setRed(color.red);
			textColor.setGreen(color.green);
			textColor.setBlue(color.blue);
		}

		result << MessagePart(content, format.font & GG_FONT_BOLD, format.font & GG_FONT_ITALIC, format.font & GG_FONT_UNDERLINE, textColor);
	}
}

#define MAX_NUMBER_OF_IMAGES 5

Message GaduFormater::createMessage(Account *account, UinType sender, const QString &content,
		unsigned char *formats, unsigned int size, bool receiveImages)
{
	Message result;

	if (size == 0 || !formats)
	{
		result << MessagePart(content, false, false, false, QColor());
		return result;
	}

	bool first = true;
	unsigned int memoryPosition = 0;
	unsigned int prevTextPosition = 0;
	unsigned int textPosition = 0;
	unsigned int images = 0;

	struct gg_msg_richtext_format prevFormat;
	struct gg_msg_richtext_format format;
	struct gg_msg_richtext_color prevColor;
	struct gg_msg_richtext_color color;
	struct gg_msg_richtext_image image;

	while (memoryPosition + sizeof(format) <= size)
	{
		memcpy(&format, formats + memoryPosition, sizeof(format));
		memoryPosition += sizeof(format);
		textPosition = gg_fix16(format.position);

		if (first && format.position > 0)
			result << MessagePart(content.mid(0, textPosition), false, false, false, QColor());

		if (format.font & GG_FONT_IMAGE)
		{
			images++;

			if (memoryPosition + sizeof(image) <= size)
			{
				memcpy(&image, formats + memoryPosition, sizeof(image));
				memoryPosition += sizeof(image);
			}
		}
		else
		{
			if (memoryPosition + sizeof(color) <= size)
				if (format.font & GG_FONT_COLOR)
				{
					memcpy(&color, formats + memoryPosition, sizeof(color));
					memoryPosition += sizeof(color);
				}
		}

		if (!first)
			appendToMessage(account, result, sender, content.mid(prevTextPosition, textPosition - prevTextPosition),
					prevFormat, prevColor, image, receiveImages && images <= MAX_NUMBER_OF_IMAGES);
		else
			first = false;

		prevTextPosition = textPosition;
		prevFormat = format;
		prevColor = color;
	}

	appendToMessage(account, result, sender, content.mid(prevTextPosition, content.length() - prevTextPosition),
			prevFormat, prevColor, image, receiveImages && images <= MAX_NUMBER_OF_IMAGES);

	return result;
}

