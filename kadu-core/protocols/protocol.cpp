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
#include "protocols/state-machine/protocol-state-machine.h"
#include "protocols/protocol-factory.h"
#include "status/status-changer-manager.h"
#include "status/status-type-manager.h"
#include "status/status.h"
#include "debug.h"

#include "protocol.h"

Protocol::Protocol(Account account, ProtocolFactory *factory) :
		Factory(factory), CurrentAccount(account)
{
	Machine = new ProtocolStateMachine(this);
	/*
	 * after machine is started we need to re-call changeStatus
	 * so proper transition can be called
	 *
	 * changeStatus was probably called before machine was started by some StatusContainer
	 * that just restored status from configuration file
	 */
	connect(Machine, SIGNAL(started()), this, SLOT(prepareStateMachine()), Qt::QueuedConnection);
	connect(Machine, SIGNAL(requestPassword()), this, SLOT(passwordRequired()));
	connect(Machine, SIGNAL(connected()), this, SLOT(connectedSlot()));
	connect(Machine, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));

	connect(Machine, SIGNAL(loggingInStateEntered()), this, SLOT(login()));
	connect(Machine, SIGNAL(loggedInStateEntered()), this, SLOT(changeStatus()));

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

void Protocol::prepareStateMachine()
{
	if (!CurrentStatus.isDisconnected())
		emit stateMachineChangeStatusToNotOffline();
}

void Protocol::passwordRequired()
{
	emit invalidPassword(CurrentAccount);
}

void Protocol::passwordProvided()
{
	if (CurrentAccount.hasPassword())
		emit stateMachinePasswordAvailable();
	else
		emit stateMachinePasswordNotAvailable();
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
	return StatusChangerManager::instance()->realStatus(account().statusContainer());
}

void Protocol::statusChanged(StatusContainer *container, Status status)
{
	if (!container || container != account().statusContainer() || CurrentStatus == status)
		return;

	CurrentStatus = status;
	if (CurrentStatus.isDisconnected())
		emit stateMachineChangeStatusToOffline();
	else
		emit stateMachineChangeStatusToNotOffline();
}

void Protocol::statusChanged(Status status)
{
	CurrentStatus = status;
	emit statusChanged(CurrentAccount, CurrentStatus);
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

void Protocol::login()
{
	// just for status icon now, this signal need to be better
	emit statusChanged(CurrentAccount, CurrentStatus);
}

void Protocol::connectedSlot()
{
	emit connected(CurrentAccount);
}

void Protocol::disconnectedSlot()
{
	emit disconnected(CurrentAccount);
}

bool Protocol::isConnected()
{
	return Machine->isLoggedIn();
}

bool Protocol::isConnecting()
{
	return Machine->isLoggingIn();
}
