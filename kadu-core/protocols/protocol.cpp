/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtGui/QTextDocument>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-details-conference.h"
#include "chat/chat-details-simple.h"
#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-state-machine.h"
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

	connect(Machine, SIGNAL(loggingInStateEntered()), this, SLOT(loggingInStateEntered()));
	connect(Machine, SIGNAL(loggedInStateEntered()), this, SLOT(loggedInStateEntered()));
	connect(Machine, SIGNAL(loggingOutStateEntered()), this, SLOT(loggingOutStateEntered()));
	connect(Machine, SIGNAL(loggedOutOnlineStateEntered()), this, SLOT(loggedOutAnyStateEntered()));
	connect(Machine, SIGNAL(loggedOutOfflineStateEntered()), this, SLOT(loggedOutAnyStateEntered()));
	connect(Machine, SIGNAL(wantToLogInStateEntered()), this, SLOT(wantToLogInStateEntered()));
	connect(Machine, SIGNAL(passwordRequiredStateEntered()), this, SLOT(passwordRequiredStateEntered()));
}

Protocol::~Protocol()
{
}

KaduIcon Protocol::icon()
{
	return Factory->icon();
}

void Protocol::prepareStateMachine()
{
	if (!CurrentStatus.isDisconnected())
		emit stateMachineChangeStatus();
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
	if (Core::instance()->isClosing())
		return;

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

void Protocol::disconnectedCleanup()
{
	setAllOffline();
}

void Protocol::setStatus(Status status)
{
	LoginStatus = status;
	doSetStatus(status);
}

void Protocol::doSetStatus(Status status)
{
	CurrentStatus = status;
	if (!CurrentStatus.isDisconnected())
	{
		emit statusChanged(CurrentAccount, CurrentStatus);
		sendStatusToServer();

		emit stateMachineChangeStatus();
	}
	else
		emit stateMachineLogout();
}

Status Protocol::loginStatus() const
{
	return LoginStatus;
}

Status Protocol::status() const
{
	return CurrentStatus;
}

void Protocol::loggedIn()
{
	emit stateMachineLoggedIn();
}

void Protocol::loggedOut()
{
	emit stateMachineLoggedOut();
}

void Protocol::passwordRequired()
{
	emit stateMachinePasswordRequired();
}

void Protocol::connectionError()
{
	statusChanged(Status());

	emit stateMachineConnectionError();
}

void Protocol::connectionClosed()
{
	doSetStatus(Status());
	statusChanged(Status());

	emit stateMachineConnectionClosed();
}

void Protocol::statusChanged(Status status)
{
	CurrentStatus = status;
	emit statusChanged(CurrentAccount, CurrentStatus);
}

KaduIcon Protocol::statusIcon()
{
	return statusIcon(CurrentStatus);
}

KaduIcon Protocol::statusIcon(const Status &status)
{
	return StatusTypeManager::instance()->statusIcon(statusPixmapPath(), status);
}

void Protocol::loggingInStateEntered()
{
	// this may be called from our connection error-handling code, when user wants to be logged in
	// at any cost, so we should assume that we were just disconnected
	// better do some cleanup then
	disconnectedCleanup();

	if (!CurrentAccount.details() || account().id().isEmpty())
	{
		emit stateMachineConnectionClosed();
		return;
	}

	if (!account().hasPassword())
	{
		emit stateMachinePasswordRequired();
		return;
	}

	// just for status icon now, this signal need to be better
	emit statusChanged(CurrentAccount, CurrentStatus);

	// call protocol implementation
	login();
}

void Protocol::loggedInStateEntered()
{
	afterLoggedIn();

	statusChanged(loginStatus());
	sendStatusToServer();

	emit connected(CurrentAccount);
}

void Protocol::loggingOutStateEntered()
{
	// call protocol implementation
	logout();
}

void Protocol::loggedOutAnyStateEntered()
{
	disconnectedCleanup();
	statusChanged(loginStatus());

	emit disconnected(CurrentAccount);
}

void Protocol::wantToLogInStateEntered()
{
	disconnectedCleanup();
	statusChanged(Status());

	emit statusChanged(CurrentAccount, Status());
}

void Protocol::passwordRequiredStateEntered()
{
	emit invalidPassword(CurrentAccount);
}

bool Protocol::isConnected()
{
	return Machine->isLoggedIn();
}

bool Protocol::isConnecting()
{
	return Machine->isLoggingIn();
}
