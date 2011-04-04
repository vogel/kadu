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

class Protocol;

class ProtocolStateMachine : public QStateMachine
{
	Q_OBJECT

	Protocol *CurrentProtocol;

	QState *LoggingOutState;
	QState *LoggedOutOnlineState;
	QState *LoggedOutOfflineState;
	QState *WantToLogInState;
	QState *PasswordRequiredState;
	QState *LoggingInState;
	QState *LoggedInState;

public:
	explicit ProtocolStateMachine(Protocol *protocol, QObject *parent = 0);
	virtual ~ProtocolStateMachine();

	bool isLoggedIn();
	bool isLoggingIn();

signals:
	void requestPassword();

	void loggingOutStateEntered();
	void loggedOutOnlineStateEntered();
	void loggedOutOfflineStateEntered();
	void wantToLogInStateEntered();
	void loggingInStateEntered();
	void loggedInStateEntered();
	void passwordRequiredStateEntered();

};

#endif // PROTOCOL_STATE_MACHINE_H
