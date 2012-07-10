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
#include "protocols/services/chat-image-key.h"

#include "protocols/services/chat-image-service.h"

class GaduConnection;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduChatImageService
 * @todo Refactor
 * @short Service for downloading and uploading avatars in Gadu-Gadu protocol.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service requries proper GaduConnection instance to work. Set it using setConnection() method.
 *
 * This service will only allow for some number of images sent by peers per minute to disable DOS attacks.
 * To manually re-enable receiving images in given minute call resetSendImageRequests().
 */
class GaduChatImageService : public ChatImageService
{
	Q_OBJECT

	static const qint64 RECOMMENDED_MAXIMUM_SIZE = 255 * 1024;

	QMap<ChatImageKey, ChatImage> ChatImages;

	QWeakPointer<GaduConnection> Connection;
	unsigned int CurrentMinuteSendImageRequests;

	ChatImage saveImage(const ChatImageKey &key, const char *data);
	QByteArray loadFileContent(const QString &localFileName);

	friend class GaduProtocolSocketNotifiers;
	void handleEventImageRequest(struct gg_event *e);
	void handleEventImageReply(struct gg_event *e);

public:
	/**
	 * @short Create new instance of GaduChatImageService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param account account bounded to this service
	 * @param parent QObject parent
	 */
	explicit GaduChatImageService(Account account, QObject *parent = 0);
	virtual ~GaduChatImageService();

	/**
	 * @short Set connection for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param connection connection for this service
	 */
	void setConnection(GaduConnection *connection);

	/**
	 * @short Reset count of received images.
	 * @author Rafał 'Vogel' Malinowski
	 *
	 * Call this method to reset count of received images. This service only allows for a number of received images
	 * in any minute to prevent DOS attacks. After reseting this value it allows for more received images.
	 */
	void resetSendImageRequests() { CurrentMinuteSendImageRequests = 0; }

	/**
	 * @short Request for an image from contact that sent info about it before.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id of contact that sent image info
	 * @param imageKey key of image to request
	 *
	 * Call this method to request an image from given contact. After image is received imageReceivedAndSaved() signal is emitted.
	 */
	virtual void requestChatImage(const QString &id, const ChatImageKey &imageKey);

	virtual ChatImage createChatImage(const QString &localFileName);

	virtual Error checkImageSize(qint64 size) const;

};

/**
 * @}
 */

#endif // GADU_CHAT_IMAGE_SERVICE
