/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "configuration/configuration-file.h"
#include "debug.h"

#include "helpers/gadu-formatter.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "gadu-chat-image-service.h"

GaduChatImageService::GaduChatImageService(GaduProtocol *protocol)
	: ChatImageService(protocol), Protocol(protocol), CurrentMinuteSendImageRequests(0)
{
}

QString GaduChatImageService::saveImage(UinType sender, quint32 size, quint32 crc32, const char *data)
{
	kdebugf();

	QString path = ChatImageService::imagesPath();
	if (!QFileInfo(path).isDir() && !QDir().mkdir(path))
	{
		kdebugm(KDEBUG_INFO, "Failed creating directory: %s\n", qPrintable(path));
		return QString();
	}

	QString fileName = GaduFormatter::createImageId(sender, size, crc32);
	QFile file(path + fileName);
	if (!file.open(QIODevice::WriteOnly))
		return QString();

	file.write(data, size);
	file.close();
	return fileName;
}

void GaduChatImageService::loadImageContent(ImageToSend &image)
{
	QFile imageFile(image.fileName);
	if (!imageFile.open(QIODevice::ReadOnly))
	{
		image.content.clear();
		kdebugm(KDEBUG_ERROR, "Error opening file\n");
		return;
	}

	QByteArray data = imageFile.readAll();
	imageFile.close();

	if (data.length() != imageFile.size())
	{
		image.content.clear();
		kdebugm(KDEBUG_ERROR, "Error reading file\n");
		return;
	}

	image.content = data;
}

void GaduChatImageService::handleEventImageRequest(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(e->event.image_request.sender).arg(e->event.image_request.size).arg(e->event.image_request.crc32)));

	quint32 size = e->event.image_request.size;
	quint32 crc32 = e->event.image_request.crc32;

	if (!ImagesToSend.contains(qMakePair(size, crc32)))
	{
		kdebugm(KDEBUG_WARNING, "Image data not found\n");
		return;
	}

	ImageToSend &image = ImagesToSend[qMakePair(size, crc32)];
	if (image.content.isNull())
	{
		loadImageContent(image);
		if (image.content.isNull())
			return;
	}

	gg_image_reply(Protocol->gaduSession(), e->event.image_request.sender, qPrintable(image.fileName), image.content.constData(), image.content.length());

	image.content.clear();
	image.lastSent = QDateTime::currentDateTime();
}

void GaduChatImageService::handleEventImageReply(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
			.arg(e->event.image_reply.sender).arg(e->event.image_reply.size)
			.arg(e->event.image_reply.crc32).arg(e->event.image_reply.filename)));

	QString fileName = saveImage(e->event.image_reply.sender,
			e->event.image_reply.size, e->event.image_reply.crc32,
			/*e->event.image_reply.filename, */e->event.image_reply.image);

	if (fileName.isEmpty())
		return;

	emit imageReceived(GaduFormatter::createImageId(e->event.image_reply.sender,
			e->event.image_reply.size, e->event.image_reply.crc32), fileName);
}

bool GaduChatImageService::sendImageRequest(Contact contact, int size, quint32 crc32)
{
	kdebugf();
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());

	if (contact.isNull() ||
			(CurrentMinuteSendImageRequests > (unsigned int)gaduAccountDetails->maximumImageRequests()))
		return false;

	CurrentMinuteSendImageRequests++;
	return 0 == gg_image_request(Protocol->gaduSession(), Protocol->uin(contact), size, crc32);
}

void GaduChatImageService::prepareImageToSend(const QString &imageFileName, quint32 &size, quint32 &crc32)
{
	kdebugmf(KDEBUG_INFO, "Using file \"%s\"\n", qPrintable(imageFileName));

	ImageToSend imageToSend;
	imageToSend.fileName = imageFileName;
	loadImageContent(imageToSend);

	if (imageToSend.content.isNull())
		return;

	imageToSend.crc32 = gg_crc32(0, (const unsigned char*)imageToSend.content.data(), imageToSend.content.length());

	size = imageToSend.content.length();
	crc32 = imageToSend.crc32;

	ImagesToSend[qMakePair(size, crc32)] = imageToSend;
}
