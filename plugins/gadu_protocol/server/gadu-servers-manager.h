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

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtNetwork/QHostAddress>
#include <injeqt/injeqt.h>

#include "../gadu-exports.h"

#include "configuration/configuration-aware-object.h"

class Configuration;
class PathsProvider;

class GADUAPI GaduServersManager : public QObject, public ConfigurationAwareObject
{
	Q_OBJECT

public:
	typedef QPair<QHostAddress, int> GaduServer;

	Q_INVOKABLE explicit GaduServersManager(QObject *parent = nullptr);
	virtual ~GaduServersManager();

	const QList<GaduServer> & getServersList();
	QPair<QHostAddress, int> getServer();
	void markServerAsGood(GaduServer server);
	void markServerAsBad(GaduServer server);

	void buildServerList();

protected:
	virtual void configurationUpdated();

private:
	QPointer<Configuration> m_configuration;
	QPointer<PathsProvider> m_pathsProvider;

	QList<GaduServer> AllServers;
	QList<GaduServer> GoodServers;
	QList<GaduServer> BadServers;

	QList<GaduServer> gaduServersFromString(const QString &serverAddress);

	void loadServerListFromFile(const QString &fileName);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

};
