/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"

#include "protocol_notification.h"

QString getAccountName(const QObject * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification &&
		notification->account()
		? notification->account()->name()
		: QString::null;
}

QString getProtocolName(const QObject * const object)
{
	const AccountNotification * const notification = dynamic_cast<const AccountNotification * const>(object);
	return notification &&
		notification->account() &&
		notification->account()->protocol() &&
		notification->account()->protocol()->protocolFactory()
		? notification->account()->protocol()->protocolFactory()->displayName()
		: QString::null;
}

AccountNotification::AccountNotification(const QString &type, const QPixmap &icon, const ContactList &contacts, Account *account)
	: Notification(type, icon, contacts), CurrentAccount(account)
{
	KaduParser::registerObjectTag("protocol", getProtocolName);
	KaduParser::registerObjectTag("account", getAccountName);
}

AccountNotification::~AccountNotification()
{
	KaduParser::unregisterObjectTag("protocol", getProtocolName);
	KaduParser::unregisterObjectTag("account", getAccountName);
}

