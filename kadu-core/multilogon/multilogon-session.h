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

#ifndef MULTILOGON_SESSION_H
#define MULTILOGON_SESSION_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtNetwork/QHostAddress>

#include "status/status.h"
#include "exports.h"

class KADUAPI MultilogonSession
{
	Q_DISABLE_COPY(MultilogonSession)

	QString Name;
	QHostAddress RemoteAddress;
	Status RemoteStatus;
	QDateTime LogonTime;

protected:
	void setName(const QString &name);
	void setRemoteAddres(const QHostAddress &remoteAddress);
	void setRemoteStatus(const Status &status);
	void setLogonTime(const QDateTime &logonTime);

public:
	MultilogonSession();
	virtual ~MultilogonSession();

	QString getName();
	QHostAddress getRemoteAddress();
	Status getRemoteStatus();
	QDateTime getLogonTime();

};

#endif // MULTILOGON_SESSION_H
