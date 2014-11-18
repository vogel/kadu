/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gadu-roster-state-machine.h"

#include "services/gadu-roster-service.h"

#include "protocols/protocol.h"
#include "debug.h"


#include <QtCore/QHistoryState>
#include <QtCore/QStringList>

GaduRosterStateMachine::GaduRosterStateMachine(GaduRosterService *service, Protocol *protocol) :
		QStateMachine{service}
{
	auto globalState = new QState{ParallelStates};

	m_workState = new QState{globalState};
	m_offlineState = new QState{m_workState};
	m_idleState = new QState{m_workState};
	m_putState = new QState{m_workState};
	m_getState = new QState{m_workState};

	m_offlineState->addTransition(protocol, SIGNAL(connected(Account)), m_idleState);
	m_idleState->addTransition(this, SIGNAL(putRequested()), m_putState);
	m_idleState->addTransition(this, SIGNAL(getRequested()), m_getState);
	m_idleState->addTransition(protocol, SIGNAL(disconnected(Account)), m_offlineState);
	m_putState->addTransition(service, SIGNAL(stateMachinePutFinished()), m_idleState);
	m_putState->addTransition(service, SIGNAL(stateMachinePutFailed()), m_idleState);
	m_putState->addTransition(protocol, SIGNAL(disconnected(Account)), m_offlineState);
	m_getState->addTransition(service, SIGNAL(stateMachineGetFinished()), m_idleState);
	m_getState->addTransition(service, SIGNAL(stateMachineGetFailed()), m_idleState);
	m_getState->addTransition(protocol, SIGNAL(disconnected(Account)), m_offlineState);

	m_localState = new QState{globalState};
	m_localCleanState = new QState{m_localState};
	m_localDirtyState = new QState{m_localState};
	m_localCleaningState = new QState{m_localState};
	m_localCleaningDirtyState = new QState{m_localState};
	m_localFailedState = new QState{m_localState};

	m_localCleanState->addTransition(service, SIGNAL(stateMachineLocalDirty()), m_localDirtyState);
	m_localDirtyState->addTransition(this, SIGNAL(putRequested()), m_localCleaningState);
	m_localCleaningState->addTransition(service, SIGNAL(stateMachineLocalDirty()), m_localCleaningDirtyState);
	m_localCleaningState->addTransition(service, SIGNAL(stateMachinePutFinished()), m_localCleanState);
	m_localCleaningState->addTransition(service, SIGNAL(stateMachinePutFailed()), m_localFailedState);
	m_localCleaningState->addTransition(protocol, SIGNAL(disconnected(Account)), m_localDirtyState);
	m_localCleaningDirtyState->addTransition(service, SIGNAL(stateMachinePutFinished()), m_localDirtyState);
	m_localCleaningDirtyState->addTransition(service, SIGNAL(stateMachinePutFailed()), m_localDirtyState);
	m_localCleaningDirtyState->addTransition(protocol, SIGNAL(disconnected(Account)), m_localDirtyState);
	m_localFailedState->addTransition(service, SIGNAL(stateMachineLocalDirty()), m_localDirtyState);
	m_localFailedState->addTransition(protocol, SIGNAL(connected(Account)), m_localDirtyState);
	m_localFailedState->addTransition(protocol, SIGNAL(disconnected(Account)), m_localDirtyState);

	m_remoteState = new QState{globalState};
	m_remoteCleanState = new QState{m_remoteState};
	m_remoteDirtyState = new QState{m_remoteState};
	m_remoteCleaningState = new QState{m_remoteState};
	m_remoteCleaningDirtyState = new QState{m_remoteState};
	m_remoteFailedState = new QState{m_remoteState};

	m_remoteCleanState->addTransition(service, SIGNAL(stateMachineRemoteDirty()), m_remoteDirtyState);
	m_remoteDirtyState->addTransition(this, SIGNAL(getRequested()), m_remoteCleaningState);
	m_remoteCleaningState->addTransition(service, SIGNAL(stateMachineRemoteDirty()), m_remoteCleaningDirtyState);
	m_remoteCleaningState->addTransition(service, SIGNAL(stateMachineGetFinished()), m_remoteCleanState);
	m_remoteCleaningState->addTransition(service, SIGNAL(stateMachineGetFailed()), m_remoteFailedState);
	m_remoteCleaningState->addTransition(protocol, SIGNAL(disconnected(Account)), m_remoteDirtyState);
	m_remoteCleaningDirtyState->addTransition(service, SIGNAL(stateMachineGetFinished()), m_remoteDirtyState);
	m_remoteCleaningDirtyState->addTransition(service, SIGNAL(stateMachineGetFailed()), m_remoteDirtyState);
	m_remoteCleaningDirtyState->addTransition(protocol, SIGNAL(disconnected(Account)), m_remoteDirtyState);
	m_remoteFailedState->addTransition(service, SIGNAL(stateMachineRemoteDirty()), m_remoteDirtyState);
	m_remoteFailedState->addTransition(protocol, SIGNAL(connected(Account)), m_remoteDirtyState);
	m_remoteFailedState->addTransition(protocol, SIGNAL(disconnected(Account)), m_remoteDirtyState);

	m_localState->setInitialState(m_localCleanState);
	m_remoteState->setInitialState(m_remoteDirtyState);
	m_workState->setInitialState(protocol->isConnected() ? m_idleState : m_offlineState);

	connect(m_idleState, SIGNAL(entered()), this, SLOT(checkIfSynchronizationRequired()));
	connect(m_localDirtyState, SIGNAL(entered()), this, SLOT(checkIfSynchronizationRequired()));
	connect(m_remoteDirtyState, SIGNAL(entered()), this, SLOT(checkIfSynchronizationRequired()));

	connect(m_offlineState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_idleState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_putState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_getState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_localCleanState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_localDirtyState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_localCleaningState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_localCleaningDirtyState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_localFailedState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_remoteCleanState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_remoteDirtyState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_remoteCleaningState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_remoteCleaningDirtyState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(m_remoteFailedState, SIGNAL(entered()), this, SLOT(printConfiguration()));

	addState(globalState);

	setInitialState(globalState);
}

GaduRosterStateMachine::~GaduRosterStateMachine()
{
}

void GaduRosterStateMachine::printConfiguration()
{
	auto states = QStringList{};

	if (configuration().contains(m_offlineState))
		states.append("offline");
	if (configuration().contains(m_idleState))
		states.append("idle");
	if (configuration().contains(m_putState))
		states.append("put");
	if (configuration().contains(m_getState))
		states.append("get");
	if (configuration().contains(m_localCleanState))
		states.append("local-clean");
	if (configuration().contains(m_localDirtyState))
		states.append("local-dirty");
	if (configuration().contains(m_localCleaningState))
		states.append("local-cleaning");
	if (configuration().contains(m_localCleaningDirtyState))
		states.append("local-cleaning-dirty");
	if (configuration().contains(m_localFailedState))
		states.append("local-failed");
	if (configuration().contains(m_remoteCleanState))
		states.append("remote-clean");
	if (configuration().contains(m_remoteDirtyState))
		states.append("remote-dirty");
	if (configuration().contains(m_remoteCleaningState))
		states.append("remote-cleaning");
	if (configuration().contains(m_remoteCleaningDirtyState))
		states.append("remote-cleaning-dirty");
	if (configuration().contains(m_remoteFailedState))
		states.append("remote-failed");

	kdebugm(KDEBUG_INFO, "Gadu contact list state machine: [%s]\n", qPrintable(states.join(", ")));
}

void GaduRosterStateMachine::checkIfSynchronizationRequired()
{
	if (shouldPerformGet())
	{
		emit getRequested();
		emit performGet();
	}
	else if (shouldPerformPut())
	{
		emit putRequested();
		emit performPut();
	}
}

bool GaduRosterStateMachine::shouldPerformPut() const
{
	return configuration().contains(m_idleState) && configuration().contains(m_localDirtyState) && !configuration().contains(m_remoteDirtyState);
}

bool GaduRosterStateMachine::isPerformingPut() const
{
	return configuration().contains(m_putState);
}

bool GaduRosterStateMachine::shouldPerformGet() const
{
	return configuration().contains(m_idleState) && configuration().contains(m_remoteDirtyState);
}

bool GaduRosterStateMachine::isPerformingGet() const
{
	return configuration().contains(m_getState);
}

#include "moc_gadu-roster-state-machine.cpp"
