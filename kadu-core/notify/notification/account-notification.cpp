/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "identities/identity.h"
#include "parser/parser.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "account-notification.h"

static QString getAccountName(const ParserData * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification && !notification->account().isNull()
			? notification->account().accountIdentity().name()
			: QString();
}

static QString getProtocolName(const ParserData * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification &&
			!notification->account().isNull() &&
			notification->account().protocolHandler() &&
			notification->account().protocolHandler()->protocolFactory()
		? notification->account().protocolHandler()->protocolFactory()->displayName()
		: QString();
}

void AccountNotification::registerParserTags()
{
	Parser::registerObjectTag("protocol", getProtocolName);
	Parser::registerObjectTag("account", getAccountName);
}

void AccountNotification::unregisterParserTags()
{
	Parser::unregisterObjectTag("protocol");
	Parser::unregisterObjectTag("account");
}

AccountNotification::AccountNotification(Account account, const QString &type, const KaduIcon &icon) :
		Notification(type, icon), CurrentAccount(account)
{
}

AccountNotification::~AccountNotification()
{
}

#include "moc_account-notification.cpp"
