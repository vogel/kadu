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

#include "protocols/protocol.h"

#include "protocol-state-machine.h"

ProtocolStateMachine::ProtocolStateMachine(Protocol *protocol, QObject *parent) :
		QStateMachine(parent), CurrentProtocol(protocol)
{
	LoggingOutState = new QState(this);
	LoggedOutState = new QState(this);
	WantToLogInState = new QState(this);
	LoggingInState = new QState(this);
	LoggedInState = new QState(this);
	PasswordRequiredState = new QState(this);

	connect(LoggingOutState, SIGNAL(entered()), this, SLOT(loggingOutStateEnteredSlot()));
	connect(LoggedOutState, SIGNAL(entered()), this, SLOT(loggedOutStateEnteredSlot()));
	connect(WantToLogInState, SIGNAL(entered()), this, SLOT(wantToLogInStateEnteredSlot()));
	connect(LoggingInState, SIGNAL(entered()), this, SLOT(loggingInStateEnteredSlot()));
	connect(LoggedInState, SIGNAL(entered()), this, SLOT(loggedInStateEnteredSlot()));
	connect(PasswordRequiredState, SIGNAL(entered()), this, SLOT(passwordRequiredStateEnteredSlot()));

	connect(LoggingOutState, SIGNAL(entered()), this, SIGNAL(loggingOutStateEntered()));
	connect(LoggedOutState, SIGNAL(entered()), this, SIGNAL(loggedOutStateEntered()));
	connect(WantToLogInState, SIGNAL(entered()), this, SIGNAL(wantToLogInStateEntered()));
	connect(LoggingInState, SIGNAL(entered()), this, SIGNAL(loggingInStateEntered()));
	connect(LoggedInState, SIGNAL(entered()), this, SIGNAL(loggedInStateEntered()));
	connect(PasswordRequiredState, SIGNAL(entered()), this, SIGNAL(passwordRequiredStateEntered()));

	LoggingOutState->addTransition(this, SIGNAL(networkOfflineSignal()), LoggedOutState);
	LoggingOutState->addTransition(CurrentProtocol, SIGNAL(stateMachineLoggedOut()), LoggedOutState);

	// add 2 new states or something?
	LoggedOutState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToNotOffline()), LoggingInState);

	WantToLogInState->addTransition(this, SIGNAL(networkOnlineSignal()), LoggingInState);
	WantToLogInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggingInState);

	LoggingInState->addTransition(this, SIGNAL(networkOfflineSignal()), WantToLogInState);
	LoggingInState->addTransition(this, SIGNAL(loggedInSignal()), LoggedInState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggedOutState);
	LoggingInState->addTransition(this, SIGNAL(passwordRequiredSignal()), PasswordRequiredState);
	LoggingInState->addTransition(this, SIGNAL(connectionErrorSignal()), LoggingInState);
	LoggingInState->addTransition(this, SIGNAL(fatalConnectionErrorSignal()), LoggedOutState);

	LoggedInState->addTransition(this, SIGNAL(networkOfflineSignal()), WantToLogInState);
	// re-enter current state, so protocol implementations can update status message
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToNotOffline()), LoggedInState);
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggingOutState);
	LoggedInState->addTransition(this, SIGNAL(connectionErrorSignal()), LoggingInState);
	LoggedInState->addTransition(this, SIGNAL(fatalConnectionErrorSignal()), LoggedOutState);

	PasswordRequiredState->addTransition(this, SIGNAL(networkOfflineSignal()), WantToLogInState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggedOutState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordAvailable()), LoggingInState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordNotAvailable()), LoggedOutState);
	PasswordRequiredState->addTransition(this, SIGNAL(fatalConnectionErrorSignal()), LoggedOutState);

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

void ProtocolStateMachine::loggedIn()
{
	emit loggedInSignal();
}

void ProtocolStateMachine::passwordRequired()
{
	emit passwordRequiredSignal();
}

void ProtocolStateMachine::connectionError()
{
	emit connectionErrorSignal();
}

void ProtocolStateMachine::fatalConnectionError()
{
	emit fatalConnectionErrorSignal();
}

#include <stdio.h>

void ProtocolStateMachine::loggedInStateEnteredSlot()
{
	printf("ProtocolStateMachine::loggedInStateEntered()\n");

	emit connected();
}

void ProtocolStateMachine::loggingOutStateEnteredSlot()
{
	printf("ProtocolStateMachine::loggingOutStateEnteredSlot()\n");
}

void ProtocolStateMachine::loggedOutStateEnteredSlot()
{
	printf("ProtocolStateMachine::loggedOutStateEntered()\n");

	emit disconnected();
}

void ProtocolStateMachine::loggingInStateEnteredSlot()
{
	printf("ProtocolStateMachine::loggingInStateEntered()\n");
}

void ProtocolStateMachine::wantToLogInStateEnteredSlot()
{
	printf("ProtocolStateMachine::wantToLogInStateEntered()\n");
}

void ProtocolStateMachine::passwordRequiredStateEnteredSlot()
{
	printf("ProtocolStateMachine::passwordRequiredStateEntered()\n");

	emit requestPassword();
}

bool ProtocolStateMachine::isLoggedIn()
{
	return configuration().contains(LoggedInState);
}

bool ProtocolStateMachine::isLoggingIn()
{
	return configuration().contains(LoggingInState);
}
