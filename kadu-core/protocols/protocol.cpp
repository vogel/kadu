/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "status/status-type-manager.h"
#include "status/status.h"
#include "debug.h"

#include "protocol.h"

Protocol::Protocol(Account account, ProtocolFactory *factory) :
		Factory(factory), CurrentAccount(account), State(NetworkDisconnected)
{
	connect(StatusChangerManager::instance(), SIGNAL(statusChanged(StatusContainer*,Status)),
			this, SLOT(statusChanged(StatusContainer*,Status)));
}

Protocol::~Protocol()
{
}

QIcon Protocol::icon()
{
	return Factory->icon();
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
	return StatusChangerManager::instance()->realStatus(account().statusContainer());
}

void Protocol::statusChanged(StatusContainer *container, Status status)
{
	if (container && container == account().statusContainer() && (CurrentStatus != status || isConnecting()))
		changeStatus();
}

void Protocol::statusChanged(Status status)
{
	CurrentStatus = status;
	emit statusChanged(CurrentAccount, CurrentStatus);
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

QIcon Protocol::statusIcon(Status status)
{
	return StatusTypeManager::instance()->statusIcon(statusPixmapPath(), status.type(),
			!status.description().isEmpty(), false);
}

QString Protocol::statusIconPath(const QString& statusType)
{
	return StatusTypeManager::instance()->statusIconPath(statusPixmapPath(), statusType, false, false);
}

QString Protocol::statusIconFullPath(const QString& statusType)
{
	return StatusTypeManager::instance()->statusIconFullPath(statusPixmapPath(), statusType, false, false);
}

QIcon Protocol::statusIcon(const QString &statusType)
{
	return StatusTypeManager::instance()->statusIcon(statusPixmapPath(), statusType, false, false);
}
