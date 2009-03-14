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

	struct ImageToSend
	{
		QString fileName;
		QDateTime lastSent;
		QByteArray content;
		uint32_t crc32;
	};
	QMap<QPair<uint32_t, uint32_t>, ImageToSend> ImagesToSend;

	struct SavedImage
	{
		QString fileName;
		uint32_t size;
		uint32_t crc32;
	};
	QList<SavedImage> SavedImages;

	GaduProtocol *Protocol;
	unsigned int CurrentMinuteSendImageRequests;

	QString saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString &fileName, const char *data);
	void loadImageContent(ImageToSend &image);

	friend class GaduProtocolSocketNotifiers;
	void handleEventImageRequest(struct gg_event *e);
	void handleEventImageReply(struct gg_event *e);

public:
	GaduChatImageService(GaduProtocol *protocol);

	void resetSendImageRequests() { CurrentMinuteSendImageRequests = 0; }

	bool sendImageRequest(Contact contact, int size, uint32_t crc32);

	QString getSavedImageFileName(uint32_t size, uint32_t crc32);
	void prepareImageToSend(const QString &imageFileName, uint32_t &size, uint32_t &crc32);

};

#endif // GADU_CHAT_IMAGE_SERVICE
