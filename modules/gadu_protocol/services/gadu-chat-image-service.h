/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CHAT_IMAGE_SERVICE
#define GADU_CHAT_IMAGE_SERVICE

#include "protocols/protocol.h"

#include "protocols/services/chat-image-service.h"

class GaduProtocol;

class GaduChatImageService : public ChatImageService
{
	Q_OBJECT

	GaduProtocol *Protocol;

private slots:
	void imageReceived(UinType sender, uint32_t size, uint32_t crc32, const QString &filename, const char *data);
	void imageRequestReceived(UinType, uint32_t, uint32_t);

public:
	GaduChatImageService(GaduProtocol *protocol);

};

#endif // GADU_CHAT_IMAGE_SERVICE
