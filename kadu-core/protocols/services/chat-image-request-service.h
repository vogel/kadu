/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QWeakPointer>

#include "accounts/account.h"
#include "protocols/services/chat-image-key.h"

class AccountManager;

/**
 * @addtogroup Protocol
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
 */
class ChatImageRequestService : public QObject
{
	Q_OBJECT

	static const quint32 ReceivedImageKeysPerMinuteLimit = 10;
	quint32 ReceivedImageKeysCount;

	QWeakPointer<AccountManager> CurrentAccountManager;

private slots:
	void accountRegistered(Account account);
	void accountUnregistered(Account account);

	void chatImageKeyReceived(const QString &id, const ChatImageKey &imageKey);

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
	 * @short Sets account manager to use by this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param accountManager account manager to use by this service
	 *
	 * This service will listen on registering/unregistering accounts to connect to ChatImageService instances
	 * from these accounts.
	 */
	void setAccountManager(AccountManager *accountManager);

};

/**
 * @}
 */

#endif // CHAT_IMAGE_REQUEST_SERVICE_H
