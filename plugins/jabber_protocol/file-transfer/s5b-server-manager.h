/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef S5B_SERVER_MANAGER_H
#define S5B_SERVER_MANAGER_H

#include <QtCore/QStringList>

#include <iris/s5b.h>

#include "configuration/configuration-aware-object.h"

class S5BServerManager : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(S5BServerManager)

	static S5BServerManager * Instance;

	XMPP::S5BServer *Server;
	QStringList Addresses;

	quint16 Port;
	QString ExternalAddress;

	S5BServerManager();
	virtual ~S5BServerManager();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	static void createInstance();
	static void destroyInstance();

	static S5BServerManager * instance() { return Instance; }

	XMPP::S5BServer * server() { return Server; }

	void addAddress(const QString &address);
	void removeAddress(const QString &address);

};

#endif // S5B_SERVER_MANAGER_H
