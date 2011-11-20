/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

	GaduProtocol *Protocol;
	unsigned int CurrentMinuteSendImageRequests;

	QString saveImage(UinType sender, quint32 size, quint32 crc32, const char *data);
	void loadImageContent(ImageToSend &image);

	friend class GaduProtocolSocketNotifiers;
	void handleEventImageRequest(struct gg_event *e);
	void handleEventImageReply(struct gg_event *e);

public:
	GaduChatImageService(GaduProtocol *protocol);

	void resetSendImageRequests() { CurrentMinuteSendImageRequests = 0; }
	bool sendImageRequest(Contact contact, int size, quint32 crc32);
	void prepareImageToSend(const QString &imageFileName, quint32 &size, quint32 &crc32);

	virtual qint64 softSizeLimit();
	virtual qint64 hardSizeLimit();
	virtual bool showSoftSizeWarning(Account account);

};

#endif // GADU_CHAT_IMAGE_SERVICE
