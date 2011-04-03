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

#ifndef PROTOCOL_STATE_MACHINE_H
#define PROTOCOL_STATE_MACHINE_H

#include <QtCore/QStateMachine>

#include "network/network-aware-object.h"

class Protocol;

class ProtocolStateMachine : public QStateMachine, private NetworkAwareObject
{
	Q_OBJECT

	Protocol *CurrentProtocol;

	QState *LoggedOutState;
	QState *WantToLogInState;
	QState *PasswordRequiredState;
	QState *LoggingInState;
	QState *LoggedInState;

private slots:
	void loggedOutStateEnteredSlot();
	void wantToLogInStateEnteredSlot();
	void loggingInStateEnteredSlot();
	void loggedInStateEnteredSlot();
	void passwordRequiredStateEnteredSlot();

protected:
	void onlineStateChanged(bool online);

public:
	explicit ProtocolStateMachine(Protocol *protocol, QObject *parent = 0);
	virtual ~ProtocolStateMachine();

	void loggedIn();
	void passwordRequired();
	void connectionError();
	void fatalConnectionError();

	bool isLoggedIn();
	bool isLoggingIn();

signals:
	void networkOnlineSignal();
	void networkOfflineSignal();
	void loggedInSignal();
	void passwordRequiredSignal();
	void passwordAvailableSignal();
	void connectionErrorSignal();
	void fatalConnectionErrorSignal();

	void connected();
	void disconnected();
	void requestPassword();

	void loggedOutStateEntered();
	void wantToLogInStateEntered();
	void loggingInStateEntered();
	void loggedInStateEntered();
	void passwordRequiredStateEntered();

};

#endif // PROTOCOL_STATE_MACHINE_H
