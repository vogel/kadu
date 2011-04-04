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

#include "network/network-manager.h"
#include "protocols/protocol.h"

#include "protocol-state-machine.h"

ProtocolStateMachine::ProtocolStateMachine(Protocol *protocol, QObject *parent) :
		QStateMachine(parent), CurrentProtocol(protocol)
{
	LoggingOutState = new QState(this);
	LoggedOutOnlineState = new QState(this);
	LoggedOutOfflineState = new QState(this);
	WantToLogInState = new QState(this);
	LoggingInState = new QState(this);
	LoggedInState = new QState(this);
	PasswordRequiredState = new QState(this);

	connect(LoggingOutState, SIGNAL(entered()), this, SIGNAL(loggingOutStateEntered()));
	connect(LoggedOutOnlineState, SIGNAL(entered()), this, SIGNAL(loggedOutOnlineStateEntered()));
	connect(LoggedOutOfflineState, SIGNAL(entered()), this, SIGNAL(loggedOutOfflineStateEntered()));
	connect(WantToLogInState, SIGNAL(entered()), this, SIGNAL(wantToLogInStateEntered()));
	connect(LoggingInState, SIGNAL(entered()), this, SIGNAL(loggingInStateEntered()));
	connect(LoggedInState, SIGNAL(entered()), this, SIGNAL(loggedInStateEntered()));
	connect(PasswordRequiredState, SIGNAL(entered()), this, SIGNAL(passwordRequiredStateEntered()));

	LoggingOutState->addTransition(NetworkManager::instance(), SIGNAL(offline()), LoggedOutOfflineState);
	LoggingOutState->addTransition(CurrentProtocol, SIGNAL(stateMachineLoggedOut()), LoggedOutOnlineState);

	LoggedOutOnlineState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToNotOffline()), LoggingInState);
	LoggedOutOnlineState->addTransition(NetworkManager::instance(), SIGNAL(offline()), LoggedOutOfflineState);

	LoggedOutOfflineState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToNotOffline()), WantToLogInState);
	LoggedOutOfflineState->addTransition(NetworkManager::instance(), SIGNAL(online()), LoggedOutOnlineState);

	WantToLogInState->addTransition(NetworkManager::instance(), SIGNAL(online()), LoggingInState);
	WantToLogInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggedOutOfflineState);

	LoggingInState->addTransition(NetworkManager::instance(), SIGNAL(offline()), WantToLogInState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineLoggedIn()), LoggedInState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggedOutOnlineState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordRequired()), PasswordRequiredState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionError()), LoggingInState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionClosed()), LoggedOutOnlineState);

	LoggedInState->addTransition(NetworkManager::instance(), SIGNAL(offline()), WantToLogInState);
	// re-enter current state, so protocol implementations can update status message
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToNotOffline()), LoggedInState);
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggingOutState);
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionError()), LoggingInState);
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionClosed()), LoggedOutOnlineState);

	PasswordRequiredState->addTransition(NetworkManager::instance(), SIGNAL(offline()), WantToLogInState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatusToOffline()), LoggedOutOnlineState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordAvailable()), LoggingInState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordNotAvailable()), LoggedOutOnlineState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionClosed()), LoggedOutOnlineState);

	if (NetworkManager::instance()->isOnline())
		setInitialState(LoggedOutOnlineState);
	else
		setInitialState(LoggedOutOfflineState);

	start();
}

ProtocolStateMachine::~ProtocolStateMachine()
{
}

bool ProtocolStateMachine::isLoggedIn()
{
	return configuration().contains(LoggedInState);
}

bool ProtocolStateMachine::isLoggingIn()
{
	return configuration().contains(LoggingInState);
}
