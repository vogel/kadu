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

class ProtocolStateMachine : public QStateMachine, private NetworkAwareObject
{
	Q_OBJECT

	QState *LoggedOutState;
	QState *WantToLogInState;
	QState *LoggingInState;
	QState *LoggedInState;

private slots:
	void loggedOutStateEntered();
	void wantToLogInStateEntered();
	void loggingInStateEntered();
	void loggedInStateEntered();

protected:
	void onlineStateChanged(bool online);

public:
	explicit ProtocolStateMachine(QObject *parent = 0);
	virtual ~ProtocolStateMachine();

	void wantToLogin();
	void loggedIn();
	void loggedOut();

signals:
	void networkOnlineSignal();
	void networkOfflineSignal();
	void wantToLoginOnlineSignal();
	void wantToLoginOfflineSignal();
	void loggedInSignal();
	void loggedOutSignal();

	void login();

};

#endif // PROTOCOL_STATE_MACHINE_H
