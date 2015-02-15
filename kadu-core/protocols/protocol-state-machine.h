/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QTimer>

class Protocol;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class ProtocolStateMachine
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for managing protocol connection and login state.
 *
 * This class is responsible for managing protocol connection and login life-cycle. State machine is aware
 * of network status (online and offline) and can reconnect accouts as network comes back to online state.
 * It is possible because of Want to log in state that is used when network is offline and user changes
 * protocol status to not-offline, or if protocol was disconnected because of network error.
 *
 * This class also manager password request for user, when no valid password was provided for given account.
 *
 * Online/offline notifications comes from NetworkManager singleton, all other signals from Protocol class
 * associated with this object.
 *
 * State machine for protocol consists of 7 states:
 * <dl>
 *   <dt>Logging out</dt>
 *   <dd>
 *     This state is used when protocol object is disconnecting from network. Protocol starts
 *     disconnecting when this state is entered, see Protocol::logout(). Possible transitions:
 *     <ul>
 *       <li>network goes offline -> Logged out, offline</li>
 *       <li>protocol logged of properly -> logged out, online</li>
 *     </ul>
 *   </dd>
 *   <dt>Logged out, online</dt>
 *   <dd>
 *     This state is used when protocol object is disconnected from network and network is online.
 *     This state is entered after each successfull logout or when network comes back from offline
 *     state. Entering this state causes all contacts from this protocol to be offline.
 *     Possible transitions:
 *     <ul>
 *       <li>network goes offline -> Logged out, offline</li>
 *       <li>protocol changes state to not offline state -> logging in</li>
 *     </ul>
 *   </dd>
 *   <dt>Logged out, offline</dt>
 *   <dd>
 *     This state is used when protocol object is disconnected from network and network is offline.
 *     This state is entered after network goes offline when protocol was disconnected or disconnecting.
 *     In other cases Want to log in state is used after offline. Entering this state causes all contacts
 *     from this protocol to be offline. Possible transitions:
 *     <ul>
 *       <li>network goes online -> Logged out, online</li>
 *       <li>protocol changes state to not offline state -> want to log in</li>
 *     </ul>
 *   </dd>
 *   <dt>Want to log in</dt>
 *   <dd>
 *     This state is used when protocol object is disconnected from network, network is offline and
 *     protocol object requested connection as soon as possible (this can be caused by user changing state
 *     or by network error). Connection will be possible only when network goes online Entering this state
 *     causes all contacts from this protocol to be offline. Possible transitions:
 *     <ul>
 *       <li>network goes online -> logging in</li>
 *       <li>do-not-belive-we-are-offline timer elapses -> logging in maby online</li>
 *       <li>protocol changes state to offline state -> logged out, offline</li>
 *     </ul>
 *   </dd>
 *   <dt>Logging in</dt>
 *   <dd>
 *     This state is used when protocol object is connecting to network. Protocol starts connecting when
 *     this state is entered, see Protocol::login(). Possible transitions:
 *     <ul>
 *       <li>network goes offline -> want to log in</li>
 *       <li>protocol logged in -> logged in</li>
 *       <li>protocol logged out -> logged out, online</li>
 *       <li>protocol password required -> password required</li>
 *       <li>protocol connection error -> Logging in (delay)</li>
 *       <li>protocol connection closed -> logged out, online</li>
 *     </ul>
 *   </dd>
 *   <dt>Logging in delay</dt>
 *   <dd>
 *     This state is used to delay login in a half second after previous unsuccessfull attempt. Possible transitions:
 *     <ul>
 *       <li>network goes offline -> want to log in</li>
 *       <li>delay timer elasped -> logging in</li>
 *       <li>protocol logged in -> logged in</li>
 *       <li>protocol logged out -> logged out, online</li>
 *       <li>protocol password required -> password required</li>
 *       <li>protocol connection closed -> logged out, online</li>
 *     </ul>
 *   </dd>
 *   <dt>Logging in maybe online</dt>
 *   <dd>
 *     This state is used to try to login in when network state is offline but we dont belive this state (as ntrack has
 *     some bugs). Possible transitions:
 *     <ul>
 *       <li>protocol logged in -> logged in</li>
 *       <li>protocol logged out -> logged out, online</li>
 *       <li>protocol password required -> password required</li>
 *       <li>protocol connection closed -> want to log in</li>
 *       <li>protocol connection error -> want to log in</li>
 *     </ul>
 *   </dd>
 *   <dt>Logged in</dt>
 *   <dd>
 *     This state is used when protocol is logged in. This state can be only entered after Logging in state
 *     and is used by protocol objects to set up services, rosters and other data, see Protocol::afterLoggedIn().
 *     Possible transitions:
 *     <ul>
 *       <li>network goes offline -> want to log in</li>
 *       <li>protocol changes state to offline state -> logging out</li>
 *       <li>protocol connection error -> Logging in (another try)</li>
 *       <li>protocol connection closed -> logged out, online</li>
 *     </ul>
 *   </dd>
 *   <dt>Password required</dt>
 *   <dd>
 *     This state is used when protocol tries to login, but valid password is not availble. Entering this state
 *     causes password dialog to be displayed. Possible transitions:
 *     <ul>
 *       <li>network goes offline -> want to log in</li>
 *       <li>protocol logged out -> logged out, online</li>
 *       <li>protocol connection closed -> logged out, online</li>
 *       <li>password available -> logging in</li>
 *       <li>password not available -> logged out, online</li>
 *     </ul>
 *   </dd>
 * <dl>
 *
 * 11 transitions are defined:
 * <dl>
 *   <dt>Network goes online</dt>
 *   <dd>Caused by NetworkManager::online() signals</dd>
 *   <dt>Network goes offline</dt>
 *   <dd>Caused by NetworkManager::offline() signals</dd>
 *   <dt>Protocol changes state to not offline state</dt>
 *   <dd>Caused by Protocol::stateMachineChangeStatus()</dd>
 *   <dt>Protocol changes state to offline state</dt>
 *   <dd>Caused by Protocol::stateMachineLogout()</dd>
 *   <dt>Protocol logged in</dt>
 *   <dd>Caused by Protocol::stateMachineLoggedIn()</dd>
 *   <dt>Protocol logged out</dt>
 *   <dd>Caused by Protocol::stateMachineLoggedOut()</dd>
 *   <dt>Protocol password required</dt>
 *   <dd>Caused by Protocol::stateMachinePasswordRequired()</dd>
 *   <dt>Password available</dt>
 *   <dd>Caused by Protocol::stateMachinePasswordAvailable()</dd>
 *   <dt>Password not available</dt>
 *   <dd>Caused by Protocol::stateMachinePasswordNotAvailable()</dd>
 *   <dt>Protocol connection error</dt>
 *   <dd>Caused by Protocol::stateMachineConnectionError()</dd>
 *   <dt>Protocol connection closed</dt>
 *   <dd>Caused by Protocol::stateMachineConnectionClosed()</dd>
 * </dl>
 */
class ProtocolStateMachine : public QStateMachine
{
	Q_OBJECT

	Protocol *CurrentProtocol;

	QTimer TryToGoOnlineTimer;
	QTimer DelayTimer;

	QState *LoggingOutState;
	QState *LoggedOutOnlineState;
	QState *LoggedOutOfflineState;
	QState *WantToLogInState;
	QState *PasswordRequiredState;
	QState *LoggingInState;
	QState *LoggingInDelayState;
	QState *LoggingInMaybeOnlineState;
	QState *LoggedInState;

private slots:
	void printConfiguration();

public:
	explicit ProtocolStateMachine(Protocol *protocol);
	virtual ~ProtocolStateMachine();

	bool isLoggedIn();
	bool isLoggingIn();
	bool isLoggingOut();

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when logging out state is entered.
	 *
	 * Signal emited when logging out state is entered. Protocol implementations should use it
	 * to start logging out.
	 */
	void loggingOutStateEntered();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when logged out, online state is entered.
	 *
	 * Signal emited when logged out, online state is entered. Protocol implementations should use it
	 * to do clean-up of resources after logging out.
	 */
	void loggedOutOnlineStateEntered();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when logged out, offline state is entered.
	 *
	 * Signal emited when logged out, offline state is entered. Protocol implementations should use it
	 * to do clean-up of resources after logging out.
	 */
	void loggedOutOfflineStateEntered();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when want to log in state is entered.
	 *
	 * Signal emited when want to log in state is entered. Protocol implementations should use it
	 * to do clean-up of resources after disconnecting.
	 */
	void wantToLogInStateEntered();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when logging in state is entered.
	 *
	 * Signal emited when logging in state is entered. Protocol implementations should use it
	 * to start logging in.
	 */
	void loggingInStateEntered();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when logged in state is entered.
	 *
	 * Signal emited when logged in state is entered. Protocol implementations should use it
	 * to do after-log-in work (like asking server for roster).
	 */
	void loggedInStateEntered();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when password required state is entered.
	 *
	 * Signal emited when logging password required is entered. Protocol implementations should use it
	 * to get password data from user.
	 */
	void passwordRequiredStateEntered();

};

/**
 * @}
 */

#endif // PROTOCOL_STATE_MACHINE_H
