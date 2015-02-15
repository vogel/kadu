/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "multilogon-session.h"

MultilogonSession::MultilogonSession(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
}

MultilogonSession::~MultilogonSession()
{
}

void MultilogonSession::setName(const QString &name)
{
	Name = name;
}

void MultilogonSession::setRemoteAddres(const QHostAddress &remoteAddress)
{
	RemoteAddress = remoteAddress;
}

void MultilogonSession::setLogonTime(const QDateTime &logonTime)
{
	LogonTime = logonTime;
}

Account MultilogonSession::account() const
{
	return MyAccount;
}

const QString & MultilogonSession::name() const
{
	return Name;
}

const QHostAddress & MultilogonSession::remoteAddress() const
{
	return RemoteAddress;
}

const QDateTime & MultilogonSession::logonTime() const
{
	return LogonTime;
}

#include "moc_multilogon-session.cpp"
