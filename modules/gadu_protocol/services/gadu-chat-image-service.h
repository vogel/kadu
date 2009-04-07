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
		quint32 crc32;
	};
	QMap<QPair<quint32, quint32>, ImageToSend> ImagesToSend;

	struct SavedImage
	{
		QString fileName;
		quint32 size;
		quint32 crc32;
	};
	QList<SavedImage> SavedImages;

	GaduProtocol *Protocol;
	unsigned int CurrentMinuteSendImageRequests;

	QString saveImage(UinType sender, quint32 size, quint32 crc32, const QString &fileName, const char *data);
	void loadImageContent(ImageToSend &image);

	friend class GaduProtocolSocketNotifiers;
	void handleEventImageRequest(struct gg_event *e);
	void handleEventImageReply(struct gg_event *e);

public:
	GaduChatImageService(GaduProtocol *protocol);

	void resetSendImageRequests() { CurrentMinuteSendImageRequests = 0; }

	bool sendImageRequest(Contact contact, int size, quint32 crc32);

	QString getSavedImageFileName(quint32 size, quint32 crc32);
	void prepareImageToSend(const QString &imageFileName, quint32 &size, quint32 &crc32);

};

#endif // GADU_CHAT_IMAGE_SERVICE
