/*
 * %kadu copyright begin%
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "account-notification.h"

static QString getAccountName(const QObject * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification && !notification->account().isNull()
			? notification->account().accountIdentity().name()
			: QString();
}

static QString getProtocolName(const QObject * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification &&
			!notification->account().isNull() &&
			notification->account().protocolHandler() &&
			notification->account().protocolHandler()->protocolFactory()
		? notification->account().protocolHandler()->protocolFactory()->displayName()
		: QString();
}

AccountNotification::AccountNotification(Account account, const QString &type, const QString &iconPath) :
		Notification(type, iconPath), CurrentAccount(account)
{
	Parser::registerObjectTag("protocol", getProtocolName);
	Parser::registerObjectTag("account", getAccountName);
}

AccountNotification::~AccountNotification()
{
	Parser::unregisterObjectTag("protocol", getProtocolName);
	Parser::unregisterObjectTag("account", getAccountName);
}
