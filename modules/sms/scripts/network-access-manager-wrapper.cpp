/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
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

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtScript/QScriptEngine>

#include "scripts/network-reply-wrapper.h"

#include "network-access-manager-wrapper.h"

NetworkAccessManagerWrapper::NetworkAccessManagerWrapper(QScriptEngine *engine, QObject *parent) :
	QNetworkAccessManager(parent), Engine(engine)
{
	Unicode = false;
}

NetworkAccessManagerWrapper::~NetworkAccessManagerWrapper()
{
}

void NetworkAccessManagerWrapper::setUnicode(bool unicode)
{
	Unicode = unicode;
}

QScriptValue NetworkAccessManagerWrapper::get(const QString &url)
{
	return Engine->newQObject(new NetworkReplyWrapper(QNetworkAccessManager::get(QNetworkRequest(url))));
}

QScriptValue NetworkAccessManagerWrapper::post(const QString &url, const QString &data)
{
	QByteArray requestData;
	QNetworkRequest request;
	request.setUrl(url);

	if (Unicode)
	{
		request.setRawHeader("Content-Type", "text/plain; charset=utf-8");
		request.setRawHeader("Accept-Encoding", "gzip, deflate");
		requestData = data.toAscii();
	}
	else
		requestData = data.toUtf8();

	return Engine->newQObject(new NetworkReplyWrapper(QNetworkAccessManager::post(request, requestData)));
}
