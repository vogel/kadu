/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "account-notification.h"

static QString getAccountName(const QObject * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification && notification->account()
		? notification->account()->name()
		: QString::null;
}

static QString getProtocolName(const QObject * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification &&
			notification->account() &&
			notification->account()->protocol() &&
			notification->account()->protocol()->protocolFactory()
		? notification->account()->protocol()->protocolFactory()->displayName()
		: QString::null;
}

AccountNotification::AccountNotification(Account *account, const QString &type, const QIcon &icon) :
		Notification(type, icon), CurrentAccount(account)
{
	Parser::registerObjectTag("protocol", getProtocolName);
	Parser::registerObjectTag("account", getAccountName);
}

AccountNotification::~AccountNotification()
{
	Parser::unregisterObjectTag("protocol", getProtocolName);
	Parser::unregisterObjectTag("account", getAccountName);
}
