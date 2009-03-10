/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "debug.h"

#include "helpers/gadu-formatter.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu_images_manager.h"
#include "gadu-protocol.h"

#include "gadu-chat-image-service.h"

GaduChatImageService::GaduChatImageService(GaduProtocol *protocol)
	: Protocol(protocol), CurrentMinuteSendImageRequests(0)
{
}

void GaduChatImageService::handleEventImageRequest(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, qPrintable(QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(e->event.image_request.sender).arg(e->event.image_request.size).arg(e->event.image_request.crc32)));

	gadu_images_manager.sendImage(e->event.image_request.sender, e->event.image_request.size, e->event.image_request.crc32);
}

void GaduChatImageService::handleEventImageReply(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, qPrintable(QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
			.arg(e->event.image_reply.sender).arg(e->event.image_reply.size)
			.arg(e->event.image_reply.crc32).arg(e->event.image_reply.filename)));

	QString fullPath = gadu_images_manager.saveImage(e->event.image_reply.sender,
			e->event.image_reply.size, e->event.image_reply.crc32,
			e->event.image_reply.filename, e->event.image_reply.image);
	emit imageReceived(GaduFormater::createImageId(e->event.image_reply.sender,
			e->event.image_reply.size, e->event.image_reply.crc32), fullPath);
}

bool GaduChatImageService::sendImageRequest(Contact contact, int size, uint32_t crc32)
{
	kdebugf();

	if (!contact.accountData(Protocol->account()) ||
			(CurrentMinuteSendImageRequests > config_file.readUnsignedNumEntry("Chat", "MaxImageRequests")))
		return false;

	CurrentMinuteSendImageRequests++;
	return 0 == gg_image_request(Protocol->gaduSession(), Protocol->uin(contact), size, crc32);
}


bool GaduChatImageService::sendImage(Contact contact, const QString &file_name, uint32_t size, const char *data)
{
	kdebugf();

	if (!contact.accountData(Protocol->account()))
		return false;

	return 0 == gg_image_reply(Protocol->gaduSession(), Protocol->uin(contact), qPrintable(file_name), data, size);
}
