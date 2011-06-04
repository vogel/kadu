/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QApplication>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "chat/message/formatted-message.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "gui/windows/message-dialog.h"

#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "gadu-formatter.h"

namespace GaduFormatter
{

unsigned int computeFormatsSize(const FormattedMessage &message)
{
	unsigned int size = sizeof(struct gg_msg_richtext);
	bool first = true;

	foreach (const FormattedMessagePart &part, message.parts())
	{
		if (!first || part.isImage() || part.bold() || part.italic() || part.underline() || part.color().isValid())
		{
			first = false;

			size += sizeof(struct gg_msg_richtext_format);

			if (part.isImage())
				size += sizeof(struct gg_msg_richtext_image);
			else if (part.color().isValid())
				size += sizeof(struct gg_msg_richtext_color);
		}
	}

	return first ? 0 : size;
}

unsigned char * createFormats(Account account, const FormattedMessage &message, unsigned int &size)
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

	foreach (const FormattedMessagePart &part, message.parts())
	{
		if (!first || part.isImage() || part.bold() || part.italic() || part.underline() || part.color().isValid())
		{
			first = false;

			format.position = gg_fix16(textPosition);
			format.font = 0;

			if (part.isImage())
				format.font |= GG_FONT_IMAGE;
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
				quint32 size;
				quint32 crc32;

				GaduChatImageService *gcis = static_cast<GaduChatImageService *>(account.protocolHandler()->chatImageService());
				gcis->prepareImageToSend(part.imagePath(), size, crc32);

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
		}

		textPosition += part.content().length();
	}

	return result;
}

static void appendToMessage(Account account, FormattedMessage &result, Contact contact, const QString &content,
		struct gg_msg_richtext_format &format,
		struct gg_msg_richtext_color &color, struct gg_msg_richtext_image &image, bool receiveImages)
{
	QColor textColor;

	if (format.font & GG_FONT_IMAGE)
	{
		quint32 size = gg_fix32(image.size);
		quint32 crc32 = gg_fix32(image.crc32);

		if (size == 20 && (crc32 == 4567 || crc32 == 99)) // fake spy images
			return;

		if (!receiveImages)
		{
			result << FormattedMessagePart(qApp->translate("@default", "IMAGE SENT BY THIS BUDDY HAS BEEN BLOCKED"), false, true, false, textColor);
			return;
		}

		GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account.details());
		if (!details)
			return;

		if (details->limitImageSize() && size > (quint32)details->maximumImageSize() * 1024)
		{
			bool allow = false;
			if (details->imageSizeAsk())
			{
				QString question = qApp->translate("@default",
						"Buddy %1 is attempting to send you an image of %2 KiB in size.\n"
						"This exceeds your configured limits.\n"
						"Do you want to accept this image anyway?")
						.arg(contact.ownerBuddy().display()).arg((size + 1023) / 1024);
				allow = MessageDialog::ask(
						KaduIcon("dialog-question"),
						qApp->translate("@default", "Kadu") + " - " + qApp->translate("@default", "Incoming Image"),
						question);
			}
			if (!allow)
			{
				result << FormattedMessagePart(qApp->translate("@default", "THIS BUDDY HAS SENT YOU AN IMAGE THAT IS TOO BIG TO BE RECEIVED"), false, true, false, textColor);
				return;
			}
		}

		// TODO: fix
		GaduProtocol *gadu = qobject_cast<GaduProtocol *>(account.protocolHandler());
		if (gadu)
		{
			static_cast<GaduChatImageService *>(gadu->chatImageService())->sendImageRequest(contact, size, crc32);
			result << FormattedMessagePart(createImageId(contact.id().toUInt(), size, crc32));
		}
	}
	else if (!content.isEmpty())
	{
		if (format.font & GG_FONT_COLOR)
		{
			textColor.setRed(color.red);
			textColor.setGreen(color.green);
			textColor.setBlue(color.blue);
		}

		result << FormattedMessagePart(content, format.font & GG_FONT_BOLD,
				format.font & GG_FONT_ITALIC, format.font & GG_FONT_UNDERLINE, textColor);
	}
}

#define MAX_NUMBER_OF_IMAGES 5

QString createImageId(unsigned int sender, unsigned int size, unsigned int crc32)
{
	return QString("gadu-%1-%2-%3")
		.arg(sender)
		.arg(size)
		.arg(crc32);
}

FormattedMessage createMessage(Account account, Contact contact, const QString &content,
		unsigned char *formats, unsigned int size, bool receiveImages)
{
	FormattedMessage result;

	if (size == 0 || !formats)
	{
		if (!content.isEmpty())
			result << FormattedMessagePart(content, false, false, false, QColor());
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
			result << FormattedMessagePart(content.left(textPosition), false, false, false, QColor());

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

		if (!first && textPosition > prevTextPosition)
			appendToMessage(account, result, contact, content.mid(prevTextPosition, textPosition - prevTextPosition),
					prevFormat, prevColor, image, receiveImages && images <= MAX_NUMBER_OF_IMAGES);
		else
			first = false;

		if (textPosition > prevTextPosition)
			prevTextPosition = textPosition;

		prevFormat = format;
		prevColor = color;
	}

	appendToMessage(account, result, contact, content.mid(prevTextPosition, content.length() - prevTextPosition),
			prevFormat, prevColor, image, receiveImages && images <= MAX_NUMBER_OF_IMAGES);

	return result;
}

} // namespace
