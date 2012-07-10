/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CHAT_IMAGE_SERVICE_H
#define CHAT_IMAGE_SERVICE_H

#include <QtCore/QObject>

#include "protocols/services/account-service.h"
#include "protocols/services/chat-image.h"
#include "protocols/services/chat-image-key.h"
#include "exports.h"

class Error;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class ChatImageService
 * @short Service for handling images in chat window.
 * @todo This is very very tightly tied to Gadu Gadu protocol. Refactor, maybe merge with ChatService.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service supports handling images in chat window. Two main functions are: checking if image will fit
 * in protocol - checkImageSize() and informing about received image - chatImageAvailable().
 */
class KADUAPI ChatImageService : public AccountService
{
	Q_OBJECT

protected:
	explicit ChatImageService(Account account, QObject *parent = 0);
	virtual ~ChatImageService();

public:
	/**
	 * @short Return path to directory that will hold all images.
	 * @author Rafał 'Vogel' Malinowski
	 * @return path to directory that will hold all images
	 */
	static QString imagesPath();

	/**
	 * @short Check if image with given size can be sent.
	 * @author Rafał 'Vogel' Malinowski
	 * @param size size of image to send (in bytes)
	 * @return information about if image with given size can be sent
	 *
	 * If returned Error object has severity NoError then image can be send without problems. If returned
	 * Error object has severity ErrorLow then protocol can try to send image but it may not succeed at this.
	 * In that case error's message should contain question to be asked to end-user. If returned Error object
	 * has severity ErrorHigh then this image can not be send at all. It that case message should contain warning
	 * to end-user.
	 */
	virtual Error checkImageSize(qint64 size) const = 0;

	virtual ChatImage createChatImage(const QString &localFileName) = 0;

	virtual void requestChatImage(const QString &id, const ChatImageKey &imageKey) = 0;

signals:
	/**
	 * @short Signal emitted when image with given key was received and saved on disc.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageKey key of received image
	 * @param image received image
	 */
	void chatImageAvailable(const ChatImageKey &imageKey, const ChatImage &image);

};

/**
 * @}
 */

#endif // CHAT_IMAGE_SERVICE_H
