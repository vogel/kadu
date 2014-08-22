/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "services/image-storage-service.h"

#include "chat-view-network-access-manager.h"

ChatViewNetworkAccessManager::ChatViewNetworkAccessManager(QNetworkAccessManager *manager, QObject *parent) :
		QNetworkAccessManager(parent)
{
	setCache(manager->cache());
	setCookieJar(manager->cookieJar());
	setProxy(manager->proxy());
	setProxyFactory(manager->proxyFactory());
}

ChatViewNetworkAccessManager::~ChatViewNetworkAccessManager()
{
}

void ChatViewNetworkAccessManager::setImageStorageService(ImageStorageService *imageStorageService)
{
	CurrentImageStorageService = imageStorageService;
}

QNetworkReply * ChatViewNetworkAccessManager::createRequest(QNetworkAccessManager::Operation operation, const QNetworkRequest &request, QIODevice *device)
{
	if (QNetworkAccessManager::GetOperation != operation && QNetworkAccessManager::HeadOperation != operation)
		operation = QNetworkAccessManager::GetOperation;

	if (!CurrentImageStorageService)
		return QNetworkAccessManager::createRequest(operation, request, device);

	QNetworkRequest newRequest(request);
	newRequest.setUrl(CurrentImageStorageService.data()->toFileUrl(request.url()));

	return QNetworkAccessManager::createRequest(operation, newRequest, device);
}

#include "moc_chat-view-network-access-manager.cpp"
