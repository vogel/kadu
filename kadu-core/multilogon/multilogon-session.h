/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef MULTILOGON_SESSION_H
#define MULTILOGON_SESSION_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtNetwork/QHostAddress>

#include "accounts/account.h"
#include "exports.h"

class KADUAPI MultilogonSession : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MultilogonSession)

	Account MyAccount;
	QString Name;
	QHostAddress RemoteAddress;
	QDateTime LogonTime;

protected:
	void setName(const QString &name);
	void setRemoteAddres(const QHostAddress &remoteAddress);
	void setLogonTime(const QDateTime &logonTime);

public:
	explicit MultilogonSession(Account account, QObject *parent = 0);
	virtual ~MultilogonSession();

	Account account() const;
	const QString & name() const;
	const QHostAddress & remoteAddress() const;
	const QDateTime & logonTime() const;

};

Q_DECLARE_METATYPE(MultilogonSession *)

#endif // MULTILOGON_SESSION_H
