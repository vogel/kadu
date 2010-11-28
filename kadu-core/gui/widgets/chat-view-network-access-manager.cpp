/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "protocols/services/chat-image-service.h"

#include "chat-view-network-access-manager.h"

ChatViewNetworkAccessManager::ChatViewNetworkAccessManager(QNetworkAccessManager *manager, QObject *parent) :
		QNetworkAccessManager(parent)
{
	setCache(manager->cache());
	setCookieJar(manager->cookieJar());
	setProxy(manager->proxy());
	setProxyFactory(manager->proxyFactory());
}

QNetworkReply * ChatViewNetworkAccessManager::createRequest(QNetworkAccessManager::Operation operation, const QNetworkRequest &request, QIODevice *device)
{
	if (request.url().scheme() != "kaduimg")
		return QNetworkAccessManager::createRequest(operation, request, device);

	QUrl newUrl(request.url());
	newUrl.setScheme("file");
	if (!newUrl.path().startsWith("//")) // TODO 0.6.6: it's a hack we need until we're saving sent images in imagesPath
		newUrl.setPath(ChatImageService::imagesPath() + newUrl.path());

	QNetworkRequest newRequest(request);
	newRequest.setUrl(newUrl);

	return QNetworkAccessManager::createRequest(operation, newRequest, device);
}
