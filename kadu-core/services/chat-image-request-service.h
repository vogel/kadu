/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_IMAGE_REQUEST_SERVICE_H
#define CHAT_IMAGE_REQUEST_SERVICE_H

#include <QtCore/QPointer>

#include "accounts/account.h"
#include "protocols/services/chat-image.h"
#include "services/chat-image-request-service-configuration.h"

class AccountManager;
class ContactManager;
class ImageStorageService;

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class ChatImageRequestService
 * @short Service for responding to received image keys.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service listens on all ChatImageService that are available through given AccountManager. It responds on
 * chatImageKeyReceived() signals from these services by either requesting full image data or ignoring it, depending
 * on configuration and number of images received during last few minutes - to avaid DOS attacks with too many
 * images.
 *
 * It also stores images received from peers in ImageStorageService,
 */
class ChatImageRequestService : public QObject
{
	Q_OBJECT

	ChatImageRequestServiceConfiguration Configuration;

	static const quint32 ReceivedImageKeysPerMinuteLimit = 10;
	quint32 ReceivedImageKeysCount;

	QPointer<ImageStorageService> CurrentImageStorageService;
	QPointer<AccountManager> CurrentAccountManager;
	QPointer<ContactManager> CurrentContactManager;

	bool acceptImage(const Account &account, const QString &id, const ChatImage &chatImage) const;

private slots:
	void accountRegistered(Account account);
	void accountUnregistered(Account account);

	void chatImageKeyReceived(const QString &id, const ChatImage &chatImage);
    void chatImageAvailable(const ChatImage &chatImage, const QByteArray &imageData);

	void resetReceivedImageKeysCount();

public:
	/**
	 * @short Create new instance of ChatImageRequestService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit ChatImageRequestService(QObject *parent = 0);
	virtual ~ChatImageRequestService();

	/**
	 * @short Sets image storage to use by this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageStorageService image storage to use by this service
	 *
	 * This service is used to storing images received from peers.
	 */
	void setImageStorageService(ImageStorageService *imageStorageService);

	/**
	 * @short Sets account manager to use by this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param accountManager account manager to use by this service
	 *
	 * This service will listen on registering/unregistering accounts to connect to ChatImageService instances
	 * from these accounts.
	 */
	void setAccountManager(AccountManager *accountManager);

	/**
	 * @short Sets contact manager to use by this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param contactManager contact manager to use by this service
	 *
	 * This service will use this manager to resolve names of contacts that ids are provided by ChatImageService signals.
	 * Resolved names are used for asking end-user about images that are too big to be accepted without asking.
	 * If no ContactManager is available then no asking will be done - too big images will be ignored.
	 */
	void setContactManager(ContactManager *contactManager);

	/**
	 * @short Sets configuration to use by this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param configuration configuration to use by this service
	 */
	void setConfiguration(ChatImageRequestServiceConfiguration configuration);

signals:
	/**
	 * @short Signal emitted when chat image was received and stored as file.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageKey key of stored image
	 * @param fullFilePath full file path of stored image
	 */
	void chatImageStored(const ChatImage &chatImage, const QString &fullFilePath);

};

/**
 * @}
 */

#endif // CHAT_IMAGE_REQUEST_SERVICE_H
