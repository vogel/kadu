/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "protocol-state-machine.h"

ProtocolStateMachine::ProtocolStateMachine(QObject *parent) :
		QStateMachine(parent)
{
	LoggedOutState = new QState(this);
	WantToLogInState = new QState(this);
	LoggingInState = new QState(this);
	LoggedInState = new QState(this);

	connect(LoggedOutState, SIGNAL(entered()), this, SLOT(loggedOutStateEntered()));
	connect(WantToLogInState, SIGNAL(entered()), this, SLOT(wantToLogInStateEntered()));
	connect(LoggingInState, SIGNAL(entered()), this, SLOT(loggingInStateEntered()));
	connect(LoggedInState, SIGNAL(entered()), this, SLOT(loggedInStateEntered()));

	LoggedOutState->addTransition(this, SIGNAL(wantToLoginOfflineSignal()), WantToLogInState);
	LoggedOutState->addTransition(this, SIGNAL(wantToLoginOnlineSignal()), LoggingInState);

	WantToLogInState->addTransition(this, SIGNAL(networkOnlineSignal()), LoggingInState);

	LoggingInState->addTransition(this, SIGNAL(networkOfflineSignal()), WantToLogInState);
	LoggingInState->addTransition(this, SIGNAL(loggedOutSignal()), LoggedOutState);

	LoggedInState->addTransition(this, SIGNAL(networkOfflineSignal()), WantToLogInState);
	LoggingInState->addTransition(this, SIGNAL(loggedOutSignal()), LoggedOutState);

	addState(LoggedOutState);
	addState(WantToLogInState);
	addState(LoggingInState);
	addState(LoggedInState);

	setInitialState(LoggedOutState);

	start();
}

ProtocolStateMachine::~ProtocolStateMachine()
{
}

void ProtocolStateMachine::onlineStateChanged(bool online)
{
	if (online)
		emit networkOnlineSignal();
	else
		emit networkOfflineSignal();
}

void ProtocolStateMachine::wantToLogin()
{
	if (isOnline())
		emit wantToLoginOnlineSignal();
	else
		emit wantToLoginOfflineSignal();
}

void ProtocolStateMachine::loggedOut()
{
	emit loggedOutSignal();
}

#include <stdio.h>
void ProtocolStateMachine::loggedInStateEntered()
{
	printf("ProtocolStateMachine::loggedInStateEntered()\n");
}

void ProtocolStateMachine::loggedOutStateEntered()
{
	printf("ProtocolStateMachine::loggedOutStateEntered()\n");
}

void ProtocolStateMachine::loggingInStateEntered()
{
	emit login();
	printf("ProtocolStateMachine::loggingInStateEntered()\n");
}

void ProtocolStateMachine::wantToLogInStateEntered()
{
	printf("ProtocolStateMachine::wantToLogInStateEntered()\n");
}
