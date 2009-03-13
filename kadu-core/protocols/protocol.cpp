/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QIcon>
#include <QtGui/QTextDocument>

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "icons_manager.h"
#include "protocols/protocol_factory.h"
#include "protocols/status.h"

#include "protocol.h"

Protocol::Protocol(Account *account, ProtocolFactory *factory)
	: State(NetworkDisconnected), Factory(factory), CurrentAccount(account)
{
	CurrentAccount->setProtocol(this);
}

Protocol::~Protocol()
{
}

QIcon Protocol::icon()
{
	QString iconName = Factory->iconName();
	return iconName.isEmpty()
		? QIcon()
		: icons_manager->loadIcon(iconName);
}

void Protocol::setAllOffline()
{
	Status status(Status::Offline);
	Status oldStatus;
	ContactAccountData *data;

	foreach (Contact contact, ContactManager::instance()->contacts(CurrentAccount, true))
	{
		data = contact.accountData(CurrentAccount);
		oldStatus = data->status();
		data->setStatus(status);
		emit contactStatusChanged(CurrentAccount, contact, oldStatus);
	}
}

void Protocol::setStatus(Status status)
{
	NextStatus = status;
	changeStatus();
}

void Protocol::statusChanged(Status status)
{
	CurrentStatus = status;
	emit statusChanged(CurrentAccount, CurrentStatus);
}

void Protocol::setPrivateMode(bool privateMode)
{
	if (PrivateMode != privateMode)
	{
		PrivateMode = privateMode;
		changePrivateMode();
	}
}

void Protocol::networkStateChanged(NetworkState state)
{
	if (State == state)
		return;

	State = state;
	switch (State)
	{
		case NetworkConnecting:
			emit connecting(CurrentAccount);
			break;
		case NetworkConnected:
			emit connected(CurrentAccount);
			break;
		case NetworkDisconnecting:
			emit disconnecting(CurrentAccount);
			break;
		case NetworkDisconnected:
			emit disconnected(CurrentAccount);
			break;
	}
}
