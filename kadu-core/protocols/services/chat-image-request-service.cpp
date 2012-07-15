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

#include <QtCore/QTimer>

#include "accounts/account-manager.h"
#include "protocols/services/chat-image-service.h"

#include "chat-image-request-service.h"

ChatImageRequestService::ChatImageRequestService(QObject *parent) :
		QObject(parent), ReceivedImageKeysCount(0)
{
	QTimer *everyMinuteTimer = new QTimer(this);
	everyMinuteTimer->setInterval(60 * 1000);
	everyMinuteTimer->setSingleShot(false);
	connect(everyMinuteTimer, SIGNAL(timeout()), this, SLOT(resetReceivedImageKeysCount()));
	everyMinuteTimer->start();
}

ChatImageRequestService::~ChatImageRequestService()
{
}

void ChatImageRequestService::setAccountManager(AccountManager *accountManager)
{
	if (CurrentAccountManager)
		disconnect(CurrentAccountManager.data(), 0, this, 0);

	CurrentAccountManager = accountManager;
	if (!CurrentAccountManager)
		return;

	connect(CurrentAccountManager.data(), SIGNAL(accountRegistered(Account)), this, SLOT(accountRegistered(Account)));
	connect(CurrentAccountManager.data(), SIGNAL(accountUnregistered(Account)), this, SLOT(accountUnregistered(Account)));
}

void ChatImageRequestService::accountRegistered(Account account)
{
	if (!account || !account.protocolHandler() || !account.protocolHandler()->chatImageService())
		return;

	connect(account.protocolHandler()->chatImageService(), SIGNAL(chatImageKeyReceived(QString,ChatImageKey)),
	        this, SLOT(chatImageKeyReceived(QString,ChatImageKey)));
}

void ChatImageRequestService::accountUnregistered(Account account)
{
	if (!account || !account.protocolHandler() || !account.protocolHandler()->chatImageService())
		return;

	disconnect(account.protocolHandler()->chatImageService(), SIGNAL(chatImageKeyReceived(QString,ChatImageKey)),
	           this, SLOT(chatImageKeyReceived(QString,ChatImageKey)));
}

void ChatImageRequestService::chatImageKeyReceived(const QString &id, const ChatImageKey &imageKey)
{
	if (ReceivedImageKeysCount >= ReceivedImageKeysPerMinuteLimit)
		return;

	ChatImageService *service = qobject_cast<ChatImageService *>(sender());
	if (!service)
		return;

	service->requestChatImage(id, imageKey);
	ReceivedImageKeysCount++;
}

void ChatImageRequestService::resetReceivedImageKeysCount()
{
	ReceivedImageKeysCount = 0;
}
