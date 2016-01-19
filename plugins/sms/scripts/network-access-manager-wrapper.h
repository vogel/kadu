/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtScript/QScriptValue>
#include <injeqt/injeqt.h>

class Configuration;
class NetworkProxyManager;

class NetworkAccessManagerWrapper : public QNetworkAccessManager, ConfigurationAwareObject
{
	Q_OBJECT

public:
	explicit NetworkAccessManagerWrapper(QScriptEngine *engine, QObject *parent = nullptr);
	virtual ~NetworkAccessManagerWrapper();

public slots:
	void setUtf8(bool utf8) { Utf8 = utf8; }
	void setHeader(const QString &headerName, const QString &headerValue);
	void clearHeaders();

	QScriptValue get(const QString &url);
	QScriptValue post(const QString &url, const QString &data);

protected:
	void configurationUpdated();

private:
	QPointer<Configuration> m_configuration;
	QPointer<NetworkProxyManager> m_networkProxyManager;

	QScriptEngine *Engine;
	bool Utf8;
	QMap<QByteArray, QByteArray> Headers;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setNetworkProxyManager(NetworkProxyManager *networkProxyManager);
	INJEQT_INIT void init();

};
