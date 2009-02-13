/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include "../gadu_images_manager.h"
#include "../gadu-protocol.h"
#include "../gadu-protocol-socket-notifiers.h"

#include "gadu-chat-image-service.h"

GaduChatImageService::GaduChatImageService(GaduProtocol *protocol)
	: Protocol(protocol)
{
	connect(Protocol->socketNotifiers(), SIGNAL(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)),
		this, SLOT(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)));
	connect(Protocol->socketNotifiers(), SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)),
		this, SLOT(imageRequestReceived(UinType, uint32_t, uint32_t)));
}

void GaduChatImageService::imageReceived(UinType sender, uint32_t size, uint32_t crc32, const QString &filename, const char *data)
{
	kdebugm(KDEBUG_INFO, qPrintable(QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
		.arg(sender).arg(size).arg(crc32).arg(filename)));

	QString full_path = gadu_images_manager.saveImage(sender,size,crc32,filename,data);
	// emit imageReceivedAndSaved(sender, size, crc32, full_path);
}

void GaduChatImageService::imageRequestReceived(UinType sender, uint32_t size, uint32_t crc32)
{
	kdebugm(KDEBUG_INFO, qPrintable(QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(sender).arg(size).arg(crc32)));

	gadu_images_manager.sendImage(sender,size,crc32);
}
