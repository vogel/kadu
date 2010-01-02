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

#include "accounts/account-manager.h"
#include "chat/chat-details-conference.h"
#include "chat/chat-details-simple.h"
#include "chat/chat-manager.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "icons-manager.h"
#include "protocols/protocol-factory.h"
#include "status/status-changer-manager.h"
#include "status/status.h"
#include "debug.h"

#include "protocol.h"

Protocol::Protocol(Account account, ProtocolFactory *factory) :
		State(NetworkDisconnected), Factory(factory), CurrentAccount(account)
{
	connect(StatusChangerManager::instance(), SIGNAL(statusChanged(StatusContainer*,Status)),
			this, SLOT(statusChanged(StatusContainer*,Status)));
}

Protocol::~Protocol()
{
}

QIcon Protocol::icon()
{
	QString iconName = Factory->iconName();
	return iconName.isEmpty()
		? QIcon()
		: IconsManager::instance()->loadIcon(iconName);
}

void Protocol::setAllOffline()
{
	Status status;
	Status oldStatus;

	foreach (const Contact &contact, ContactManager::instance()->contacts(CurrentAccount))
	{
		oldStatus = contact.currentStatus();

		if (oldStatus != status)
		{
			contact.setCurrentStatus(status);
			emit contactStatusChanged(contact, oldStatus);
		}
	}
}

void Protocol::setStatus(Status status)
{
	StatusChangerManager::instance()->setStatus(account().statusContainer(), status);
}

Status Protocol::status() const
{
	return CurrentStatus;
}

Status Protocol::nextStatus() const
{
	return StatusChangerManager::instance()->status(account().statusContainer());
}

void Protocol::statusChanged(StatusContainer *container, Status status)
{
	if (container && container == account().statusContainer() && CurrentStatus != status)
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
