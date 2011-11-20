/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtScript/QScriptEngine>

#include "configuration/configuration-file.h"
#include "network/proxy/network-proxy-manager.h"

#include "scripts/network-reply-wrapper.h"

#include "network-access-manager-wrapper.h"

NetworkAccessManagerWrapper::NetworkAccessManagerWrapper(QScriptEngine *engine, QObject *parent) :
		QNetworkAccessManager(parent), Engine(engine), Utf8(false)
{
	configurationUpdated();
}

NetworkAccessManagerWrapper::~NetworkAccessManagerWrapper()
{
}

void NetworkAccessManagerWrapper::configurationUpdated()
{
	NetworkProxy networkProxy;

	if (config_file.readBoolEntry("SMS", "DefaultProxy", true))
		networkProxy = NetworkProxyManager::instance()->defaultProxy();
	else
		networkProxy = NetworkProxyManager::instance()->byUuid(config_file.readEntry("SMS", "Proxy"));

	QNetworkProxy proxy;

	if (networkProxy)
	{
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(networkProxy.address());
		proxy.setPort(networkProxy.port());
		proxy.setUser(networkProxy.user());
		proxy.setPassword(networkProxy.password());
	}
	else
		proxy.setType(QNetworkProxy::NoProxy);

	setProxy(proxy);
}

QScriptValue NetworkAccessManagerWrapper::get(const QString &url)
{
	return Engine->newQObject(new NetworkReplyWrapper(QNetworkAccessManager::get(QNetworkRequest(url))));
}

void NetworkAccessManagerWrapper::setHeader(const QString &headerName, const QString &headerValue)
{
	// Note that QtScript by default doesn't support conversion to QByteArray,
	// so we cannot simply convert arguments to QByteArray.
	Headers.insert(headerName.toAscii(), headerValue.toAscii());
}

void NetworkAccessManagerWrapper::clearHeaders()
{
	Headers.clear();
}

QScriptValue NetworkAccessManagerWrapper::post(const QString &url, const QString &data)
{
	QNetworkRequest request;
	request.setUrl(url);
	for (QMap<QByteArray, QByteArray>::const_iterator i = Headers.constBegin(); i != Headers.constEnd(); ++i)
		request.setRawHeader(i.key(), i.value());

	QByteArray requestData;
	if (Utf8)
		requestData = data.toUtf8();
	else
		requestData = data.toAscii();

	return Engine->newQObject(new NetworkReplyWrapper(QNetworkAccessManager::post(request, requestData)));
}
